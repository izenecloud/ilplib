#include <la/analyzer/JapaneseAnalyzer.h>


// Foreign Language is Japanese POS
#define JAPANESE_FL "S-A"

// Special character is Japanese POS
#define JAPANESE_SC "S-G"

namespace la
{

JapaneseAnalyzer::JapaneseAnalyzer( const std::string knowledgePath)
    : CommonLanguageAnalyzer(),
      pA_(NULL), pS_(NULL), input_string_buffer_size_(4096*4)
{
    bChinese_ = true;
    jma::JMA_Factory* factory = jma::JMA_Factory::instance();

    pA_ = factory->createAnalyzer();

    jma::Knowledge* pK = la::JMAKnowledge::getInstance(knowledgePath.c_str()).pKnowledge_;

    pA_->setKnowledge( pK );

    pS_ = new jma::Sentence();

    flMorp_ = pA_->getCodeFromStr( JAPANESE_FL );

    scMorp_ = pA_->getCodeFromStr( JAPANESE_SC );

    setCaseSensitive(false);

    setIndexMode(); // Index mode is set by default

    input_string_buffer_ = new char[input_string_buffer_size_];
    output_ustring_buffer_ = new UString::CharT[term_ustring_buffer_limit_];

}

JapaneseAnalyzer::~JapaneseAnalyzer()
{
    delete pA_;
    delete pS_;

    delete input_string_buffer_;
    delete output_ustring_buffer_;
}

void JapaneseAnalyzer::setIndexMode()
{
    if( pA_ == NULL )
    {
        throw std::logic_error( "JapaneseAnalyzer::setIndexMode() is call with pA_ NULL" );
    }

    pA_->setOption( jma::Analyzer::OPTION_TYPE_NBEST, 1 );
    pA_->setOption( jma::Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN, 0 );
    pA_->setOption( jma::Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY, 0 );
    pA_->setOption( jma::Analyzer::OPTION_TYPE_CONVERT_TO_LOWER_CASE, 0 );
}

void JapaneseAnalyzer::setLabelMode()
{
    if( pA_ == NULL )
    {
        throw std::logic_error( "JapaneseAnalyzer::setLabelMode() is call with pA_ NULL" );
    }

    pA_->setOption( jma::Analyzer::OPTION_TYPE_NBEST, 1 );
    pA_->setOption( jma::Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY, 1 );
    pA_->setOption( jma::Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN, 1 );
    pA_->setOption( jma::Analyzer::OPTION_TYPE_CONVERT_TO_LOWER_CASE, 0 );	
}


}
