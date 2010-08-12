#ifndef _CHINESE_ANALYZER_H_
#define _CHINESE_ANALYZER_H_

#include <la/analyzer/CommonLanguageAnalyzer.h>

#include <cma_factory.h>
#include <knowledge.h>
#include <sentence.h>
#include <analyzer.h>

namespace la
{

class ChineseAnalyzer : public CommonLanguageAnalyzer<cma::Analyzer, cma::Sentence>
{
public:

    ChineseAnalyzer( const std::string knowledgePath, bool loadModel = true );

    ~ChineseAnalyzer();

    inline cma::Sentence* invokeMA( const char* input );

    inline void setIndexMode();

    inline void setLabelMode();

    /**
     * @brief   Set the analysis approach type, only for iCMA
     */
    inline void setAnalysisType( unsigned int type = 2 )
    {
        pA_->setOption( cma::Analyzer::OPTION_ANALYSIS_TYPE, type );
    }

    inline void setNBest( unsigned int num=2 )
    {
        pA_->setOption( cma::Analyzer::OPTION_TYPE_NBEST, num );
    }


//    inline bool isScFlSn( int morp )
//    {
//        return morp == flMorp_ || morp == snMorp_;
//    }
//
//    inline bool isAcceptedNoun( int morp )
//    {
//        if( morp < 0 || morp >= posSize_ )
//            return false;
//        return acceptedNouns_[ morp ];
//    }

protected:

    inline void resetAnalyzer();
    /**
     * Invoked by the last step of the constructor
     */
    inline void initAcceptedNouns();

    inline void addDefaultPOSList( vector<string>& posList );

private:

    int snMorp_;

    int posSize_;

    bool* acceptedNouns_;

};

}

#endif
