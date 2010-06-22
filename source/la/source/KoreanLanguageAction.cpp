/**
 * @file    KoreanLanguageAction.cpp
 * @author  Vernkin, MyungHyun Lee (Kent)
 * @date    Nov 23, 2009
 * @details
 *  Separated the EnglishLanguageAction from the KoreanLanguageAction. The two were in the same file.
 */
#ifdef USE_WISEKMA
#include "la/KoreanLanguageAction.h"

namespace la
{

using namespace izenelib::util;
using namespace kmaOrange;
using namespace std;

int sTagSetNoun_  = (NNG|NFG|NNB|NNP|NNU|NNR|NP|NU|NNI|NNC|NFU); //(N_|UW);

const int SC_FL_SN_TAGS = (SC|FL|SN);

KoreanLanguageAction::KoreanLanguageAction( const string& knowledgePath, bool loadModel )
    : pE_(NULL),
      pA_(NULL),
      pEsyn_(NULL),
      pAsyn_(NULL)
{
    // 1. INIT INSTANCES
    kmaOrange::WK_Knowledge* pK = KMAKnowledge::getInstance(knowledgePath.c_str()).pKnowledge_;
    if( pK == NULL )
    {
        string msg = "Failed to load KMA knowledge: ";
        msg += knowledgePath;
        throw std::logic_error( msg );
    }
    pE_ = kmaOrange::WK_Eojul::createObject();
    if( pE_ == NULL )
    {
        //TODO: CATCH ALL EXCEPTIONS
    }
    pA_ = kmaOrange::WK_Analyzer::createObject( pK, pE_ );
    if( pA_ == NULL )
    {
    }

    pEsyn_ = kmaOrange::WK_Eojul::createObject();
    if( pEsyn_ == NULL )
    {
    }
    pAsyn_ = kmaOrange::WK_Analyzer::createObject( pK, pEsyn_ );
    if( pAsyn_ == NULL )
    {
    }

    // 2. SET DEFAULT SETTINGS
    pAsyn_->setExOption( kmaOrange::COMPOSEAFFIX );
    pAsyn_->setOption(kmaOrange::WKO_OPTION_EXTRACT_ALPHA, 1);
    pAsyn_->setOption(kmaOrange::WKO_OPTION_EXTRACT_UNKNOWN, 1);
    pAsyn_->setOption(kmaOrange::WKO_OPTION_EXTRACT_BOUND_NOUN, 1);
    pAsyn_->setOption(kmaOrange::WKO_OPTION_N_BEST, 1);
}

KoreanLanguageAction::~KoreanLanguageAction() {
    if( pE_ != NULL )
        delete pE_;
    if( pA_ != NULL )
        delete pA_;
    if( pEsyn_ != NULL )
        delete pEsyn_;
    if( pAsyn_ != NULL )
        delete pAsyn_;
}

}
#endif
