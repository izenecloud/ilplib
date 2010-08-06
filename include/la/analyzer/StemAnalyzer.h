/**
 * @file    KoreanAnalyzer.cpp
 * @author  MyungHyun Lee (Kent)
 * @date    June 11, 09
 */

#ifndef _LA_STEMANALYZER_H_
#define _LA_STEMANALYZER_H_

#include <la/analyzer/Analyzer.h>
#include <la/stem/Stemmer.h>


namespace la
{
    ///
    /// \brief interface of StemAnalyzer
    /// This class extracts terms with stemming method
    ///
    class StemAnalyzer: public Analyzer
    {
        protected:
            stem::Stemmer stemmer_; ///< a stemmer provided by "libstemmer"
            //izenelib::util::UString::EncodingType encodingType_; ///< encoding type
            stem::Language language_; ///< language type

        public:

            StemAnalyzer( const std::string & lang );
            StemAnalyzer( const stem::Language lang );

            ~StemAnalyzer();

            virtual int analyze_index( const TermList & input, TermList & output, unsigned char retFlag );
            virtual int analyze_search( const TermList & input, TermList & output, unsigned char retFlag );


    };

}


#endif /* _LA_STEMANALYZER_H_ */
