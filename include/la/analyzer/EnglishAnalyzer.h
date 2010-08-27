/**
 * @brief   Header file of EnligshAnalyzer class
 * @file    EnglishAnalyzer.h
 * @author  zhjay, MyungHyun Lee (Kent)
 * @date    Aug 25, 09 (originally, July 8, 09)
 * @details
 *  Separated the EnglishAnalyzer from the KoreanAanlyzer. The two were in the same file.
 */

#ifndef _LA_ENGLISH_ANALYZER_H_
#define  _LA_ENGLISH_ANALYZER_H_

#include <la/analyzer/CommonLanguageAnalyzer.h>

namespace la
{
class EnglishAnalyzer : public CommonLanguageAnalyzer
{
public:

    EnglishAnalyzer() : CommonLanguageAnalyzer()
    {
    }

    ~EnglishAnalyzer()
    {
    }

protected:

    virtual inline void parse(const UString & input)
    {
        tokenizer_.tokenize(input);
        localOffset_ = 0;
        resetToken();
    }

    virtual inline bool nextToken()
    {
        if (tokenizer_.nextToken() ) {
            token_ = tokenizer_.getToken();
            len_ = tokenizer_.getLength();
            offset_ = localOffset_;
            localOffset_ ++;
            isIndex_ = true;
            pos_ = Term::EnglishPOS;
            return true;
        } else {
            resetToken();
            return false;
        }
    }

    inline bool isAlpha()
    {
        return true;
    }

    inline bool isSpecialChar()
    {
        return tokenizer_.isDelimiter();
    }

private:

    int localOffset_;

};
}


#endif  // _LA_ENGLISH_ANALYZER_H_
