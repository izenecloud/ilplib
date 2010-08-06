/**
 * @brief   Defines Analyzerclass
 * @file    Analyzer.cpp
 * @author  Vernkin
 * @date    2010.02.24
 */

#include <la/analyzer/Analyzer.h>

using namespace izenelib::ir::idmanager;

namespace la
{
    const unsigned char Analyzer::ANALYZE_NONE_ = 0x00;
    const unsigned char Analyzer::ANALYZE_PRIME_    = 0x01;
    const unsigned char Analyzer::ANALYZE_SECOND_   = 0x02;
    const unsigned char Analyzer::ANALYZE_ALL_  = 0x03;

    int Analyzer::analyze(IDManager* idm, const Term & input, TermIdList & output) {
        return analyze(idm, input, output, retFlag_idx_);
    }

    int Analyzer::analyze_index( const TermList & input, TermList & output )
    {
        return analyze_index( input, output, retFlag_idx_ );
    }

    int Analyzer::analyze_search( const TermList & input, TermList & output )
    {
        return analyze_search( input, output, retFlag_sch_ );
    }

}
