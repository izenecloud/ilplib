/** \file knowledge.cpp
 * Implementation of class Knowledge.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Oct 30, 2009
 */

#include "langid/knowledge.h"

#include <string>
#include <cassert>

using namespace std;

namespace
{
/** language name for each language type */
const char* LANGUAGE_NAME[ilplib::langid::LANGUAGE_ID_NUM] = {"Unknown", "English", "Chinese Simplified", "Chinese Traditional", "Japanese", "Korean"};

/** encoding name for each encoding type */
const char* ENCODING_NAME[ilplib::langid::ENCODING_ID_NUM] = {"UTF-8", "GB18030", "BIG5", "EUC-JP", "Shift_JIS", "EUC-KR"};

/** string for undefined type */
const char* UNDEFINED_NAME = "UNDEFINED";

/**
 * Get a string in lower alphabets.
 * \param s the original string
 * \return the converted string with lower alphabets. For example, "euc-jp" is returned when "EUC-JP" is input.
 */
string toLower(const char* s)
{
    string str(s);
    for(size_t i=0; i<str.size(); ++i)
    {
        char c = str[i];
        if((c >= 'A') && (c <= 'Z'))
        {
            c += 'a' - 'A';
            str[i] = c;
        }
    }
    return str;
}
} // namespace

NS_ILPLIB_LANGID_BEGIN

Knowledge::Knowledge()
{
}

Knowledge::~Knowledge()
{
}

LanguageID Knowledge::getLanguageIDFromName(const char* name)
{
    if(name)
    {
        string lower = toLower(name);
        for(int i=0; i<LANGUAGE_ID_NUM; ++i)
        {
            if(lower == toLower(LANGUAGE_NAME[i]))
            {
                return static_cast<LanguageID>(i);
            }
        }
    }

    // undefined language type
    return LANGUAGE_ID_NUM;
}

const char* Knowledge::getLanguageNameFromID(LanguageID id)
{
    if(id >= 0 && id < LANGUAGE_ID_NUM)
    {
        return LANGUAGE_NAME[id];
    }

    return UNDEFINED_NAME;
}

EncodingID Knowledge::getEncodingIDFromName(const char* name)
{
    if(name)
    {
        string lower = toLower(name);
        for(int i=0; i<ENCODING_ID_NUM; ++i)
        {
            if(lower == toLower(ENCODING_NAME[i]))
            {
                return static_cast<EncodingID>(i);
            }
        }
    }

    // undefined encoding type
    return ENCODING_ID_NUM ;
}

const char* Knowledge::getEncodingNameFromID(EncodingID id)
{
    if(id >= 0 && id < ENCODING_ID_NUM)
    {
        return ENCODING_NAME[id];
    }

    return UNDEFINED_NAME;
}

NS_ILPLIB_LANGID_END
