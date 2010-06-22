/**
 * @file    KoreanLanguageAction.h
 * @author  Vernkin, MyungHyun Lee (Kent)
 * @date    Nov 23, 2009
 * @details
 *  Separated the EnglishLanguageAction from the KoreanLanguageAction. The two were in the same file.
 */
#ifdef USE_WISEKMA
#ifndef KOREANLANGUAGEACTION_H_
#define KOREANLANGUAGEACTION_H_

#include "la/CommonLanguageAnalyzer.h"

#include <wk_eojul.h>
#include <wk_analyzer.h>

#include <wk_pos.h>

namespace la
{

extern int sTagSetNoun_;
extern const int SC_FL_SN_TAGS;

class KoreanLanguageAction
{
public:
    KoreanLanguageAction( const std::string& knowledgePath, bool loadModel = true );
    virtual ~KoreanLanguageAction();

    inline int getFLMorp()
    {
        return kmaOrange::FL;
    }

    inline string getFLPOS()
    {
        return "FL";
    }

    inline int getNNIMorp()
    {
        return kmaOrange::NNI;
    }

    inline string getNNIPOS()
    {
        return "NNI";
    }

    inline int getNNPMorp()
    {
        return kmaOrange::NNP;
    }

    inline string getNNPPOS()
    {
        return "NNP";
    }

    inline int getSCMorp()
    {
        return kmaOrange::SC;
    }

    inline izenelib::util::UString::EncodingType getEncodeType()
    {
        return izenelib::util::UString::CP949;
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
        //pAsyn_->setOption(WKO_OPTION_EXTRACT_NUM, num );
    }
    /**
     * @brief   Whether to combine number dependent noun to extracted term
     */
    inline void setCombineBoundNoun( bool flag=false )
    {
        pA_->setOption(kmaOrange::WKO_OPTION_COMBINE_BOUND_NOUN, (flag) ? 1 : 0);
        //pAsyn_->setOption(WKO_OPTION_COMBINE_BOUND_NOUN, (flag) ? 1 : 0);
    }

    /**
     * @brief   Whether or not to extract stem terms of verb and adj words
     */
    inline void setVerbAdjStems( bool flag=false )
    {
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_VERB_STEMS, (flag) ? 1 : 0);
        //pAsyn_->setOption(WKO_OPTION_EXTRACT_VERB_STEMS, (flag) ? 1 : 0);
    }

    /*
    inline void KoreanAnalyzer::setSearchMode()
    {
        if( pA_ == NULL )
        {
            throw std::logic_error( "KoreanAnalyzer::setSearchMode() is call with pA_ NULL" );
        }
        resetWKAnalyzer();
        pA_->setExOption( kmaOrange::COMPOSEAFFIX );

        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_ALPHA, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_UNKNOWN, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_BOUND_NOUN, 1);
        pA_->setOption(kmaOrange::WKO_OPTION_N_BEST, 1);

        //mode_ = SEARCHING;
    }
    */

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

    inline kmaOrange::WK_Eojul* getSynBasicSentence( const char* input )
    {
        pEsyn_->initialize();
        pEsyn_->setString( input );
        pAsyn_->runWithEojul();
        return pEsyn_;
    }

    inline kmaOrange::WK_Eojul* getBasicSentence( const char* input )
    {
        pE_->initialize();
        pE_->setString( input );
        pA_->runWithEojul();
        return pE_;
    }

    inline bool isScFlSn( int morp )
    {
        return morp & SC_FL_SN_TAGS == morp;
    }

    inline bool isAcceptedNoun( int morp )
    {
        return (morp & sTagSetNoun_ ) == morp;
    }

    inline void setCaseSensitive( bool flag = true )
    {
        int val = flag ? 1 : 0;
        pA_->setOption( kmaOrange::WKO_OPTION_CASE_SENSITIVE, val );
    }

    /**
     * @brief   Set the analysis approach type
     */
    inline void setAnalysisType( unsigned int type = 2 )
    {
        // empty
    }

    /**
     * In the same token, whether each Morpheme shared the same word offset
     */
    inline bool isSharedWordOffset()
    {
        return true;
    }

private:
    //// PRIVATE
    /*
    inline void resetAnalyzer()
    {
    // LOG: this method somehow disrupted KMA's behavior in getting index terms. 
    // Terms such as "80" that should be Index terms were not passing isIndexTerm
        pA_->setOption( kmaOrange::WKO_OPTION_N_BEST, 0 );
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_ALPHA, 0 );
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_NUM, 0 );
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_VERB_STEMS, 0 );
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_ADNOMINAL, 0 );
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_ADVERB, 0 );
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_INTERJECTION, 0 );
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_UNKNOWN, 0 );
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_BOUND_NOUN, 0 );
    }
    */

private:
    kmaOrange::WK_Eojul*                pE_;
    kmaOrange::WK_Analyzer*             pA_;
    kmaOrange::WK_Eojul*                pEsyn_;     // Eojul for processing synonyms
    kmaOrange::WK_Analyzer*             pAsyn_;     // Analyzer for processing synonyms

};

}

#endif /* KOREANLANGUAGEACTION_H_ */
#endif
