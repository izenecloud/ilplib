/*
 * LA.cpp
 *
 *  Created on: 2009-6-16
 *      Author: zhjay
 */

#include <la/LA.h>
#include <la/util/EnglishUtil.h>

#include <util/ustring/UString.h>

#include <map>

using namespace la::stem;
using namespace izenelib::util;
using namespace izenelib::ir::idmanager;
using namespace std;

namespace la
{

    const string RES_PUNCT = "RP";
    const Term newTerm;

    const UString OP_USTR( " |!(){}[]^\"", UString::UTF_8 );
    const UString BACK_SLASH( "\\", UString::UTF_8 );

    map< UString, UString > OP_REP_MAP;

    void replaceSpecialChar( const UString& in, UString& ret )
    {
        map< UString, UString >::iterator itr;
        for( size_t i = 0; i < in.length(); ++i )
        {
            UString key = in.substr( i, 1 );
            itr = OP_REP_MAP.find( key );
            if( itr == OP_REP_MAP.end() )
            {
                ret += key;
            }
            else
            {
                ret += itr->second;
            }
        }
    }

    LA::LA() :
        TERM_LENGTH_THRESHOLD_(128),
        bCaseSensitive_( false )
    {
        if( OP_REP_MAP.empty() == true )
        {
            for( size_t i = 0; i < OP_USTR.length(); ++i )
            {
                UString key = OP_USTR.substr( i, 1 );
                OP_REP_MAP[ key ] = BACK_SLASH;
                OP_REP_MAP[ key ] += key;
            }
        }
    }

    void LA::process( IDManager* idm,
            const izenelib::util::UString & inputString,
            TermIdList & outList )
    {
        outList.clear();

//        TermList tokenList;
//        tokenizer_.tokenize( inputString, tokenList );

        if( analyzer_.get() != NULL ) {
//            for(TermList::iterator it = tokenList.begin(); it !=tokenList.end(); it++ ) {
                analyzer_->analyze( idm, inputString, outList );
//            }
        }
        else
        {
            /// TODO
            /// tokenizer_.tokenize( uinputstr, outList );
        }
    }

    void LA::process_index( const izenelib::util::UString & inputString, TermList & outList )
    {
        outList.clear();

        izenelib::util::UString uinputstr = inputString;
        if( !bCaseSensitive_ )
            uinputstr.toLowerString();

        if( analyzer_.get() != NULL )
        {
            TermList tokenList;
            tokenizer_.tokenize( uinputstr, tokenList );

            analyzer_->analyze_index( tokenList, outList );
        }
        else
        {
            tokenizer_.tokenize( uinputstr, outList );
        }

        lengthFilter( outList );
    }

    void LA::process_search( const izenelib::util::UString & inputString, TermList & outList )
    {
        outList.clear();

        izenelib::util::UString uinputstr = inputString;
        if( !bCaseSensitive_ )
            uinputstr.toLowerString();


        if( analyzer_.get() != NULL )
        {
            TermList tokenList;
            tokenizer_.tokenize( uinputstr, tokenList );

            analyzer_->analyze_search( tokenList, outList );
        }
        else
        {
            tokenizer_.tokenize( uinputstr, outList );
        }
        lengthFilter( outList );
    }

    void LA::process_index(
            const izenelib::util::UString& inputString,
            TermList & specialTermList,
            TermList & primaryTermList,
            TermList & outList
            )
    {
        specialTermList.clear();
        primaryTermList.clear();
        outList.clear();

        if( bCaseSensitive_ )
            tokenizer_.tokenize(inputString, specialTermList, primaryTermList );
        else
        {
            izenelib::util::UString uinputstr = inputString;
            uinputstr.toLowerString();
            tokenizer_.tokenize(uinputstr, specialTermList, primaryTermList );
        }

        lengthFilter( specialTermList );

        if( analyzer_.get() != NULL )
        {
            analyzer_->analyze_index( primaryTermList, outList );
            lengthFilter( primaryTermList );
            lengthFilter( outList );
        }
        else
        {
            lengthFilter( primaryTermList );
//            outList.splice( outList.end(), primaryTermList );
        }

    }

    void LA::process_search(
            const izenelib::util::UString& inputString,
            TermList & specialTermList,
            TermList & primaryTermList,
            TermList & outList
            )
    {
        specialTermList.clear();
        primaryTermList.clear();
        outList.clear();

        if( bCaseSensitive_ )
            tokenizer_.tokenize(inputString, specialTermList, primaryTermList );
        else
        {
            izenelib::util::UString uinputstr = inputString;
            uinputstr.toLowerString();
            tokenizer_.tokenize(uinputstr, specialTermList, primaryTermList );
        }


        lengthFilter( specialTermList );

        if( analyzer_.get() != NULL )
        {
            analyzer_->analyze_search( primaryTermList, outList );
            lengthFilter( primaryTermList );
            lengthFilter( outList );
        }
        else
        {
            lengthFilter( primaryTermList );
//            outList.splice( outList.end(), primaryTermList );
        }
    }

    izenelib::util::UString toExpandedString( const TermList & termList )
    {
        if( termList.empty() )
        {
            UString ustr;
            return ustr;
        }

        izenelib::util::UCS2Char SPACE = 32;
        izenelib::util::UCS2Char LBRACKET = 40;
        izenelib::util::UCS2Char RBRACKET = 41;
        izenelib::util::UCS2Char OR_CHAR = 124;

        TermList::const_iterator it;
        unsigned int prevOffset = 0;
        unsigned char baseLevel = 0;
        unsigned char prevLevel = 0;
        unsigned char prevAndOr = 0;

        unsigned char andOrBit = 0, level = 0;
        izenelib::util::UString output;

        for( it = termList.begin(); it != termList.end(); it++ )
        {
            readStatBit( it->stats_, andOrBit, level );

            if( it == termList.begin() )
            {
                output += LBRACKET;
                baseLevel = level;
            }
            else
            {

                if( prevOffset < it->wordOffset_ )
                {
                    for( int i = prevLevel; i >= baseLevel; i-- )
                        output += RBRACKET;
                    //if( prevLevel > level )
                    //output += RBRACKET;
                    //output += RBRACKET;
                    output += SPACE;
                    output += LBRACKET;
                    baseLevel = level;
                }
                else
                {
                    if( prevLevel < level )
                    {
                        if( prevAndOr == Term::AND_BIT )
                            output += SPACE;
                        else if( prevAndOr == Term::OR_BIT )
                            output += OR_CHAR;
                        output += LBRACKET;
                    }
                    else
                    {
                        if( prevLevel > level )
                            output += RBRACKET;
                        if( andOrBit == Term::AND_BIT )
                        {
                            output += SPACE;
                        }
                        else if( andOrBit == Term::OR_BIT )
                        {
                            output += OR_CHAR;
                        }
                    }
                }
            }

            UString replacedText;
            replaceSpecialChar( it->text_, replacedText );
            output += replacedText;

            prevLevel = level;
            prevOffset = it->wordOffset_;
            prevAndOr = andOrBit;

            //output.displayStringValue( izenelib::util::UString::UTF_8 ); cout << endl;
        }
        for( int i = prevLevel; i >= baseLevel; i-- )
            output += RBRACKET;

        return output;
    }

    void LA::lengthFilter( TermList & termList )
    {
        TermList::iterator it = termList.begin();
        while( it != termList.end() )
        {
            if( it->text_.length() > TERM_LENGTH_THRESHOLD_ )
            {
                it->text_.resize(TERM_LENGTH_THRESHOLD_);
            }
            it++;
        }
    }

    void LA::removeStopwords( TermList & termList,
            shared_ptr<PlainDictionary>&  stopDict )
    {
        ScopedReadLock<ReadWriteLock> srl( stopDict->getLock() );
        TermList::iterator itr = termList.begin();
        while( itr != termList.end() )
        {
            //cout<<"Contains ";itr->text_.displayStringInfo( UString::UTF_8);
            //cout<<" : "<<stopDict->containNoLock( itr->text_ )<<", size: "<<stopDict->size()<<endl;
            if( stopDict->containNoLock( itr->text_ ) )
                itr = termList.erase( itr );
            else
                ++itr;
        }
    }

}
