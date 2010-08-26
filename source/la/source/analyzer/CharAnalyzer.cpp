/**
 * @brief A character based analyzer
 * @author Wei
 * @date 2010.08.24
 */

#include <la/analyzer/CharAnalyzer.h>

#include <util/ustring/UString.h>

using namespace izenelib::util;
using namespace std;

namespace la
{

CharAnalyzer::CharAnalyzer() : Analyzer()
{
}

CharAnalyzer::~CharAnalyzer()
{
}

int CharAnalyzer::analyze_impl( const Term& input, void* data, HookType func )
{
    int offset = 0;

    bool digitOrAlpha = false;
    int begin = 0;
    int end = 0;
    for( size_t i = 0; i < input.text_.length(); ++i )
    {
        UString::CharT ch = input.text_.at(i);

        if(UString::isThisDigitChar(ch) || UString::isThisAlphaChar(ch))
        {
            if(!digitOrAlpha)
            {
                begin = i;
                digitOrAlpha = true;
            }
            end = i;
        }
        else
        {
            if(digitOrAlpha)
            {
                func(data, input.text_.c_str()+begin, end-begin+1, offset++, NULL, Term::AND , 0, false);
                digitOrAlpha = false;
            }

            if(UString::isThisSpaceChar(ch))
            {
                continue;
            }
            else
            {
                func(data, input.text_.c_str()+i, 1, offset++, NULL, Term::AND , 0, UString::isThisPunctuationChar(ch));
            }
        }
    }
    if(digitOrAlpha)
    {
        func(data, input.text_.c_str()+begin, end-begin+1, offset++, NULL, Term::AND , 0, false);
    }
    return 0;
};

}
