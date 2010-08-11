/**
 * @file    CommonLanguageAnalyzer.h
 * @author  Kent, Vernkin
 * @date    Nov 23, 2009
 * @details
 *  Common Language Analyzer for Chinese/Japanese/Korean.
 */

#ifndef COMMONLANGUAGEANALYZER_H_
#define COMMONLANGUAGEANALYZER_H_

#include <la/analyzer/Analyzer.h>
#include <la/dict/SingletonDictionary.h>
#include <la/dict/UpdatableSynonymContainer.h>
#include <la/dict/UpdateDictThread.h>
#include <am/vsynonym/VSynonym.h>

#include <la/stem/Stemmer.h>

#include <util/ustring/UString.h>

namespace la
{

template <class LanguageAction, class BasicSentence>
class CommonLanguageAnalyzer : public Analyzer
{
public:

    CommonLanguageAnalyzer( const std::string knowledgePath, bool loadModel = true );

    virtual ~CommonLanguageAnalyzer();

    DECLARE_ANALYZER_METHODS

    void setIndexMode()
    {
        lat_->setIndexMode();
    }

    void setLabelMode()
    {
        lat_->setLabelMode();
    }

    /**
     * @brief   Set the analysis approach type, only for iCMA
     */
    inline void setAnalysisType( unsigned int type = 2 )
    {
        lat_->setAnalysisType( type );
    }

    /**
     * @brief Whether enable case-sensitive search
     * @param flag default value is true
     */
    virtual void setCaseSensitive( bool flag = true )
    {
        bCaseSensitive_ = flag;
        lat_->setCaseSensitive( flag );
    }

protected:

    template <typename IDManagerType>
    int analyze(IDManagerType* idm,
            const Term & input,
            TermIdList & output,
            analyzermode flags);

protected:

    LanguageAction*                       lat_;

    izenelib::am::VSynonymContainer*      pSynonymContainer_;
    izenelib::am::VSynonym*               pSynonymResult_;
    shared_ptr<UpdatableSynonymContainer> uscSPtr_;

    stem::Stemmer *                     pStemmer_;

    char * ustring_convert_buffer1_;
    char * ustring_convert_buffer_;

    int flMorp_;
    std::string flPOS_;

    int nniMorp_;
    std::string nniPOS_;

    int nnpMorp_;
    std::string nnpPOS_;

    int scMorp_;


    izenelib::util::UString::EncodingType encode_;

    /** In the same token, whether each Morpheme shared the same word offset */
    bool bSharedWordOffset_;
};


template <class LanguageAction, class BasicSentence>
CommonLanguageAnalyzer<LanguageAction, BasicSentence>::~CommonLanguageAnalyzer()
{
    delete lat_;
    delete pSynonymContainer_;
    delete pSynonymResult_;
    delete pStemmer_;

    delete ustring_convert_buffer_;
    delete ustring_convert_buffer1_;
}
//#define DEBUG_CLA


template <class LanguageAction, class BasicSentence>
CommonLanguageAnalyzer<LanguageAction, BasicSentence>::CommonLanguageAnalyzer(
    const std::string pKnowledgePath, bool loadModel )
    : Analyzer(),
    lat_( NULL ),
    pSynonymContainer_( NULL ),
    pSynonymResult_( NULL ),
    pStemmer_( NULL )
{
    // ( if possible) remove tailing path separator in the knowledgePath
    string knowledgePath = pKnowledgePath;
    if( knowledgePath.length() > 0 )
    {
        char klpLastChar = knowledgePath[ knowledgePath.length() - 1 ];
        if( klpLastChar == '/' || klpLastChar == '\\' )
            knowledgePath = knowledgePath.substr( 0, knowledgePath.length() - 1 );
    }

    // 0. INIT Language-Independent constant variable
    lat_ = new LanguageAction( knowledgePath, loadModel );
    flMorp_ = lat_->getFLMorp();
    flPOS_ = lat_->getFLPOS();
    nniMorp_ = lat_->getNNIMorp();
    nniPOS_ = lat_->getNNIPOS();
    nnpMorp_ = lat_->getNNPMorp();
    nnpPOS_ = lat_->getNNPPOS();
//    scMorp_ = lat_->getSCMorp();
    encode_ = lat_->getEncodeType();
    bSharedWordOffset_ = lat_->isSharedWordOffset();

    pSynonymContainer_ = izenelib::am::VSynonymContainer::createObject();
    if( pSynonymContainer_ == NULL )
    {
    }
    pSynonymResult_ = izenelib::am::VSynonym::createObject();
    if( pSynonymResult_ == NULL )
    {
    }

    string synonymPath = knowledgePath + "/synonym.txt";
    pSynonymContainer_->setSynonymDelimiter(" ");
    pSynonymContainer_->setWordDelimiter("_");
    if( pSynonymContainer_->loadSynonym( synonymPath.c_str() ) != 1 )
    {
        string msg = "Failed to load synonym dictionary from path: ";
        msg += knowledgePath;
        throw std::logic_error( msg );
    }

    // set updatable Synonym Dictionary
    uscSPtr_.reset( new UpdatableSynonymContainer( pSynonymContainer_, synonymPath ) );
    UpdateDictThread::staticUDT.addRelatedDict( synonymPath.c_str(), uscSPtr_ );

    pStemmer_ = new stem::Stemmer();
    pStemmer_->init(stem::STEM_LANG_ENGLISH);


    ustring_convert_buffer_ = new char[4096];
    ustring_convert_buffer1_ = new char[4096*4];

    // 2. SET DEFAULT SETTINGS

//
//    setGenerateCompNoun();
//    setNBest();
//    setLowDigitBound();
//    setCombineBoundNoun();
//    setVerbAdjStems();
//    setExtractChinese();
//    setExtractEngStem();
//    setIndexSynonym(false);
//    setSearchSynonym(true);
    setCaseSensitive(false);
//    bSpecialChars_ = false;

    setIndexMode(); // Index mode is set by default
}

template <class LanguageAction, class BasicSentence>
template <typename IDManagerType>
int CommonLanguageAnalyzer<LanguageAction, BasicSentence>::analyze(
    IDManagerType* idm, const Term & input, TermIdList & output, analyzermode flags)
{
    // prime terms is meaningless to Chinese
    if( flags & prime )
    {
        output.push_back(TermId());
        idm->getTermIdByTermString(input.text_, output.back().termid_);
        output.back().wordOffset_ = input.wordOffset_;
    }

    if( flags & second)
    {
        int localOffset = 0;
        input.text_.convertString(encode_, ustring_convert_buffer1_, 4096*4);
        BasicSentence* pE = lat_->getBasicSentence(ustring_convert_buffer1_);

        int listSize = pE->getListSize();
        int bestIdx = 0;//pE->getOneBestIndex(); FIXME the one best index maybe not correct
        for ( int i = 0; i < listSize; ++i )
        {
            bool isBestIndex = ( i == bestIdx );
            int count = pE->getCount( i );
            for ( int j = 0; j < count; j++ )
            {
                const char * lexicon = pE->getLexicon( i, j );
                size_t len = strlen(lexicon);

                if( len == 0) continue;

                int morpheme = pE->getPOS(i,j);

                // decide the offset
                int wOffset = input.wordOffset_;
                if( bSharedWordOffset_ == false )
                {
                    if( isBestIndex && j > 0 )
                    {
                        ++localOffset;
                    }
                    wOffset += localOffset;
                }


                if ( pE->isIndexWord(i, j) )
                {
                    char* termUstr = ustring_convert_buffer_;
                    size_t termUstrLen = sizeof(UString::CharT) * UString::toUcs2(encode_, lexicon, len,
                                         (UString::CharT*)ustring_convert_buffer_, 4096/sizeof(UString::CharT));
                    char* synonymUstr = termUstr;

                    // foreign language, e.g. English
                    if( (morpheme & flMorp_) == flMorp_)
                    {
                        char* lowercaseTermUstr = termUstr;
                        bool lowercaseIsDifferent = false;
                        /// TODO implement to_lower
                        /// UString::to_lower(ustring_convert_buffer, lower_ustring_buffer);
                        /// lowercaseTermUstr = lower_ustring_buffer;

                        if(bCaseSensitive_)
                        {
                            output.add(idm, termUstr, termUstrLen, wOffset);
                            if(bContainLower_ & lowercaseIsDifferent)
                            {
                                output.add(idm, lowercaseTermUstr, termUstrLen, wOffset);
                            }
                        }
                        else
                        {
                            output.add(idm, lowercaseTermUstr, termUstrLen, wOffset);
                        }

                        if(flags & stemming )
                        {
                            string stem_term;
                            pStemmer_->stem( lowercaseTermUstr, stem_term );
                            if( strcmp(stem_term.c_str(), lowercaseTermUstr) != 0 )
                            {
                                output.add(idm, stem_term.c_str(), stem_term.size(), wOffset);
                            }
                        }

                        synonymUstr = lowercaseTermUstr;
                    }
                    else
                    {
                        output.add(idm, termUstr, termUstrLen, wOffset);
                    }

                    if(flags & synonym )
                    {
                        pSynonymContainer_ = uscSPtr_->getSynonymContainer();
                        pSynonymContainer_->searchNgetSynonym( synonymUstr, pSynonymResult_ );
                        char * synonym = pSynonymResult_->getHeadWord(0);
                        if( synonym )
                        {
                            output.add(idm, synonym, strlen(synonym), wOffset);
                        }
                    }
                }
            }
        }
        return localOffset;
    }

    return 1;
}

}

#endif /* COMMONLANGUAGEANALYZER_H_ */
