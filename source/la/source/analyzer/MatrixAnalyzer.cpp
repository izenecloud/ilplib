#include <la/analyzer/MatrixAnalyzer.h>

using namespace std;

namespace la
{

//    const unsigned char defLogicOp = Term::OR_BIT;

    MatrixAnalyzer::MatrixAnalyzer( bool prefix, bool suffix )
        : bPrefix_(prefix), bSuffix_(suffix)
    {}

    int MatrixAnalyzer::analyze_index( const TermList & input, TermList & output, unsigned char retFlag )
    {
        return analyze_search( input, output, retFlag );
    }

    int MatrixAnalyzer::analyze_search( const TermList & input, TermList & output, unsigned char retFlag )
    {
        TermList::const_iterator it;

        Term                newTerm;
        TermList::iterator  term_it;

        size_t length = 0;

        for( it = input.begin(); it != input.end(); it++ )
        {
            term_it = output.insert( output.end(), *it );

            length = it->text_.length();
            if( bPrefix_ )
            {
                for( unsigned int preLength = 1; preLength < length; preLength ++ )
                {
                    term_it = output.insert( output.end(), *it );

                    term_it->text_ = it->text_.substr(0, preLength);
                }
            }

            if( bSuffix_ )
            {
                for( unsigned int start = 1; start < length; start ++ )
                {
                    term_it = output.insert( output.end(), *it );

                    term_it->text_ = it->text_.substr(start, length-start);
                }
            }
        }
        return 0;
    }

}
