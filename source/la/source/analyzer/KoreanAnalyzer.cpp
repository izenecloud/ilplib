#include <la/analyzer/KoreanAnalyzer.h>

using namespace kmaOrange;

namespace la
{

    KoreanAnalyzer::KoreanAnalyzer( const std::string knowledgePath, bool loadModel)
        : CommonLanguageAnalyzer(knowledgePath, loadModel),
        pA_(NULL)
//        pS_(NULL)
    {
        // 1. INIT INSTANCES
        kmaOrange::WK_Knowledge* pK = KMAKnowledge::getInstance(knowledgePath.c_str()).pKnowledge_;
        if( pK == NULL )
        {
            string msg = "Failed to load KMA knowledge: ";
            msg += knowledgePath;
            throw std::logic_error( msg );
        }
//        pS_ = kmaOrange::WK_Eojul::createObject();
//        if( pS_ == NULL )
//        {
//            //TODO: CATCH ALL EXCEPTIONS
//        }
//        pA_ = kmaOrange::WK_Analyzer::createObject( pK, pS_ );
        pA_ = kmaOrange::WK_Analyzer::createObject( pK );
        if( pA_ == NULL )
        {
        }

        encode_ = UString::CP949;

        setNBest();
        setLowDigitBound();
        setVerbAdjStems();
        setCaseSensitive(false);

        setIndexMode(); // Index mode is set by default
    }

    KoreanAnalyzer::~KoreanAnalyzer()
    {
        delete pA_;
//        delete pS_;
    }

    void KoreanAnalyzer::setIndexMode()
    {
        if( pA_ == NULL )
        {
            throw std::logic_error( "KoreanAnalyzer::setIndexMode() is call with pA_ NULL" );
        }
        //resetAnalyzer();
        //pA_->setExOption( kmaOrange::COMPOSEAFFIX );
        pA_->setExOption( kmaOrange::DOCUMENT );

        pA_->setOption( kmaOrange::WKO_OPTION_N_BEST, 2 );

        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_ALPHA, 1 );
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_UNKNOWN, 1 );
        pA_->setOption( kmaOrange::WKO_OPTION_EXTRACT_BOUND_NOUN, 1 );
    }

    void KoreanAnalyzer::setLabelMode()
    {
        if( pA_ == NULL )
        {
            throw std::logic_error( "KoreanAnalyzer::setLabelMode() is call with pA_ NULL" );
        }
        //resetAnalyzer();
        // pA_->setExOption( kmaOrange::COMPOSEAFFIX );
        pA_->setExOption( kmaOrange::DOCUMENT );

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

}
