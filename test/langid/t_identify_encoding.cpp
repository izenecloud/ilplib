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

BOOST_AUTO_TEST_CASE(checkEncodingFromEnglishString)
{
    Analyzer* analyzer = getAnalyzer();

    EncodingID id;
    string utf8 = "Hello, how are you?";
    BOOST_CHECK(analyzer->encodingFromString(utf8.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_UTF8);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromEnglishFile)
{
    Analyzer* analyzer = getAnalyzer();

    EncodingID id;
    string fileName = createTestFile("utf8.txt", "Hello, how are you?");
    BOOST_CHECK(analyzer->encodingFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_UTF8);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromChineseString)
{
    Analyzer* analyzer = getAnalyzer();

    EncodingID id;
    string utf8 = "中华人民共和国";
    BOOST_CHECK(analyzer->encodingFromString(utf8.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_UTF8);

    izenelib::util::UString ustr(utf8, UString::UTF_8);
    string gb2312;
    ustr.convertString(gb2312, UString::GB2312);
    BOOST_CHECK(analyzer->encodingFromString(gb2312.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_GB18030);

    string big5;
    ustr.convertString(big5, UString::BIG5);
    BOOST_CHECK(analyzer->encodingFromString(big5.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_BIG5);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromChineseFile)
{
    Analyzer* analyzer = getAnalyzer();

    EncodingID id;
    string utf8 = "中华人民共和国";
    string fileName = createTestFile("utf8.txt", utf8.c_str());
    BOOST_CHECK(analyzer->encodingFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_UTF8);

    izenelib::util::UString ustr(utf8, UString::UTF_8);
    string gb2312;
    ustr.convertString(gb2312, UString::GB2312);
    fileName = createTestFile("gb2312.txt", gb2312.c_str());
    BOOST_CHECK(analyzer->encodingFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_GB18030);

    string big5;
    ustr.convertString(big5, UString::BIG5);
    fileName = createTestFile("big5.txt", big5.c_str());
    BOOST_CHECK(analyzer->encodingFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_BIG5);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromJapaneseString)
{
    Analyzer* analyzer = getAnalyzer();

    EncodingID id;
    string utf8 = "おはようございます";
    BOOST_CHECK(analyzer->encodingFromString(utf8.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_UTF8);

    izenelib::util::UString ustr(utf8, UString::UTF_8);
    string eucjp;
    ustr.convertString(eucjp, UString::EUC_JP);
    BOOST_CHECK(analyzer->encodingFromString(eucjp.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_EUCJP);

    string sjis;
    ustr.convertString(sjis, UString::SJIS);
    BOOST_CHECK(analyzer->encodingFromString(sjis.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_SHIFT_JIS);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromJapaneseFile)
{
    Analyzer* analyzer = getAnalyzer();

    EncodingID id;
    string utf8 = "おはようございます";
    string fileName = createTestFile("utf8.txt", utf8.c_str());
    BOOST_CHECK(analyzer->encodingFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_UTF8);

    izenelib::util::UString ustr(utf8, UString::UTF_8);
    string eucjp;
    ustr.convertString(eucjp, UString::EUC_JP);
    fileName = createTestFile("eucjp.txt", eucjp.c_str());
    BOOST_CHECK(analyzer->encodingFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_EUCJP);

    string sjis;
    ustr.convertString(sjis, UString::SJIS);
    fileName = createTestFile("sjis.txt", sjis.c_str());
    BOOST_CHECK(analyzer->encodingFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_SHIFT_JIS);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromKoreanString)
{
    Analyzer* analyzer = getAnalyzer();

    EncodingID id;
    string utf8 = "환영. 안녕하세요.";
    BOOST_CHECK(analyzer->encodingFromString(utf8.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_UTF8);

    izenelib::util::UString ustr(utf8, UString::UTF_8);
    string euckr;
    ustr.convertString(euckr, UString::EUC_KR);
    BOOST_CHECK(analyzer->encodingFromString(euckr.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_EUCKR);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromKoreanFile)
{
    Analyzer* analyzer = getAnalyzer();

    EncodingID id;
    string utf8 = "환영. 안녕하세요.";
    string fileName = createTestFile("utf8.txt", utf8.c_str());
    BOOST_CHECK(analyzer->encodingFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_UTF8);

    izenelib::util::UString ustr(utf8, UString::UTF_8);
    string euckr;
    ustr.convertString(euckr, UString::EUC_KR);
    fileName = createTestFile("euckr.txt", euckr.c_str());
    BOOST_CHECK(analyzer->encodingFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_EUCKR);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromSpecialString)
{
    Analyzer* analyzer = getAnalyzer();

    EncodingID id;
    string utf8 = "?@#$%";
    BOOST_CHECK(analyzer->encodingFromString(utf8.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_UTF8);

    // empty string is assumed as UTF-8
    BOOST_CHECK(analyzer->encodingFromString("", id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_UTF8);

    BOOST_CHECK(analyzer->encodingFromString(NULL, id) == false);
}

BOOST_AUTO_TEST_CASE(checkEncodingFromSpecialFile)
{
    Analyzer* analyzer = getAnalyzer();

    EncodingID id;
    string utf8 = "?@#$%";
    string fileName = createTestFile("utf8.txt", utf8.c_str());
    BOOST_CHECK(analyzer->encodingFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_UTF8);

    // empty file is assumed as UTF-8
    fileName = createTestFile("empty.txt", "");
    BOOST_CHECK(analyzer->encodingFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, ENCODING_ID_UTF8);

    BOOST_CHECK(analyzer->encodingFromFile("", id) == false);
    BOOST_CHECK(analyzer->encodingFromFile(NULL, id) == false);
}

BOOST_AUTO_TEST_SUITE_END() 
