/** \file ucs2_converter.h
 * Definition of class UCS2_Converter.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 18, 2009
 */

#ifndef LANGID_UCS2_CONVERTER_H
#define LANGID_UCS2_CONVERTER_H

#include "langid/language_id.h"

#include <cstddef> // std::size_t

namespace langid
{

/**
 * UCS2_Converter converts the first character in input string to UCS2 value.
 * Typically, the usage is like below:
 *
 * \code
 * // create instance of encoding type
 * Utf8ToUcs2 converter;
 *
 * const char* begin = "...";
 * const char* end = begin + strlen(begin);
 *
 * // print each character and its UCS2 value
 * size_t mblen;
 * for(const char* p=begin; p!=end; p+=mblen)
 * {
 *      unsigned short value = converter.convertToUCS2(p, end, &mblen);
 *      string str(p, mblen);
 *
 *      cout << str << "\t" << value << endl;
 * }
 * \endcode
 */
template<EncodingID id>
class UCS2_Converter
{
public:
    /**
     * Convert the character to its UCS2 value.
     * \param begin pointer to the first character
     * \param end pointer to the last character, the range used is [\e begin, \e end)
     * \param mblen stores how many bytes are consumed in the conversion
     * \return UCS2 value of the first character starting from \e begin in specific encode type
     * \attention 0 is returned if the character is out of UCS2
     * \attention if \e begin is not less than \e end, 0 is returned, and \e mblen would be 0.
     */
    unsigned short convertToUCS2(const char* begin, const char* end, std::size_t* mblen) const;
};

/**
 * UCS2_Converter<ENCODING_ID_UTF8> converts from UTF-8 to UCS2.
 */
template<>
class UCS2_Converter<ENCODING_ID_UTF8>
{
public:
    unsigned short convertToUCS2(const char* begin, const char* end, std::size_t* mblen) const
    {
        // in case of out of range [begin, end)
        if(begin >= end)
        {
            *mblen = 0;
            return 0;
        }

        const size_t len = end - begin;

        if(static_cast<unsigned char>(begin[0]) < 0x80)
        {
            *mblen = 1;
            return static_cast<unsigned char>(begin[0]);
        }
        else if(len >= 2 && (begin[0] & 0xe0) == 0xc0)
        {
            *mblen = 2;
            return ((begin[0] & 0x1f) << 6) | (begin[1] & 0x3f);
        }
        else if(len >= 3 && (begin[0] & 0xf0) == 0xe0)
        {
            *mblen = 3;
            return ((begin[0] & 0x0f) << 12) |
                ((begin[1] & 0x3f) << 6) |
                (begin[2] & 0x3f);
        }
        /* belows are out of UCS2 */
        else if(len >= 4 && (begin[0] & 0xf8) == 0xf0)
        {
            *mblen = 4;
            return 0;
        }
        else if(len >= 5 && (begin[0] & 0xfc) == 0xf8)
        {
            *mblen = 5;
            return 0;
        }
        else if(len >= 6 && (begin[0] & 0xfe) == 0xfc)
        {
            *mblen = 6;
            return 0;
        }
        else
        {
            *mblen = 1;
            return 0;
        }
    }
};

/** Utf8ToUcs2 converts from UTF-8 to UCS2. */
typedef UCS2_Converter<ENCODING_ID_UTF8> Utf8ToUcs2;

} // namespace langid

#endif // LANGID_UCS2_CONVERTER_H
