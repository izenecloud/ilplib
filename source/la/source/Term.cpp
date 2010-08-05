/*
 * Term.cpp
 *
 *  Created on: 2009-6-17
 *      Author: zhjay
 */
#include <la/Term.h>

namespace la
{
    const unsigned char Term::OR_BIT = 1;
    const unsigned char Term::AND_BIT = 0;

    //-------------------------------- CLASS MEMBER METHODS -----------------------------


    std::string Term::toString() const
    {
        std::string tmp;
        text_.convertString(tmp, izenelib::util::UString::UTF_8);
        std::stringstream ss;
        ss << "pos=" 		<< pos_ 		            << "\t";
        ss << "morpheme=" 	<< bitset<32>(morpheme_) 	<< "\t";
        ss << "woffset=" 	<< wordOffset_			    << "\t";
        ss << "stats=" 	    << bitset<8>(stats_)  		<< "\t";
        ss << "\ttext=[" 	<< tmp			            << "]";
        return ss.str();
    }

    std::ostream & operator<<( std::ostream & out, const Term & term )
    {
        std::string tmp;
        term.text_.convertString(tmp, izenelib::util::UString::UTF_8);
        out << "pos=" 		<< term.pos_ 		            << "\t";
        out << "morpheme=" 	<< bitset<32>(term.morpheme_) 	<< "\t";
        out << "woffset=" 	<< term.wordOffset_			    << "\t";
        out << "stats=" 	<< bitset<8>(term.stats_)  		<< "\t";
        out << "\ttext=[" 	<< tmp			                << "]";
        return out;
    }

    //-------------------------------- NAMESPACE METHODS -----------------------------

    std::ostream & operator<<( std::ostream & out,  TermList& termList )
    {
        TermList::iterator it = termList.begin();
        for(; it != termList.end() ; it ++)
        {
            out<< *it << endl;
        }
        return out;
    }

    void sortByWordOffset( TermList & termList )
    {
//        termList.sort(wordOffset_sorter);
    }

    void appendPlaceHolder( TermList & termList, TermList & placeHolder )
    {
        if( termList.empty() == true )
        {
//            termList.splice( termList.end(), placeHolder );
            return;
        }

        TermList::iterator termItr = termList.begin();
        TermList::iterator phItr = placeHolder.begin();

        unsigned int preWS = 0; // previous word offset
        unsigned int curWS; // current word offset
        while( termItr != termList.end() && phItr != placeHolder.end() )
        {
            curWS = termItr->wordOffset_;
            if( ( preWS > 0 ) ? ( preWS < curWS ) : ( curWS > 0 ) )
            {
                while( preWS < curWS && phItr != placeHolder.end() )
                {
                    phItr->wordOffset_ = preWS;
                    termList.insert( termItr, *phItr );
                    ++phItr;
                    ++preWS;
                }

                if( phItr == placeHolder.end() )
                    break;
            }

            preWS = curWS + 1;
            ++termItr;
        }

        if( phItr != placeHolder.end() )
        {
            TermList::iterator phItr2 = phItr;
            unsigned int bgOff = termList.rbegin()->wordOffset_ + 1;
            while( phItr2 != placeHolder.end() )
            {
                phItr2->wordOffset_ = bgOff;
                ++bgOff;
                ++phItr2;
            }

//            termList.splice( termList.end(), placeHolder, phItr, placeHolder.end() );

        }
    }

    std::ostream & operator<<( std::ostream & out, const TermId & termid )
    {
        out << "woffset=" 	<< termid.wordOffset_			    << "\t";
        out << "\ttermid =[" 	<< termid.termid_	                << "]";
        return out;
    }

    UStringHashFunctor::~UStringHashFunctor(){}

}
