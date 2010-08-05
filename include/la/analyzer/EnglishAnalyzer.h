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

#include <la/analyzer/Analyzer.h>
#include <la/pos/POSTaggerEnglish.h>

namespace la
{
    class EnglishAnalyzer : public Analyzer
    {
        private:
            POSTaggerEnglish* tagger_;

        public:
            EnglishAnalyzer( std::string modelPath )
            {
                tagger_ = &POSTaggerEnglish::getInstance(new std::string(modelPath));
            }
            EnglishAnalyzer()
            {
                tagger_ = &POSTaggerEnglish::getInstance();
            }
            ~EnglishAnalyzer()
            {}


            virtual int analyze_index( const TermList & input, TermList & output, unsigned char retFlag );
            virtual int analyze_search( const TermList & input, TermList & output, unsigned char retFlag );

    };
}


#endif  // _LA_ENGLISH_ANALYZER_H_
