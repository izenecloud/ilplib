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
        termList.sort(wordOffset_sorter);
    }

}
