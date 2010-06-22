/*
 * StemAnalyzer.cpp
 *
 *  Created on: 2009-6-11
 *      Author: zhjay
 */

#include <StemAnalyzer.h>

using namespace wiselib;
using namespace la::stem;

namespace la
{
    StemAnalyzer::StemAnalyzer( const string & lang )
    {
        if( lang == "danish" )
            language_ = STEM_LANG_DANISH;
        else if( lang == "dutch" )
            language_ = STEM_LANG_DUTCH;
        else if( lang == "english" )
            language_ = STEM_LANG_ENGLISH;
        else if( lang == "finnish" )
            language_ = STEM_LANG_FINNISH;
        else if( lang == "french" )
            language_ = STEM_LANG_FRENCH;
        else if( lang == "german" )
            language_ = STEM_LANG_GERMAN;
        else if( lang == "hungarian" )
            language_ = STEM_LANG_HUNGARIAN;
        else if( lang == "italian" )
            language_ = STEM_LANG_ITALIAN;
        else if( lang == "norwegian" )
            language_ = STEM_LANG_NORWEGIAN;
        else if( lang == "portuguese" )
            language_ = STEM_LANG_PORTUGUESE;
        else if( lang == "romanian" )
            language_ = STEM_LANG_ROMANIAN;
        else if( lang == "russian" )
            language_ = STEM_LANG_RUSSIAN;
        else if( lang == "spanish" )
            language_ = STEM_LANG_SPANISH;
        else if( lang == "swedish" )
            language_ = STEM_LANG_SWEDISH;
        else if( lang == "turkish" )
            language_ = STEM_LANG_TURKISH;
        else
            language_ = STEM_LANG_UNKNOWN;

        if(!stemmer_.init(language_))
        {
            throw std::logic_error( "Incorrect language type given for Stemmer." );
        }
    }

    StemAnalyzer::StemAnalyzer( const stem::Language lang )
        : language_(lang)
    {
        if(!stemmer_.init(language_))
        {
            throw std::logic_error( "Incorrect language type given for Stemmer." );
        }
    }

    StemAnalyzer::~StemAnalyzer()
    {
        stemmer_.deinit();
    }

    int StemAnalyzer::analyze_index( const TermList & input, TermList & output, unsigned char retFlag )
    {
        string inputstr, stem;
        TermList::const_iterator it;

        Term newTerm;
        TermList::iterator term_it;

        for( it = input.begin(); it != input.end(); it++ )
        {
            if( retFlag_idx_ & ANALYZE_PRIME_ )
            {
                term_it = output.insert( output.end(), *it );
            }
            if( (retFlag_idx_ & ANALYZE_SECOND_) == 0 )
                continue;


            it->text_.convertString( inputstr, UString::CP949 );
            stemmer_.stem( inputstr, stem );

            if( !(retFlag_idx_ & ANALYZE_PRIME_) || inputstr != stem )
            {
                term_it = output.insert( output.end(), *it );
                term_it->text_.assign( stem, UString::CP949 );
            }
        }
        return 0;
    }

    int StemAnalyzer::analyze_search( const TermList & input, TermList & output, unsigned char retFlag )
    {
        string inputstr, stem;
        TermList::const_iterator it;

        unsigned char       level = 0;
        Term                newTerm;
        TermList::iterator  term_it;

        for( it = input.begin(); it != input.end(); it++ )
        {
            if( retFlag_sch_ & ANALYZE_PRIME_ )
            {
                term_it = output.insert( output.end(), *it );
                term_it->stats_ = makeStatBit( Term::OR_BIT, level++ );
            }
            if( (retFlag_sch_ & ANALYZE_SECOND_) == 0 )
                continue;


            it->text_.convertString( inputstr, UString::CP949 );
            stemmer_.stem( inputstr, stem );

            if( !(retFlag_sch_ & ANALYZE_PRIME_) || inputstr != stem )
            {
                term_it = output.insert( output.end(), newTerm );

                term_it->text_.assign( stem, UString::CP949 );
                term_it->stats_ = makeStatBit( Term::AND_BIT, level );
            }
        }
        return 0;
    }



}


