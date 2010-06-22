/*
 * Tokenizer.h
 *
 *  Created on: 2009-6-10
 *      Author: Huajie Zhang
 */

#ifndef _LA_TOKENIZER_H_
#define _LA_TOKENIZER_H_

#include <Term.h>
//#include <LACache.h>

#define TWO_BYPE_MAX 65535

namespace la
{
    typedef unsigned char CharType;
    extern const CharType ALLOW_CHR;        /// < regular term
    extern const CharType DELIMITER_CHR;    /// < delimiter
    extern const CharType SPACE_CHR;        /// < space term
    extern const CharType UNITE_CHR;        /// < united term

    ///
    /// \brief tokenizer option class
    /// This class record the user defined options of chars
    ///
    class TokenizeConfig
    {

        public:
            void addAllows(wiselib::UString astr){
                allows_ += astr;
            }
            void addDivides(wiselib::UString dstr){
                divides_ += dstr;
            }
            void addUnites(wiselib::UString ustr){
                unites_ += ustr;
            }
            void addAllows(std::string astr){
                wiselib::UString str(astr, wiselib::UString::UTF_8);
                allows_ += str;
            }
            void addDivides(std::string dstr){
                wiselib::UString str(dstr, wiselib::UString::UTF_8);
                divides_ += str;
            }
            void addUnites(std::string ustr){
                wiselib::UString str(ustr, wiselib::UString::UTF_8);
                unites_ += str;
            }
            wiselib::UString allows_;
            wiselib::UString divides_;
            wiselib::UString unites_;
    };

    //typedef vector<TokenizeOption> TokenOptList;

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

        CharType getType(wiselib::UCS2Char c){
            return charTypeTable_[c];
        }

        /// \brief check whether default allow
        /// \param c char
        /// \return true if so, else false
        bool isAllow(wiselib::UCS2Char c){
            return charTypeTable_[c] == ALLOW_CHR ;
        }

        /// \brief check whether default divide
        /// \param c char
        /// \return true if so, else false
        bool isDivide(wiselib::UCS2Char c){
            return charTypeTable_[c] == DELIMITER_CHR ;
        }

        /// \brief check whether defined allow
        /// \param c char
        /// \return true if so, else false
        bool isUnite(wiselib::UCS2Char c){
            return charTypeTable_[c] == UNITE_CHR ;
        }

        bool isEqualType(wiselib::UCS2Char c1, wiselib::UCS2Char c2){
            return  charTypeTable_[c1] == charTypeTable_[c2] ;
        }
    };


    ///
    /// \brief interface of Tokenizer
    /// This class tokenize the input string according to defined char type table
    ///
    class Tokenizer 
    {
        CharTypeTable table_; ///< char type table

        //	void setTermType(Term& term);
        /// \param useDefDelim whether use the default delimiter, default is true
        public:
        Tokenizer( bool useDefDelim = true )
			: table_( useDefDelim )
        {
        }



        ~Tokenizer()
        {

        }

        /// \brief set the user defined char types
        /// \param list char type option list
        void setConfig(const TokenizeConfig& conf);

        /// \brief tokenize the input text, output two term lists: raw term list and primary term list
        /// \param inputString input text string
        /// \param rawTerms the output raw term list
        /// \param primTerms the output primary term list
        bool tokenize(const wiselib::UString & inputString, TermList & specialTerms, TermList& primTerms );

        /// \brief tokenize the input text, remove the space chars, output raw term list
        /// \param inputString input text string
        /// \param rawTerms the output raw term list
        bool tokenizeWhite(const wiselib::UString& inputString, TermList& rawTerms);

        /// \brief tokenize the input text, output two term lists: raw term list and primary term list
        /// \param inputString input text string
        /// \param primTerms the output primary term list
        bool tokenize(const wiselib::UString & inputString, TermList & primTerms);

        //bool tokenize(const wiselib::UString& inputString, std::list<Term*> & primTerms);

    };

}

#endif /* _LA_TOKENIZER_H_ */
