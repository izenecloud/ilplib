/*
 * NGramAnalyzer.h
 *
 *  Created on: 2009-6-15
 *      Author: zhjay
 */

#ifndef _LA_NGRAM_ANALYZER_H_
#define _LA_NGRAM_ANALYZER_H_

#include <la/Analyzer.h>

namespace la 
{

    ///
    /// \brief interface of NGramAnalyzer
    /// This class extracts terms with char-based n-gram method
    ///
    class NGramAnalyzer : public Analyzer
    {
        private:
            int             minGram_;   ///< min gram
            int             maxGram_;   ///< max gram (mingram <= ngram <= maxgram)
            unsigned int    maxNo_;     ///< The maximum number of terms to generate.
            bool            bApart_;     ///< Creates NGram for korean, alphabet, and numerical character independently

            /// @brief  Defines what character types to apply ngram with.
            /// @details
            ///  0000 - no apart
            ///  0001 - Korean
            ///  0010 - non-Korean
            ///  0100 - Numeric characters
            ///  1000 - Other
            ///  1111 - All
            unsigned char   apartFlag_; 

        public:
            static const unsigned int NGRAM_APART_NON_;
            static const unsigned int NGRAM_APART_ALL_;
            static const unsigned int NGRAM_APART_CJK_;
            static const unsigned int NGRAM_APART_ALPHA_;
            static const unsigned int NGRAM_APART_NUMERIC_;
            static const unsigned int NGRAM_APART_OTHER_;


        public:

            NGramAnalyzer( int nmin, int nmax, unsigned int maxNo, unsigned char apartFlag = NGRAM_APART_NON_ ) 
                :apartFlag_(apartFlag)
            {
                setMinMaxGram( nmin, nmax );
                setMaxNo(maxNo);
            }

            ~NGramAnalyzer(){}

            virtual int analyze_index( const TermList & input, TermList & output, unsigned char retFlag );
            virtual int analyze_search( const TermList & input, TermList & output, unsigned char retFlag );


            void setMinMaxGram( int min, int max )
            {
                if( min < 1 || max < 1 )
                    throw std::logic_error( "NGramAnalyzer: mingram < 1 || maxgram < 1" );

                minGram_ = min;
                maxGram_ = max;
                if( minGram_ > maxGram_ )
                    throw std::logic_error( "NGramAnalyzer mingram > maxgram" );
            }

            void setMaxNo( unsigned int no )
            {
                if( no < 1 )
                    throw std::logic_error( "NGramAnalyzer: maxno < 1" );

                maxNo_ = no;
            }

            void setApartFlag( unsigned char flag )
            {
                apartFlag_ = flag;
            }
            

        private:
            unsigned char getCharTypeBit( const izenelib::util::UCS2Char & c )
            {
                unsigned char bitmap = 0x00;

                if( isCJKType(c) )
                {
                    bitmap |= NGRAM_APART_CJK_;
                }
                else if( isAlphabeticType(c) )
                {
                    bitmap |= NGRAM_APART_ALPHA_;
                }
                else if( isNumericType(c) )
                {
                    bitmap |= NGRAM_APART_NUMERIC_;
                }
                else
                {
                    bitmap |= NGRAM_APART_OTHER_;
                }

                return bitmap;
            }

            bool isCJKType( const izenelib::util::UCS2Char & c ) const
            {
                return ( izenelib::util::UString::isThisKoreanChar(c) ||
                        izenelib::util::UString::isThisChineseChar(c) ||
                        izenelib::util::UString::isThisJapaneseChar(c) );
            }

            bool isAlphabeticType( const izenelib::util::UCS2Char & c ) const
            {
                return ( izenelib::util::UString::isThisAlphaChar(c) );
            }

            bool isNumericType( const izenelib::util::UCS2Char & c ) const
            {
                return izenelib::util::UString::isThisDigitChar(c);
            }

            bool isOtherType( const izenelib::util::UCS2Char & c ) const
            {
                return ( isCJKType(c) == false && isAlphabeticType(c) == false && isNumericType(c) == false );
            }

    };

    //void getTermNGram( const TermList & input, TermList & output, int nmin = 2, int nmax = 2 );

}


#endif /* _LA_NGRAM_ANALYZER_H_ */
