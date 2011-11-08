/**
 * @brief A tokenizer based analyzer
 * @author Wei
 * @date 2010.08.24
 */

#include <la/analyzer/TokenAnalyzer.h>

#include <util/ustring/UString.h>

using namespace izenelib::util;
using namespace std;

namespace la
{

TokenAnalyzer::TokenAnalyzer() : Analyzer()
{
}

TokenAnalyzer::~TokenAnalyzer()
{
}

int TokenAnalyzer::analyze_impl( const Term& input, void* data, HookType func )
{
    int offset = 0;

    tokenizer_.tokenize(input.text_);
    while (tokenizer_.nextToken() )
    {
        func(data, tokenizer_.getToken(), tokenizer_.getLength(),
             offset, NULL, Term::AND, 0, tokenizer_.isDelimiter() );
        offset++;
    }
    return offset;
};

}
