/**
 * @file    PlainDictionary.cpp
 * @author  Vernkin
 * @date    Dec 14, 2009
 * @details
 *  Plain Dictionary only contains a set of words, without any extra information. And can Save to/Load
 *  from the file.
 */

#include "la/dict/PlainDictionary.h"

#include <fstream>

using izenelib::util::UString;

namespace la
{

PlainDictionary::PlainDictionary( UString::EncodingType encoding )
    : encoding_( encoding ),
    ignoreNoExistFile_( false )
{
}

PlainDictionary::~PlainDictionary()
{
}

int PlainDictionary::loadDict( const char* srcFile, bool ignoreNoExistFile )
{
    izenelib::util::ScopedWriteLock<izenelib::util::ReadWriteLock> swl( lock_ );
    ignoreNoExistFile_ = ignoreNoExistFile;
    ifstream in( srcFile );
	if( !in.good() )
	{
		if( ignoreNoExistFile_ )
		{
			destPath_ = srcFile;
			return 0;
		}
		cerr << "Can't load PlainDictionary from " << srcFile << endl;
		return 1;
	}

	string line;
	while( !in.eof() )
	{
		getline( in, line );
		if( !line.empty() )
		{
			UString ustr( line.c_str(), encoding_ );
		    words_[ ustr ] = true;
		}
	}
	in.close();
	destPath_ = srcFile;

	return 0;
}

void PlainDictionary::display( ostream& out )
{
    izenelib::util::ScopedReadLock<izenelib::util::ReadWriteLock> srl( lock_ );
    for( DictType::iterator itr = words_.begin(); itr != words_.end(); ++itr )
    {
        itr->first.displayStringInfo( encoding_, out ); out<<endl;
    }
}

}
