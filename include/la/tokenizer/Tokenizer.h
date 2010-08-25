/**
 * Tokenizer.h
 *
 *  Created on: 2009-6-10
 *      Author: Huajie Zhang
 */

/**
 * @brief Rewrite Tokenizer using nextToken() methods
 * @author Wei
 */

#ifndef _LA_TOKENIZER_H_
#define _LA_TOKENIZER_H_

#include <la/common/Term.h>

#define TWO_BYPE_MAX 65535

namespace la
{
///
/// \brief tokenizer option class
/// This class record the user defined options of chars
///
class TokenizeConfig
{

public:
    void addAllows(izenelib::util::UString astr)
    {
        allows_ += astr;
    }
    void addDivides(izenelib::util::UString dstr)
    {
        divides_ += dstr;
    }
    void addUnites(izenelib::util::UString ustr)
    {
        unites_ += ustr;
    }
    void addAllows(std::string astr)
    {
        izenelib::util::UString str(astr, izenelib::util::UString::UTF_8);
        allows_ += str;
    }
    void addDivides(std::string dstr)
    {
        izenelib::util::UString str(dstr, izenelib::util::UString::UTF_8);
        divides_ += str;
    }
    void addUnites(std::string ustr)
    {
        izenelib::util::UString str(ustr, izenelib::util::UString::UTF_8);
        unites_ += str;
    }
    izenelib::util::UString allows_;
    izenelib::util::UString divides_;
    izenelib::util::UString unites_;
};

typedef unsigned char CharType;

extern const CharType ALLOW_CHR;        /// < regular term
extern const CharType DELIMITER_CHR;    /// < delimiter
extern const CharType SPACE_CHR;        /// < space term
extern const CharType UNITE_CHR;        /// < united term

///
/// \brief class of char type table
/// This class record the char type table for tokenizer to look up
///
class CharTypeTable
{
    CharType charTypeTable_[TWO_BYPE_MAX];
public:

    /// \param useDefDelim whether use the default delimiter, default is true
    CharTypeTable( bool useDefDelim = true );

    /// \brief change the char type table according to user's defined option list
    /// \param list user defined option list
    void setConfig(const TokenizeConfig& conf);

    CharType getType(izenelib::util::UCS2Char c)
    {
        return charTypeTable_[c];
    }

    /// \brief check whether default allow
    /// \param c char
    /// \return true if so, else false
    bool isAllow(izenelib::util::UCS2Char c)
    {
        return charTypeTable_[c] == ALLOW_CHR ;
    }

    /// \brief check whether default divide
    /// \param c char
    /// \return true if so, else false
    bool isDivide(izenelib::util::UCS2Char c)
    {
        return charTypeTable_[c] == DELIMITER_CHR ;
    }

    /// \brief check whether defined allow
    /// \param c char
    /// \return true if so, else false
    bool isUnite(izenelib::util::UCS2Char c)
    {
        return charTypeTable_[c] == UNITE_CHR ;
    }

    bool isEqualType(izenelib::util::UCS2Char c1, izenelib::util::UCS2Char c2)
    {
        return  charTypeTable_[c1] == charTypeTable_[c2] ;
    }
};


///
/// \brief interface of Tokenizer
/// This class tokenize the input string according to defined char type table
///
class Tokenizer
{
public:
    /// \param useDefDelim whether use the default delimiter, default is true
    Tokenizer( bool useDefDelim = true )
        : table_(useDefDelim),
          output_buffer_size_(4096),
          output_buffer_(NULL)
    {
        output_buffer_ = new izenelib::util::UString::CharT[output_buffer_size_];
    }

    ~Tokenizer()
    {
        delete output_buffer_;
    }

    /// \brief set the user defined char types
    /// \param list char type option list
    void setConfig(const TokenizeConfig& conf);

    /// \brief tokenize the input text, call nextToken(), getToken(), getLength() to get the result.
    /// \param input input text string
    void tokenize(const izenelib::util::UString & input);

    bool nextToken();

    inline const izenelib::util::UString::CharT * getToken() {
        return output_buffer_;
    }

    inline const size_t getLength() {
        return output_buffer_cursor_;
    }

    inline const bool isDelimiter() {
        return isDelimiter_;
    }

    /// TODO: rewrite following methods

    /// \brief tokenize the input text, output two term lists: raw term list and primary term list
    /// \param inputString input text string
    /// \param rawTerms the output raw term list
    /// \param primTerms the output primary term list
    bool tokenize(const izenelib::util::UString & inputString, TermList & specialTerms, TermList& primTerms );

    /// \brief tokenize the input text, remove the space chars, output raw term list
    /// \param inputString input text string
    /// \param rawTerms the output raw term list
    bool tokenizeWhite(const izenelib::util::UString& inputString, TermList& rawTerms);

    /// \brief tokenize the input text, output two term lists: raw term list and primary term list
    /// \param inputString input text string
    /// \param primTerms the output primary term list
    bool tokenize(const izenelib::util::UString & inputString, TermList & primTerms);

private:

    CharTypeTable table_;

    izenelib::util::UString * input_;

    size_t input_cursor_;

    size_t output_buffer_size_;

    izenelib::util::UString::CharT * output_buffer_;

    size_t output_buffer_cursor_;

    bool isDelimiter_;

};

}

#endif /* _LA_TOKENIZER_H_ */
