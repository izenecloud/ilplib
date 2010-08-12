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

template <typename MAAnalyzerType, typename MASentenceType>
class CommonLanguageAnalyzer : public Analyzer
{
public:

    enum mode {indexmode, labelmode};

    CommonLanguageAnalyzer( const std::string knowledgePath, bool loadModel = true );

    virtual ~CommonLanguageAnalyzer();

    virtual MASentenceType* invokeMA( const char* sentence ) = 0;

    virtual void setIndexMode() = 0;

    virtual void setLabelMode() = 0;

    virtual void setNBest(unsigned int nbest = 2) {};

    virtual void setCaseSensitive(bool casesensitive = true, bool containlower = true)
    {
        bCaseSensitive_ = casesensitive;
        bContainLower_ = containlower;
    };

//    /// Set token_,
//    virtual bool nextToken();

    DECLARE_ANALYZER_METHODS

protected:

    template <typename IDManagerType>
    int analyze(IDManagerType* idm,
            const Term & input,
            TermIdList & output,
            analyzermode flags);

protected:

    izenelib::am::VSynonymContainer*      pSynonymContainer_;
    izenelib::am::VSynonym*               pSynonymResult_;
    shared_ptr<UpdatableSynonymContainer> uscSPtr_;

    stem::Stemmer *                     pStemmer_;

    char * ustring_convert_buffer1_;
    char * ustring_convert_buffer_;

    /** MA specific data, you must initiailize them in initializeMA */
    MAAnalyzerType *                      pA_;
    MASentenceType *                      pS_;

    char * token_;
    int len_;
    int morpheme_;
    bool fl_;


    int flMorp_;
    std::string flPOS_;

    int nniMorp_;
    std::string nniPOS_;

    int nnpMorp_;
    std::string nnpPOS_;

    izenelib::util::UString::EncodingType encode_;

    bool bSharedWordOffset_;

    bool bCaseSensitive_;

    bool bContainLower_;
};


template <typename MAAnalyzerType, typename MASentenceType>
CommonLanguageAnalyzer<MAAnalyzerType, MASentenceType>::CommonLanguageAnalyzer(
    const std::string pKnowledgePath, bool loadModel )
    : Analyzer(),
    pSynonymContainer_( NULL ),
    pSynonymResult_( NULL ),
    pStemmer_( NULL ),
    pA_( NULL ),
    pS_( NULL )
{
    // ( if possible) remove tailing path separator in the knowledgePath
    string knowledgePath = pKnowledgePath;
    if( knowledgePath.length() > 0 )
    {
        char klpLastChar = knowledgePath[ knowledgePath.length() - 1 ];
        if( klpLastChar == '/' || klpLastChar == '\\' )
            knowledgePath = knowledgePath.substr( 0, knowledgePath.length() - 1 );
    }

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
}

template <typename MAAnalyzerType, typename MASentenceType>
CommonLanguageAnalyzer<MAAnalyzerType, MASentenceType>::~CommonLanguageAnalyzer()
{
    delete pA_;
    delete pS_;

    delete pSynonymContainer_;
    delete pSynonymResult_;
    delete pStemmer_;

    delete ustring_convert_buffer_;
    delete ustring_convert_buffer1_;
}

template <typename MAAnalyzerType, typename MASentenceType>
template <typename IDManagerType>
int CommonLanguageAnalyzer<MAAnalyzerType, MASentenceType>::analyze(
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
        MASentenceType* pE = invokeMA(ustring_convert_buffer1_);

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
