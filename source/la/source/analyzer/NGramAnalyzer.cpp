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

    int NGramAnalyzer::analyze_index( const TermList & input, TermList & output, unsigned char retFlag )
    {
        // the two work in the same way
        return analyze_search( input, output, retFlag );
    }

    int NGramAnalyzer::analyze_search( const TermList & input, TermList & output, unsigned char retFlag )
    {
        int len = 0, ngram = 0, max_gram = 0;
        unsigned int insertCnt = 0;

        unsigned char       level = 0;
        Term                newTerm;
        TermList::iterator  term_it;
        TermList            tempList;

        unsigned char   curType = 0x00;
        int typeStart = 0, typeEnd = 0, substart = 0;

        TermList::const_iterator it;
        for( it = input.begin(); it != input.end(); it++ )
        {
            len = it->text_.length();


            // add original term according to option
            if( retFlag & ANALYZE_PRIME_ )
            {
                term_it = tempList.insert( tempList.end(), *it );
                term_it->stats_ = makeStatBit( Term::OR_BIT, level );
            }
            level++;

            if( retFlag & ANALYZE_SECOND_ )
            {
                if( minGram_ <= len )
                {
                    ngram = minGram_;
                    max_gram = min(len, maxGram_);
                }
                else if( retFlag & ANALYZE_PRIME_ )
                {
                    //no need to do anything
                    ngram = 1;
                    max_gram = 0;
                }
                else
                {
                    ngram = max_gram = len;
                }

                for(; ngram <= max_gram; ngram++)
                {
                    if( apartFlag_ == 0 )
                    {
                        for(int start = 0; start <= len - ngram ; start++)
                        {
                            term_it = tempList.insert( tempList.end(), newTerm );
                            term_it->stats_ = makeStatBit( Term::AND_BIT, level);
                            term_it->wordOffset_ = it->wordOffset_;

                            term_it->text_   = it->text_.substr(start, ngram);
                            if( ++insertCnt == maxNo_ )
                                return 0;
                        }
                    }
                    else
                    {
                        typeStart = 0;

                        curType = getCharTypeBit( it->text_[0] );
                        for(typeEnd = 1; typeEnd < len; typeEnd++)
                        {
                            if( curType != getCharTypeBit(it->text_[typeEnd]) )
                            {
                                if( (curType & apartFlag_) == 0 || ngram >= (typeEnd-typeStart) )
                                {
                                    term_it = tempList.insert( tempList.end(), newTerm );
                                    term_it->stats_ = makeStatBit( Term::AND_BIT, level);
                                    term_it->wordOffset_ = it->wordOffset_;

                                    term_it->text_ = it->text_.substr(typeStart, typeEnd-typeStart);

                                    if( ++insertCnt == maxNo_ )
                                        return 0;
                                }
                                else
                                {
                                        for(substart = typeStart; substart <= typeEnd - ngram; substart++)
                                        {
                                            term_it = tempList.insert( tempList.end(), newTerm );
                                            term_it->stats_ = makeStatBit( Term::AND_BIT, level);
                                            term_it->wordOffset_ = it->wordOffset_;

                                            term_it->text_ = it->text_.substr(substart, ngram);

                                            if( ++insertCnt == maxNo_ )
                                                return 0;
                                        }
                                }

                                typeStart = typeEnd;
                                curType = getCharTypeBit( it->text_[typeStart] );
                            }
                        }

                        // it will duplicate the analyze_prime term
                        if( typeStart == 0 && (retFlag & ANALYZE_PRIME_) && ngram == len )
                            break;

                        if( (curType & apartFlag_) == 0 || ngram >= (typeEnd-typeStart) )
                        {
                            term_it = tempList.insert( tempList.end(), newTerm );
                            term_it->stats_ = makeStatBit( Term::AND_BIT, level);

                            term_it->wordOffset_ = it->wordOffset_;
                            term_it->text_ = it->text_.substr(typeStart, len-typeStart);

                            if( ++insertCnt == maxNo_ )
                                return 0;
                        }
                        else
                        {
                            for(substart = typeStart; substart <= len - ngram; substart++)
                            {
                                term_it = tempList.insert( tempList.end(), newTerm );
                                term_it->stats_ = makeStatBit( Term::AND_BIT, level);
                                term_it->wordOffset_ = it->wordOffset_;

                                term_it->text_ = it->text_.substr(substart, ngram);

                                if( ++insertCnt == maxNo_ )
                                    return 0;
                            }
                        }
                    }
                }
                /*
                if( (retFlag & ANALYZE_PRIME_) && maxGram_ == len )
                {
                    term_it = tempList.end();
                    term_it--;
                    tempList.splice( tempList.begin(), tempList, term_it );
                    tempList.begin()->stats_ = makeStatBit( Term::OR_BIT, level-1 );
                }
                */
            }
            else
            {
                if( (retFlag & ANALYZE_PRIME_ ) == 0 )
                {
                    term_it = tempList.insert( tempList.end(), *it );
                    term_it->stats_ = makeStatBit( Term::AND_BIT, level );
                }
            }

            level--;

            if( tempList.empty() == false )
            {
                output.splice( output.end(), tempList );
            }
            else if( tempList.size() == 1 )
            {
                tempList.begin()->stats_ = makeStatBit( Term::AND_BIT, level );
            }
        }
        return 0;
    }



} // end: namespace la
