/**
 * @brief   Defines MatrixAnalyzer class
 * @file    MatrixAnalyzer.h
 * @author  MyungHyun Lee (Kent)
 * @date    2009.10.28
 * @details
 */

#ifndef _LA_MATRIX_ANALYZER_H_
#define _LA_MATRIX_ANALYZER_H_


#include <la/Analyzer.h>

#include <list>

namespace la 
{

    class MatrixAnalyzer : public Analyzer
    {
        protected:
            bool bPrefix_;
            bool bSuffix_;


        public:
            MatrixAnalyzer( bool prefix, bool suffix ); 

            virtual int analyze_index( const TermList & input, TermList & output, unsigned char retFlag );
            virtual int analyze_search( const TermList & input, TermList & output, unsigned char retFlag );

    };

}

#endif /* _LA_MATRIX_ANALYZER_H_ */

