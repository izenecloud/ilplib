#ifndef _KOREAN_ANALYZER_H_
#define _KOREAN_ANALYZER_H_

#include <la/analyzer/CommonLanguageAnalyzer.h>

#include <wk_eojul.h>
#include <wk_analyzer.h>
#include <wk_pos.h>


#define _CLA_INSERT_INDEX_USTR( term_it, termList, text, wordOffset, pos, morpheme ) \
    term_it = termList.insert( termList.end(), globalNewTerm_ ); \
    term_it->text_ = text; \
    term_it->wordOffset_ = wordOffset; \
    term_it->pos_ = pos; \
    term_it->morpheme_ = morpheme

#define _CLA_INSERT_INDEX_STR( term_it, termList, text, wordOffset, pos, morpheme ) \
    term_it = termList.insert( termList.end(), globalNewTerm_ ); \
    term_it->text_.assign( text, encode_ ); \
    term_it->wordOffset_ = wordOffset; \
    term_it->pos_ = pos; \
    term_it->morpheme_ = morpheme

namespace la
{

class KoreanAnalyzer : public CommonLanguageAnalyzer
{

public:

    KoreanAnalyzer( const std::string knowledgePath, bool loadModel = true )
        : CommonLanguageAnalyzer(knowledgePath, loadModel),
              pA_(NULL),
      pS_(NULL)
    {
        // 1. INIT INSTANCES
        kmaOrange::WK_Knowledge* pK = KMAKnowledge::getInstance(knowledgePath.c_str()).pKnowledge_;
        if( pK == NULL )
        {
            string msg = "Failed to load KMA knowledge: ";
            msg += knowledgePath;
            throw std::logic_error( msg );
        }
        pS_ = kmaOrange::WK_Eojul::createObject();
        if( pS_ == NULL )
        {
            //TODO: CATCH ALL EXCEPTIONS
        }
        pA_ = kmaOrange::WK_Analyzer::createObject( pK, pS_ );
        if( pA_ == NULL )
        {
        }

        flMorp_ = kmaOrange::FL;
        flPOS_ = "FL";

        nniMorp_ = kmaOrange::NNI;
        nniPOS_ = "NNI";

        nnpMorp_ = kmaOrange::NNP;
        nnpPOS_ = "NNP";

        encode_ = UString::CP949;

        bSharedWordOffset_ = true;

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

    ~KoreanAnalyzer()
    {
        delete pA_;
        delete pS_;
    }

    inline kmaOrange::WK_Eojul* invokeMA( const char* input )
    {
        pS_->initialize();
        pS_->setString( input );
        pA_->runWithEojul();
        return pS_;
    }

    inline void setIndexMode()
    {
        if( pA_ == NULL )
        {
            throw std::logic_error( "KoreanAnalyzer::setIndexMode() is call with pA_ NULL" );
        }
        //resetAnalyzer();
        pA_->setExOption( kmaOrange::COMPOSEAFFIX );

        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_ALPHA, 1 );
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_UNKNOWN, 1 );
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_BOUND_NOUN, 1 );
        pA_->setOption( kmaOrange::WKO_OPTION_N_BEST, 2 );

    }

    inline void setLabelMode()
    {
        if( pA_ == NULL )
        {
            throw std::logic_error( "KoreanAnalyzer::setLabelMode() is call with pA_ NULL" );
        }
        //resetAnalyzer();
        pA_->setExOption( kmaOrange::COMPOSEAFFIX );

        /*
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_ALPHA, 1 );
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_UNKNOWN, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_BOUND_NOUN, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_VERB_STEMS, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_ADNOMINAL, 1);
        */
        pA_->setOption(kmaOrange::WKO_OPTION_N_BEST, 1);

        // LOG: changed option setting for label mode to test Korean TG Label generation
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_ALPHA, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_NUM, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_VERB_STEMS, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_ADNOMINAL, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_ADVERB, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_INTERJECTION, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_UNKNOWN, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_BOUND_NOUN, 1);

    }

    inline void setNBest( unsigned int num=2 )
    {
        pA_->setOption(kmaOrange::WKO_OPTION_N_BEST, num);
    }

    /**
     * @brief  Sets the lower bound digit limit of the numbers extracted
     */
    inline void setLowDigitBound( unsigned int num=1 )
    {
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_NUM, num );
    }
    /**
     * @brief   Whether to combine number dependent noun to extracted term
     */
    inline void setCombineBoundNoun( bool flag=false )
    {
        pA_->setOption(kmaOrange::WKO_OPTION_COMBINE_BOUND_NOUN, (flag) ? 1 : 0);
    }

    /**
     * @brief   Whether or not to extract stem terms of verb and adj words
     */
    inline void setVerbAdjStems( bool flag=false )
    {
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_VERB_STEMS, (flag) ? 1 : 0);
    }

    inline void setCaseSensitive( bool flag = true )
    {
        int val = flag ? 1 : 0;
        pA_->setOption( kmaOrange::WKO_OPTION_CASE_SENSITIVE, val );
    }

    /**
     * @brief   Sets whether to generate compound noun with two adjacent extracted nouns
     *          (only indexing)
     */
    inline void setGenerateCompNoun( bool flag=false )
    {
        bGenCompNoun_ = flag;
    }

    // --- SPECIALCHAR ---

    /**
     * @brief   Set special characters to process. The configured characters, of course,
     *          should be passed to KoreanAnalyzer. (normall with "allow" option to Tokenizer)
     */
    void setSpecialChars( const string & chars )
    {
        const char *o = chars.c_str();

        // init special chars table
        memset(specialCharTable_, 0x00, sizeof(char) * 128);

        if (*o)
        {
            // turn on special chars option.
            bSpecialChars_ = true;

            while (*o)
            {
                if( (unsigned char)*o < 128 )
                {
                    specialCharTable_[(unsigned char)*o] = 1;
                }
                o++;
            }
        }
    }

//    int sTagSetNoun_  = (NNG|NFG|NNB|NNP|NNU|NNR|NP|NU|NNI|NNC|NFU); //(N_|UW);
//
//    const int SC_FL_SN_TAGS = (SC|FL|SN);
//
//    inline bool isScFlSn( int morp )
//    {
//        return morp & SC_FL_SN_TAGS == morp;
//    }
//


protected:

    /// Parse given input
    void parse(const char* sentence, int initoffset) {};

    /// Fill token_, len_, offset_, morpheme_
    bool nextToken() {return false;}

    /// whether morpheme_ indicates foreign language
    bool isFL() {return false;}

    inline bool isScFlSn( int morp );

    inline bool isAcceptedNoun( int morp );


    inline void combineSpecialChar( kmaOrange::WK_Eojul * pE, int i, int count, TermList& termList )
    {
        TermList::iterator term_it;
        string specialStr;
        int k;
        for( int j = 0; j < count; ++j )
        {
            unsigned int pos = pE->getPOS(i,j);
            if( ( pos & scMorp_ ) == pos )
            {
                if( (k = getSpecialCharsString(pE, i, j, specialStr)) >= 0 )
                {
                    _CLA_INSERT_INDEX_STR( term_it, termList, specialStr, k, nniPOS_, nniMorp_ );
                    j = k;
                }
            }
        }
    }


    int getSpecialCharsString(
        kmaOrange::WK_Eojul * pEojul,
        int listi, int counti,
        string & specialStr );

    void generateCompundNouns(
        kmaOrange::WK_Eojul * pE,
        const string& inputstr,
        int i, int count,
        unsigned int wordOffset,
        TermList& termList );

    void addChineseTerm(
        const char * pTerm,
        const unsigned int wordOffset,
        TermList & tlist );

private:

    kmaOrange::WK_Analyzer * pA_;

    kmaOrange::WK_Eojul * pS_;

    int flMorp_;
    std::string flPOS_;

    int nniMorp_;
    std::string nniPOS_;

    int nnpMorp_;
    std::string nnpPOS_;

    int scMorp_;

    // ---- RELATED TO "specialchar" IN LAMANAGER
    /// @brief  Whether to handle special characters.
    /// Adjacent alphabets, numeric characters, and special characters will be concatenated.
    bool bSpecialChars_;

    /// @brief  Table for special characters that should be considered for analyzing.
    char specialCharTable_[128];


    /// @brief  Genearte compound noun out of two adjacent extracted noun terms
    /// Default: false
    bool            bGenCompNoun_;


    /// @brief  Can choose the number of possible extractions to generate.
    /// Default: 2
    //unsigned int    nBest_;


    /// @brief  Whether to add Chinese characters additional to their Korean counterpart
    /// Default: false
    bool            bExtractChinese_;     //H

bool bSharedWordOffset_;

    Term globalNewTerm_;

};
}

#endif
