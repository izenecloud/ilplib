/**
 * @file    EnglishUtil.h
 * @author  Vernkin
 * @date    Dec 14, 2009
 * @details
 *  Utility class for English processing
 */

#ifndef ENGLISHUTIL_H_
#define ENGLISHUTIL_H_

#include <util/ustring/UString.h>


namespace la
{

inline bool containUpperLetter( wiselib::UString& str )
{
    size_t len = str.length();
    for( size_t i = 0; i < len; ++i )
    {
        if( str.isUpperChar( i ) )
            return true;
    }
    return false;
}

inline bool isAllPunctuations( wiselib::UString& str )
{
    size_t len = str.length();
    for( size_t i = 0; i < len; ++i )
    {
        if( !str.isPunctuationChar( i ) )
            return false;
    }
    return true;
}

}
#endif /* ENGLISHUTIL_H_ */
