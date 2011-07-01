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

CharAnalyzer::CharAnalyzer()
    : Analyzer(),
      isSeparateAll( true ),
      isCaseSensitive( true )
{
}

CharAnalyzer::~CharAnalyzer()
{
}

void CharAnalyzer::setSeparateAll( bool flag )
{
    isSeparateAll = flag;
}

int CharAnalyzer::analyze_impl( const Term& input, void* data, HookType func )
{
    if( isSeparateAll == true )
        return separate_all( input, data, func );
    return separate_part( input, data, func );
}

int CharAnalyzer::separate_part( const Term& input, void* data, HookType func )
{
    int offset = 0;

    bool digit = false;
    bool alpha = false;
    int begin = 0;
    int end = 0;
    for( size_t i = 0; i < input.text_.length(); ++i )
    {
        UString::CharT ch = input.text_.at(i);

        if(UString::isThisNumericChar(ch))
        {
            if(!digit)
            {
                if(alpha)
                {
                    tmpStr_ = UString(input.text_.c_str()+begin, input.text_.c_str()+end+1);
                    if (!isCaseSensitive)
                    {
                        tmpStr_.toLowerString();
                    }
                    func(data, tmpStr_.c_str(), end-begin+1, offset++, Term::EnglishPOS, Term::AND , 0, false);
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
                tmpStr_ = UString(input.text_.c_str()+begin, input.text_.c_str()+end+1);
                if (!isCaseSensitive)
                {
                    tmpStr_.toLowerString();
                }
                func(data, tmpStr_.c_str(), end-begin+1, offset++, Term::EnglishPOS, Term::AND , 0, false);
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
        tmpStr_ = UString(input.text_.c_str()+begin, input.text_.c_str()+end+1);
        if (!isCaseSensitive)
        {
            tmpStr_.toLowerString();
        }
        func(data, tmpStr_.c_str(), end-begin+1, offset++, Term::EnglishPOS, Term::AND , 0, false);
    }
    return 0;
}

int CharAnalyzer::separate_all(  const Term& input, void* data, HookType func )
{
    int offset = 0;

    for( size_t i = 0; i < input.text_.length(); ++i )
    {
        UString::CharT ch = input.text_.at(i);

        if(UString::isThisChineseChar(ch))
        {
            func(data, input.text_.c_str()+i, 1, offset++, Term::ChinesePOS, Term::AND , 0, false);
        }
        else if(UString::isThisDigitChar(ch))
        {
            func(data, input.text_.c_str()+i, 1, offset++, Term::DigitPOS, Term::AND , 0, false);
        }
        else if(UString::isThisAlphaChar(ch))
        {
            if (isCaseSensitive)
            {
                func(data, input.text_.c_str()+i, 1, offset++, Term::EnglishPOS, Term::AND , 0, false);
            }
            else
            {
                tmpCh_ = UString::toLowerChar(ch);
                func(data, &tmpCh_, 1, offset++, Term::EnglishPOS, Term::AND , 0, false);
            }
        }
        else if(UString::isThisSpaceChar(ch))
        {
            continue;
        }
        else if(UString::isThisPunctuationChar(ch))
        {
            func(data, input.text_.c_str()+i, 1, offset++, Term::SpecialCharPOS, Term::AND , 0, true);
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
    return 0;
}

}
