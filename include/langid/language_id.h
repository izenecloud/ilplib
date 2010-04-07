/** \file language_id.h
 * Definition of language and encoding types.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Oct 30, 2009
 */

#ifndef LANGID_LANGUAGE_ID_H
#define LANGID_LANGUAGE_ID_H

#include <ilplib.h>

NS_ILPLIB_LANGID_BEGIN

/**
 * Language types.
 */
enum LanguageID
{
    LANGUAGE_ID_UNKNOWN, ///< language ID for unknown type
    LANGUAGE_ID_ENGLISH, ///< language ID for "English"
    LANGUAGE_ID_CHINESE_SIMPLIFIED, ///< language ID for "Chinese simplified"
    LANGUAGE_ID_CHINESE_TRADITIONAL, ///< language ID for "Chinese traditional"
    LANGUAGE_ID_JAPANESE, ///< language ID for "Japanese"
    LANGUAGE_ID_KOREAN, ///< language ID for "Korean"
    LANGUAGE_ID_NUM ///< the count of language types
};

/**
 * Encoding types.
 */
enum EncodingID
{
    ENCODING_ID_UTF8, ///< encoding ID for "UTF-8"
    ENCODING_ID_GB18030, ///< encoding ID for "GB18030"
    ENCODING_ID_BIG5, ///< encoding ID for "BIG5"
    ENCODING_ID_EUCJP, ///< encoding ID for "EUC-JP"
    ENCODING_ID_SHIFT_JIS, ///< encoding ID for "Shift_JIS"
    ENCODING_ID_EUCKR, ///< encoding ID for "EUC-KR"
    ENCODING_ID_NUM ///< the count of encoding types
};

NS_ILPLIB_LANGID_END

#endif // LANGID_LANGUAGE_ID_H
