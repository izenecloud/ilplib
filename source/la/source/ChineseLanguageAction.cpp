/**
 * @file    ChineseLanguageAction.cpp
 * @author  Vernkin
 * @date    Oct 9, 2009
 * @details
 *  LanguageAction for Chinese language.
 */
#ifdef USE_IZENECMA
#include "la/ChineseLanguageAction.h"

#include <cma_factory.h>
#include <iostream>
using namespace wiselib;
using namespace std;
using namespace cma;

namespace la
{

ChineseLanguageAction::ChineseLanguageAction( const string& knowledgePath, bool loadModel )
        : pA_( 0 ), pAsyn_(0), acceptedNouns_(0)
{
    CMA_Factory* factory = CMA_Factory::instance();

    pA_ = factory->createAnalyzer();

    Knowledge* pK = 0;
    if( loadModel )
        pK = la::CMAKnowledge::getInstance(knowledgePath.c_str()).pKnowledge_;
    else
        pK = la::CMANoModelKnowledge::getInstance(knowledgePath.c_str()).pKnowledge_;

    pA_->setKnowledge( pK );

    // pAsyn_ is equals to pA_
    pAsyn_= pA_;

    flMorp_ = pA_->getCodeFromStr( CHINESE_FL );
    snMorp_ = pA_->getCodeFromStr( CHINESE_SN );
    initAcceptedNouns();
}

ChineseLanguageAction::~ChineseLanguageAction() {
    delete pA_;
    //delete pAsyn_;
    delete[] acceptedNouns_;
}

/**
 * In the same token, whether each Morpheme shared the same word offset
 */
bool ChineseLanguageAction::isSharedWordOffset()
{
    return false;
}

}

#endif
