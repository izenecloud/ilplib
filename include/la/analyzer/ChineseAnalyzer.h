#ifndef _CHINESE_ANALYZER_H_
#define _CHINESE_ANALYZER_H_

#include <la/analyzer/CommonLanguageAnalyzer.h>
#include <la/analyzer/ChineseLanguageAction.h>

namespace la
{
    class ChineseAnalyzer : public CommonLanguageAnalyzer<cma::Analyzer, cma::Sentence>
    {
    public:

        inline void setIndexMode()
        {
            if( pA_ == NULL )
            {
                throw std::logic_error( "ChineseAnalyzer::setIndexMode() is call with pA_ NULL" );
            }
            resetAnalyzer();

            //pA_->setOption( cma::Analyzer::OPTION_TYPE_NBEST, 1 );

            vector<string> posList;
            addDefaultPOSList(posList);

            // unknown
            posList.push_back("un");

            pA_->setIndexPOSList(posList);
        }

        inline void setLabelMode()
        {
            if( pA_ == NULL )
            {
                throw std::logic_error( "ChineseAnalyzer::setLabelMode() is call with pA_ NULL" );
            }
            resetAnalyzer();

            //pA_->setOption( cma::Analyzer::OPTION_TYPE_NBEST, 1 );

            // to index all the POS
            pA_->resetIndexPOSList( true );
        }

        /**
         * @brief   Set the analysis approach type, only for iCMA
         */
        inline void setAnalysisType( unsigned int type = 2 )
        {
            pA_->setOption( cma::Analyzer::OPTION_ANALYSIS_TYPE, type );
        }



    };
}

#endif
