/*
 * LA.cpp
 *
 *  Created on: 2009-6-16
 *      Author: zhjay
 */

#include <la/LA.h>
#include <la/EnglishUtil.h>

#include <map>

using namespace la::stem;
using namespace izenelib::util;
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
            outList.splice( outList.end(), primaryTermList );
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
            outList.splice( outList.end(), primaryTermList );
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

    void LA::process_MIA( const izenelib::util::UString& inputString, TermList & outList,
            shared_ptr<PunctsType>& puncts, bool isIndex)
    {
        // if the special punctuations are empty
        if( puncts->empty() )
        {
            if( isIndex )
            {
                process_index( inputString, outList );
                return;
            }
            else
            {
                process_search( inputString, outList );
                return;
            }
        }

        // carry out normal analyze_XXXX
        TermList primaryTermList;
        outList.clear();

        if( bCaseSensitive_ )
            tokenizer_.tokenize(inputString, primaryTermList, primaryTermList );
        else
        {
            izenelib::util::UString uinputstr = inputString;
            uinputstr.toLowerString();
            tokenizer_.tokenize(uinputstr, primaryTermList, primaryTermList );
        }

        if( analyzer_.get() != NULL )
        {
            if( isIndex )
                analyzer_->analyze_index( primaryTermList, outList);
            else
                analyzer_->analyze_search( primaryTermList, outList);
        }

        lengthFilter( outList );

        // ignore the unused punctuations
        TermList::iterator itr = outList.begin();
        // temporal iterator
        TermList::iterator itr2;
        unsigned int offset = 0;
        while( itr != outList.end() )
        {
            if ( isAllPunctuations( itr->text_ ) )
            {
                UString& ustr = itr->text_;
                int len = static_cast<int>(ustr.length());
                if( len == 1 )
                {
                    if( puncts->find( ustr[0] ) == puncts->end() )
                    {
                        itr = outList.erase( itr );
                        continue;
                    }
                    else
                        itr->pos_ = RES_PUNCT;
                }
                else if( len > 1 )
                {
                    // if beginIdx < 0, all the punctuations should be removed
                    int beginIdx = -1;
                    itr->wordOffset_ += offset;
                    itr2 = itr;
                    ++itr2;
                    int newAdded = 0;
                    for( int i = 0; i < len; ++i )
                    {
                        if( puncts->find( ustr[i] ) == puncts->end() )
                            continue;
                        if( beginIdx < 0 )
                        {
                            beginIdx = i;
                            continue;
                        }

                        itr2 = outList.insert( itr2, newTerm );
                        itr2->text_ = ustr.substr( i, 1 );
                        itr2->pos_ = RES_PUNCT;
                        itr2->wordOffset_ = itr->wordOffset_ + i;
                        ++newAdded;
                    }

                    if( beginIdx < 0 )
                    {
                        itr = outList.erase( itr );
                        offset += len - 1;
                        continue;
                    }
                    else
                    {
                        itr->text_ = ustr.substr( beginIdx, 1 );
                        itr->pos_ = RES_PUNCT;
                        itr->wordOffset_ += beginIdx;
                        offset += len - 1;
                        itr = itr2;
                        if( newAdded > 0 )
                            ++itr;
                        continue;
                    }
                } // end else if
            }

            itr->wordOffset_ += offset;
            ++itr;
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
