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

    bool digit = false;
    bool alpha = false;
    int begin = 0;
    int end = 0;
    for( size_t i = 0; i < input.text_.length(); ++i )
    {
        UString::CharT ch = input.text_.at(i);

        if(UString::isThisDigitChar(ch))
        {
            if(!digit)
            {
                if(alpha)
                {
                    func(data, input.text_.c_str()+begin, end-begin+1, offset++, Term::EnglishPOS, Term::AND , 0, false);
                    alpha = false;
                }
                begin = i;
                digit = true;
            }
            end = i;
            continue;
        }
        else
        {
            if(digit)
            {
                func(data, input.text_.c_str()+begin, end-begin+1, offset++, Term::DigitPOS, Term::AND , 0, false);
                digit = false;
            }
        }

        if(UString::isThisAlphaChar(ch))
        {
            if(!alpha)
            {
                begin = i;
                alpha = true;
            }
            end = i;
            continue;
        }
        else
        {
            if(alpha)
            {
                func(data, input.text_.c_str()+begin, end-begin+1, offset++, Term::EnglishPOS, Term::AND , 0, false);
                alpha = false;
            }
        }

        if(UString::isThisSpaceChar(ch))
        {
            continue;
        }
        else if(UString::isThisPunctuationChar(ch))
        {
            func(data, input.text_.c_str()+i, 1, offset++, Term::SpecialCharPOS, Term::AND , 0, true);
        }
        else if(UString::isThisChineseChar(ch))
        {
            func(data, input.text_.c_str()+i, 1, offset++, Term::ChinesePOS, Term::AND , 0, false);
        }
        else if(UString::isThisKoreanChar(ch))
        {
            func(data, input.text_.c_str()+i, 1, offset++, Term::KoreanPOS, Term::AND , 0, false);
        }
        else
        {
            func(data, input.text_.c_str()+i, 1, offset++, Term::OtherPOS, Term::AND , 0, false);
        }
    }
    if(digit)
    {
        func(data, input.text_.c_str()+begin, end-begin+1, offset++, Term::DigitPOS, Term::AND , 0, false);
    }
    if(alpha)
    {
        func(data, input.text_.c_str()+begin, end-begin+1, offset++, Term::EnglishPOS, Term::AND , 0, false);
    }
    return 0;
};

}
