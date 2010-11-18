/*
 * NGramAnalyzer.cpp
 *
 *  Created on: 2009-6-15
 *      Author: zhjay
 */

#include <la/analyzer/NGramAnalyzer.h>

#include <math.h>

using namespace izenelib::util;
using namespace std;

namespace la
{


    const unsigned int NGramAnalyzer::NGRAM_APART_NON_         = 0x00;
    const unsigned int NGramAnalyzer::NGRAM_APART_ALL_         = 0x0F;
    const unsigned int NGramAnalyzer::NGRAM_APART_CJK_         = 0x01;
    const unsigned int NGramAnalyzer::NGRAM_APART_ALPHA_       = 0x02;
    const unsigned int NGramAnalyzer::NGRAM_APART_NUMERIC_     = 0x04;
    const unsigned int NGramAnalyzer::NGRAM_APART_OTHER_       = 0x08;


    int NGramAnalyzer::analyze_impl( const Term& input, void* data, HookType func )
    {
        int ngram = 0, max_gram = 0;
        unsigned int insertCnt = 0;
        unsigned char level = 0;

        int len = input.text_.length();
        if( len == 0 )
            return 0;

        // decide proper minGram and maxGram
        if( minGram_ <= len )
        {
            ngram = minGram_;
            max_gram = min(len, maxGram_);
        }
        else
        {
            ngram = max_gram = len;
        }

        for( ; ngram <= max_gram; ++ngram )
        {
            if( apartFlag_ == 0 )
            {
                for( int start = 0; start <= len - ngram ; ++start )
                {
                    func(data, input.text_.c_str() + start, ngram, input.wordOffset_, Term::OtherPOS, Term::AND, level, false);
                    if( maxNo_ > 0 && ++insertCnt >= maxNo_ )
                        return 0;
                }
            }
            else
            {
                int typeStart = 0;
                unsigned char curType = getCharTypeBit( input.text_[0] );

                for( int typeEnd = 1; ; ++typeEnd )
                {
                    if( typeEnd == len || curType != getCharTypeBit(input.text_[typeEnd]) )
                    {
                        if( (curType & apartFlag_) == 0 || ngram >= (typeEnd-typeStart) )
                        {
                            func(data, input.text_.c_str() + typeStart, typeEnd-typeStart, input.wordOffset_, Term::OtherPOS, Term::AND, level, false);
                            if( maxNo_ > 0 && ++insertCnt >= maxNo_ )
                                return 0;
                        }
                        else
                        {
                            for( int substart = typeStart; substart <= typeEnd - ngram; ++substart )
                            {
                                func(data, input.text_.c_str() + substart, ngram, input.wordOffset_, Term::OtherPOS, Term::AND, level, false);
                                if( maxNo_ > 0 && ++insertCnt == maxNo_ )
                                    return 0;
                            }
                        }

                        if( typeEnd == len )
                            break;
                        typeStart = typeEnd;
                        curType = getCharTypeBit( input.text_[typeStart] );
                    }
                }
            }
        } // end for( ; ngram <= max_gram; ++ngram )

        return 0;
    }



} // end: namespace la
