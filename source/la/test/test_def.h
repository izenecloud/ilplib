
#ifndef _LA_TEST_DEF_H_
#define _LA_TEST_DEF_H_

#include <Term.h>
#include <util/ustring/UString.h>
#include <stdlib.h>
#include <iostream>


izenelib::util::UString::EncodingType IO_ENCODING = izenelib::util::UString::UTF_8;
izenelib::util::UString::EncodingType FILE_ENCODING = izenelib::util::UString::UTF_8;

const unsigned int BUFFER = 1024;

std::ostream & printTerm( const la::Term & term, std::ostream & out=cout )
{
    std::string tmp;
    term.text_.convertString(tmp, IO_ENCODING );
    out << "len=" 		    << (term.text_.length()) 	    << "\t";
    out << "pos=" 		    << term.pos_ 		            << "\t";
    out << "morpheme=" 		<< bitset<32>(term.morpheme_) 	<< "\t";
    out << "woffset=" 	    << term.wordOffset_			    << "\t";
    out << "stats=" 	    << bitset<8>(term.stats_)  		<< "\t";
    out << "\ttext=[" 		<< tmp			                << "]";
    out << std::endl;
    return out;
}
std::ostream & printTermList( const la::TermList & termList, std::ostream & out=cout )
{
    la::TermList::const_iterator it;
    for( it = termList.begin(); it != termList.end(); it++ )
    {
        printTerm( *it, out );
    }
    return out;
}

izenelib::util::UString showMenu()
{
    std::cout << "Enter a text. (buffer size is: " << BUFFER << ")" << std::endl;

    char buf[BUFFER];
    std::cin.getline( buf, 1024 );

    izenelib::util::UString query( buf, IO_ENCODING );

    return query;
}

inline void downcase( std::string & str )
{
    for(size_t i = 0; i < str.length(); i++ )
    {
        str[i] = tolower( str[i] );
    }
}

int parseTruth( const std::string & str ) 
{
    std::string tmp;
    for( size_t i = 0; i < str.length(); i++ )
    {
        tmp.push_back( std::tolower(str[i]) );
    }

    if( tmp.compare("y") == 0 || tmp.compare("yes") ==  0 )
        return 1;
    else if( tmp.compare("n") == 0 || tmp.compare("no") ==  0 )
        return -1;
    else
        return 0;
}

string getKmaKnowledgePath()
{
    char * p = getenv("WISEKMA");
    if( p != NULL )
    {
        string str(p);
        str.append("/knowledge");
        return str;
    }
    else
    {
        throw logic_error( "cannot find \"knowledge\" under WISEKMA(environment variable)." );
    }
}

string getCmaKnowledgePath()
{
    char * p = getenv("IZENECMA");
    if( p != NULL )
    {
        string str(p);
        str.append("/db/icwb/utf8");
        return str;
    }
    else
    {
        throw logic_error( "cannot find db/icwb/utf under IZENECMA(environment variable)." );
    }
}

#define TO_STRING_(x) #x "/knowledge"
#define TO_STRING(x) TO_STRING_(x)
#define KMA_KNOWLEDGE TO_STRING(WISEKMA)

#define TO_CMA_STRING_(x) #x "/db/icwb/utf8"
#define TO_CMA_STRING(x) TO_CMA_STRING_(x)
#define CMA_KNOWLEDGE TO_CMA_STRING(IZENECMA)



#endif  //_LA_TEST_DEF_H_
