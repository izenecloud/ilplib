/**
 * @brief   Defines Analyzerclass
 * @file    Analyzer.cpp
 * @author  Vernkin
 * @date    2010.02.24
 */

#include <la/Analyzer.h>

namespace la
{
    const unsigned char Analyzer::ANALYZE_NONE_ = 0x00;
    const unsigned char Analyzer::ANALYZE_PRIME_    = 0x01;
    const unsigned char Analyzer::ANALYZE_SECOND_   = 0x02;
    const unsigned char Analyzer::ANALYZE_ALL_  = 0x03;

    const UString SPACE_USTR(" ", UString::UTF_8 );

    int Analyzer::analyze_index( const TermList & input, TermList & output )
    {
        return analyze_index( input, output, retFlag_idx_ );
    }

    int Analyzer::analyze_search( const TermList & input, TermList & output )
    {
        return analyze_search( input, output, retFlag_sch_ );
    }

    void removeRedundantSpaces( izenelib::util::UString& input, izenelib::util::UString& output )
    {
        using namespace izenelib::util;

        bool lastIsASCII = false;
        bool coverSpace = false; // whether traverse space last time

        size_t len = input.length();
        output.clear();

        unsigned int asciiUpBound = (unsigned int)0xFF;

        for( size_t i = 0; i < len; ++i )
        {
            // set the current mode
            UCS2Char curChar = input[ i ];
            if( UString::isThisSpaceChar( curChar ) )
            {
                coverSpace = true;
                continue;
            }

            bool curIsASCII = ( curChar < asciiUpBound );

            if( coverSpace == true )
            {
                if( curIsASCII == true && lastIsASCII == true )
                {
                    output += SPACE_USTR;
                }
                coverSpace = false;
            }

            output += curChar;
            lastIsASCII = curIsASCII;
        }
    }
}
