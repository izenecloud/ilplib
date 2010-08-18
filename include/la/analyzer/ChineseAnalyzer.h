#ifndef _CHINESE_ANALYZER_H_
#define _CHINESE_ANALYZER_H_

#include <la/analyzer/CommonLanguageAnalyzer.h>

#include <cma_factory.h>
#include <knowledge.h>
#include <sentence.h>
#include <analyzer.h>

namespace la
{

class ChineseAnalyzer : public CommonLanguageAnalyzer {
public:

    enum ChineseAnalysisType{
        maximum_entropy = 1,
        maximum_match = 2,
        minimum_match = 3,
    };

    ChineseAnalyzer( const std::string knowledgePath, bool loadModel = true );

    ~ChineseAnalyzer();

    void setIndexMode();

    void setLabelMode();

    /**
     * @brief   Set the analysis approach type, only for iCMA
     */
    inline void setAnalysisType( ChineseAnalysisType type )
    {
        pA_->setOption( cma::Analyzer::OPTION_ANALYSIS_TYPE, type );
    }

    inline void setNBest( unsigned int num=2 )
    {
        pA_->setOption( cma::Analyzer::OPTION_TYPE_NBEST, num );
    }

protected:

    /// Parse given input
    inline void parse(const char* sentence, int sentenceOffset)
    {
        pS_->setString( sentence );
        pA_->runWithSentence( *pS_ );

        sentenceOffset_ = sentenceOffset;
        localOffset_ = 0;

        listIndex_ = 0;
        lexiconIndex_ = 0;

        token_ = NULL;
        len_ = 0;
        morpheme_ = 0;
        offset_ = 0;
        needIndex_ = false;
    }

    /// Fill token_, len_, morpheme_
    inline bool nextToken()
    {
        if(listIndex_ == pS_->getListSize()) {
            token_ = NULL;
            len_ = 0;
            morpheme_ = 0;
            offset_ = 0;
            needIndex_ = false;
            return false;
        }

        token_ = pS_->getLexicon(listIndex_, lexiconIndex_);
        len_ = strlen(token_);
        morpheme_ = pS_->getPOS(listIndex_, lexiconIndex_);
        offset_ = sentenceOffset_ + localOffset_;
        needIndex_ = pS_->isIndexWord(listIndex_, lexiconIndex_);

        ++ localOffset_;
        ++ lexiconIndex_;

        while(lexiconIndex_ == pS_->getCount(listIndex_)) {
            ++ listIndex_;
            lexiconIndex_ = 0;
            localOffset_ = 0;
        }

        return true;
    }

    inline bool isFL()
    {
        return morpheme_ == flMorp_;
    }

    inline bool isSpecialChar()
    {
        return morpheme_ == scMorp_;
    }

private:

    inline void resetAnalyzer();

    inline void addDefaultPOSList( vector<string>& posList );

private:

    cma::Analyzer * pA_;

    cma::Sentence * pS_;

    int sentenceOffset_;

    int localOffset_;

    int listIndex_;

    int lexiconIndex_;

    int flMorp_;

    int scMorp_;
};

}

#endif
