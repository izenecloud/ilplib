///
/// @file t_identify_encoding.cpp
/// @brief test identifying encoing from string and file
/// @author Jun Jiang
/// @date Created 2011-09-19
///

#include "LangidTestFixture.h"
#include <util/ustring/UString.h>

#include <boost/test/unit_test.hpp>
#include <string>

using namespace std;
using namespace boost;
using namespace ilplib::langid;

BOOST_FIXTURE_TEST_SUITE(LangidTest, LangidTestFixture)

BOOST_AUTO_TEST_CASE(checkIdentifyEncodingFail)
{
    Analyzer* analyzer = getAnalyzer();

    EncodingID id;
    BOOST_CHECK(analyzer->encodingFromString(NULL, id) == false);
    BOOST_CHECK(analyzer->encodingFromFile("", id) == false);
    BOOST_CHECK(analyzer->encodingFromFile(NULL, id) == false);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromEmpty)
{
    // empty content is assumed as UTF-8
    string empty;
    checkEncoding(empty.c_str(), ENCODING_ID_UTF8);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromEnglish)
{
    string utf8 = "Hello, how are you?";
    checkEncoding(utf8.c_str(), ENCODING_ID_UTF8);

    utf8 = "?@#$%";
    checkEncoding(utf8.c_str(), ENCODING_ID_UTF8);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromChinese)
{
    string utf8 = "中华人民共和国";
    checkEncoding(utf8.c_str(), ENCODING_ID_UTF8);

    izenelib::util::UString ustr(utf8, UString::UTF_8);
    string target;

    ustr.convertString(target, UString::GB2312);
    checkEncoding(target.c_str(), ENCODING_ID_GB18030);

    ustr.convertString(target, UString::BIG5);
    checkEncoding(target.c_str(), ENCODING_ID_BIG5);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromJapanese)
{
    string utf8 = "おはようございます";
    checkEncoding(utf8.c_str(), ENCODING_ID_UTF8);

    izenelib::util::UString ustr(utf8, UString::UTF_8);
    string target;

    ustr.convertString(target, UString::EUC_JP);
    checkEncoding(target.c_str(), ENCODING_ID_EUCJP);

    ustr.convertString(target, UString::SJIS);
    checkEncoding(target.c_str(), ENCODING_ID_SHIFT_JIS);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromKorean)
{
    string utf8 = "환영. 안녕하세요.";
    checkEncoding(utf8.c_str(), ENCODING_ID_UTF8);

    izenelib::util::UString ustr(utf8, UString::UTF_8);
    string target;

    ustr.convertString(target, UString::EUC_KR);
    checkEncoding(target.c_str(), ENCODING_ID_EUCKR);
}

BOOST_AUTO_TEST_SUITE_END() 
