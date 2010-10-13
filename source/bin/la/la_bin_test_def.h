#ifndef _LA_BIN_TEST_DEF_H_
#define _LA_BIN_TEST_DEF_H_

#include <boost/filesystem.hpp>
#include <util/ustring/UString.h>

using namespace boost::filesystem;
using namespace izenelib::util;

#define TO_STRING_(x) #x "/knowledge"
#define TO_STRING(x) TO_STRING_(x)
#define KMA_KNOWLEDGE TO_STRING(WISEKMA)

#define TO_CMA_STRING_(x) #x "/db/icwb/utf8"
#define TO_CMA_STRING(x) TO_CMA_STRING_(x)
#define CMA_KNOWLEDGE TO_CMA_STRING(IZENECMA)

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

UString::EncodingType IO_ENCODING = UString::UTF_8;

std::ostream & printTerm( const la::Term & term, std::ostream & out=cout )
{
    std::string tmp;
    term.text_.convertString(tmp, IO_ENCODING );
    out << "len="           << (term.text_.length())        << "\t";
    out << "pos="           << term.pos_                    << "\t";
    out << "woffset="       << term.wordOffset_             << "\t";
    out << "stats="         << bitset<8>(term.stats_)       << "\t";
    out << "\ttext=["       << tmp                          << "]";
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

    out << endl;
    return out;
}

#endif  //_LA_BIN_TEST_DEF_H_
