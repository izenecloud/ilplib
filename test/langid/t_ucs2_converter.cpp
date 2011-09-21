///
/// @file t_ucs2_converter.cpp
/// @brief test converting UTF-8/UTF-16 to UCS2
/// @author Jun Jiang
/// @date Created 2011-09-21
///

#include <ucs2_converter.h>
#include <util/ustring/UString.h>

#include <boost/test/unit_test.hpp>
#include <cstring>

using namespace ilplib::langid;
using namespace std;

namespace
{

void checkUTF8(const char* str, size_t goldMBLen, unsigned short goldUCS2)
{
    typedef UCS2_Converter<ENCODING_ID_UTF8> UCSConv;

    size_t mblen;
    unsigned short ucs2 = UCSConv::convertToUCS2(str, str+strlen(str), &mblen);

    BOOST_CHECK_EQUAL(mblen, goldMBLen);
    BOOST_CHECK_EQUAL(ucs2, goldUCS2);
}

void checkUTF16(const char* str, size_t goldMBLen, unsigned short goldUCS2)
{
    typedef UCS2_Converter<ENCODING_ID_UTF16> UCSConv;

    izenelib::util::UString ustr(str, UString::UTF_8);
    const char* start = reinterpret_cast<const char*>(ustr.data());
    size_t mblen;
    unsigned short ucs2 = UCSConv::convertToUCS2(start, start+ustr.size(), &mblen);

    BOOST_CHECK_EQUAL(mblen, goldMBLen);
    BOOST_CHECK_EQUAL(ucs2, goldUCS2);
}

}

BOOST_AUTO_TEST_SUITE(LangidTest)

BOOST_AUTO_TEST_CASE(checkConvertFromUTF8)
{
    checkUTF8("", 0, 0);
    checkUTF8("?!@#", 1, 0x3F);
    checkUTF8("IJK", 1, 0x49);
    checkUTF8("我们", 3, 0x6211);
    checkUTF8("妳們", 3, 0x59B3);
    checkUTF8("はひふへほ", 3, 0x306F);
    checkUTF8("안녕하", 3, 0xC548);
}

BOOST_AUTO_TEST_CASE(checkConvertFromUTF16)
{
    checkUTF16("", 0, 0);
    checkUTF16("?!@#", 2, 0x3F);
    checkUTF16("IJK", 2, 0x49);
    checkUTF16("我们", 2, 0x6211);
    checkUTF16("妳們", 2, 0x59B3);
    checkUTF16("はひふへほ", 2, 0x306F);
    checkUTF16("안녕하", 2, 0xC548);
}

BOOST_AUTO_TEST_SUITE_END() 
