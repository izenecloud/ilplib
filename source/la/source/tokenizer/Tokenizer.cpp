/**
 * Tokenizer.cpp
 *
 *  Created on: 2009-6-10
 *      Author: Huajie Zhang
 */

/**
 * @brief Rewrite Tokenizer using nextToken() methods
 * @author Wei
 */

#include <la/tokenizer/Tokenizer.h>
using namespace izenelib::util;
using namespace std;

namespace la
{

    const CharType ALLOW_CHR = 0;        /// < regular term
    const CharType DELIMITER_CHR = 1;    /// < delimiter
    const CharType SPACE_CHR = 2;        /// < space term
    const CharType UNITE_CHR = 3;        /// < united term

    CharTypeTable::CharTypeTable( bool useDefDelim )
    {
        memset(charTypeTable_, 0, TWO_BYPE_MAX);
        // if useDefDelim is set, all the characters are allows
        if( !useDefDelim )
        	return;
        // set the lower 4 bit to record default char type
        for(UCS2Char i = 0; i < TWO_BYPE_MAX; i++)
        {
            if(UString::isThisLanguageChar(i)
                    || UString::isThisDigitChar(i))
                continue;
            else if(UString::isThisSpaceChar(i))
                charTypeTable_[i] = SPACE_CHR;
            else
                charTypeTable_[i] = DELIMITER_CHR;
        }
    }

    void CharTypeTable::setConfig(const TokenizeConfig& conf)
    {
        // set the higher 4 bit to record user defined option type
        UString str;  // why need to copy?

        str = conf.divides_;
        if(!str.empty())
        {
            for(unsigned int j = 0; j < str.length() ; j ++)
            {
            	assert(str[j] < TWO_BYPE_MAX);
                charTypeTable_[str[j]] = DELIMITER_CHR;
            }
        }

        str = conf.unites_;
        if(!str.empty())
        {
            for(unsigned int j = 0; j < str.length() ; j ++)
            {
            	assert(str[j] < TWO_BYPE_MAX);
                charTypeTable_[str[j]] = UNITE_CHR;
            }
        }

        str = conf.allows_;
        if(!str.empty())
        {
            for(unsigned int j = 0; j < str.length() ; j ++)
            {
            	assert(str[j] < TWO_BYPE_MAX);
                charTypeTable_[str[j]] = ALLOW_CHR;
            }
        }

    }

    void Tokenizer::setConfig(const TokenizeConfig& conf)
    {
        table_.setConfig(conf);
    }

    void Tokenizer::tokenize(const UString & input)
    {
        input_ = (UString*)&input;
        input_cursor_ = 0;
    }

    bool Tokenizer::nextToken()
    {
        while( input_cursor_ < input_->length() && table_.getType(input_->at(input_cursor_)) == SPACE_CHR)  {
//            cout << input_cursor_ << "," << (char)input_->at(input_cursor_) << "," << (int)table_.getType(input_->at(input_cursor_)) << endl;
            input_cursor_ ++;
        }
        if(input_cursor_ == input_->length()) return false;

        output_buffer_cursor_ = 0;

        if(output_buffer_cursor_>=output_buffer_size_) {
            grow_output_buffer();
        }
        output_buffer_[output_buffer_cursor_++] = input_->at(input_cursor_);
        if(table_.getType(input_->at(input_cursor_)) == DELIMITER_CHR) {
           ++ input_cursor_;
           isDelimiter_ = true;
           return true;
        } else {
            ++ input_cursor_;
            isDelimiter_ = false;

            while(input_cursor_ < input_->length()) {
                CharType curType = table_.getType(input_->at(input_cursor_));
                if( curType==SPACE_CHR || curType==DELIMITER_CHR) {
                    return true;
                } else if(curType==ALLOW_CHR) {
                    if(output_buffer_cursor_>=output_buffer_size_) {
                        grow_output_buffer();
                    }
                    output_buffer_[output_buffer_cursor_++] = input_->at(input_cursor_++);
                } else {
                    ++ input_cursor_;
                }
            }
            return true;
        }
    }

    bool Tokenizer::grow_output_buffer()
    {
        UString::CharT* new_output_buffer = new UString::CharT[output_buffer_size_*2];
        memcpy(new_output_buffer, output_buffer_, output_buffer_size_*sizeof(UString::CharT));
        output_buffer_ = new_output_buffer;
        output_buffer_size_ *= 2;
        return true;
    }

    bool Tokenizer::tokenize(const UString & inputString, TermList & specialTerms, TermList& primTerms )
    {
        specialTerms.clear();
        primTerms.clear();

        size_t len = inputString.length();
        if(len == 0)
            return false;

        Term t;
        TermList::iterator it;

        unsigned int wordOff = 0, charOff = 0;

        UCS2Char curChar;
        CharType curType;
        //CharType curType, preType;


        for( charOff = 0 ; charOff < len ; )//charOff++ )   // charOff is always incremented inside
        {
            curType = table_.getType( inputString.at(charOff) );

            if( curType == ALLOW_CHR || curType == UNITE_CHR )
            {
                it = primTerms.insert( primTerms.end(), t );

                do
                {
                    curChar = inputString.at( charOff );
                    curType = table_.getType( curChar );

                    if( curType == ALLOW_CHR )
                    {
                        it->text_ += curChar;
                    }
                    else if( curType == SPACE_CHR || curType == DELIMITER_CHR )
                    {
                        break;
                    }

                    charOff++;
                }
                while( charOff < len );

                if( it->text_.length() == 0 )
                {
                    primTerms.erase( it );
                    continue;
                    //charOff--;
                }

                it->wordOffset_ = wordOff++;

                //charOff--;
            }
            else if( curType == DELIMITER_CHR )
            {

                it = specialTerms.insert( specialTerms.end(), t );

                do
                {
                    curChar = inputString.at(charOff);
                    curType = table_.getType( curChar );

                    if( curType == DELIMITER_CHR )
                        it->text_ += curChar;
                    else
                        break;
                    charOff++;
                }
                while( charOff < len );


                it->wordOffset_ = wordOff++;

                //charOff--;
            }
            else
                charOff++;
        }

        return true;
    }


    bool Tokenizer::tokenize(const UString & inputString, TermList & primTerms)
    {

        primTerms.clear();
        size_t len = inputString.length();
        if(len == 0)
            return false;

        Term t;
        TermList::iterator it;

        unsigned int wordOff = 0, charOff = 0;

        UCS2Char curChar;
        CharType curType;
        //CharType curType, preType;


        for( charOff = 0 ; charOff < len ; ) //charOff++ )
        {
            curType = table_.getType( inputString.at(charOff) );

            if( curType == ALLOW_CHR || curType == UNITE_CHR )
            {

                it = primTerms.insert( primTerms.end(), t );
                //it->begin_ = charOff;

                do
                {
                    curChar = inputString.at( charOff );
                    curType = table_.getType( curChar );

                    if( curType == ALLOW_CHR )
                    {
                        it->text_ += curChar;
                    }
                    else if( curType == SPACE_CHR || curType == DELIMITER_CHR )
                    {
                        break;
                    }

                    charOff++;
                }
                while( charOff < len );

                if( it->text_.length() == 0 )
                {
                    primTerms.erase( it );
                    continue;
                    //charOff--;
                }

                it->wordOffset_ = wordOff++;


                //charOff--;
            }
            else if( curType == DELIMITER_CHR )
            {
                if( ( (charOff +1) < len ) && table_.getType( inputString.at(charOff+1) ) != DELIMITER_CHR )
                {
                    wordOff++;
                }
                charOff++;
            }
            else
                charOff++;

        }

        return true;
    }

	bool Tokenizer::tokenizeWhite(const izenelib::util::UString& inputString, TermList& rawTerms)
    {
        rawTerms.clear();

        size_t len = inputString.length();
        if(len == 0)
            return false;

        Term t;
        TermList::iterator it;

        unsigned int wordOff = 0, charOff = 0;

        UCS2Char curChar;
        CharType curType;
        //CharType curType, preType;


        for( charOff = 0 ; charOff < len ; )//charOff++ )
        {
            curType = table_.getType( inputString.at(charOff) );

            if( curType == ALLOW_CHR || curType == UNITE_CHR )
            {
                it = rawTerms.insert( rawTerms.end(), t );
                //it->begin_ = charOff;

                do
                {
                    curChar = inputString.at( charOff );
                    curType = table_.getType( curChar );

                    if( curType == ALLOW_CHR )
                    {
                        it->text_ += curChar;
                    }
                    else if( curType == SPACE_CHR || curType == DELIMITER_CHR )
                    {
                        break;
                    }

                    charOff++;
                }
                while( charOff < len );

                if( it->text_.length() == 0 )
                {
                    rawTerms.erase( it );
                    continue;
                    //charOff--;
                }

                it->wordOffset_ = wordOff++;

                //charOff--;
            }
            else if( curType == DELIMITER_CHR )
            {

                it = rawTerms.insert( rawTerms.end(), t );

                do
                {
                    curChar = inputString.at(charOff);
                    curType = table_.getType( curChar );
                    if( curType == DELIMITER_CHR )
                        it->text_ += curChar;
                    else
                        break;
                    charOff++;
                }
                while( charOff < len );


                it->wordOffset_ = wordOff++;

                //charOff--;
            }
            else
            {   // SPACE_CHR  nothing to do
                charOff++;
            }

        }

        return true;
    }

}
