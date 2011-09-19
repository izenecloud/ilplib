///
/// @file t_identify_primary_language.cpp
/// @brief test identifying primary language from string and file
/// @author Jun Jiang
/// @date Created 2011-09-19
///

#include "LangidTestFixture.h"

#include <boost/test/unit_test.hpp>
#include <string>

using namespace std;
using namespace boost;
using namespace ilplib::langid;

BOOST_FIXTURE_TEST_SUITE(LangidTest, LangidTestFixture)

BOOST_AUTO_TEST_CASE(checkLanguageFromString)
{
    Analyzer* analyzer = getAnalyzer();

    LanguageID id;
    BOOST_CHECK(analyzer->languageFromString("Hello. How are you? 中国", id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_ENGLISH);

    BOOST_CHECK(analyzer->languageFromString("中国。你好！How are you?", id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_CHINESE_SIMPLIFIED);

    BOOST_CHECK(analyzer->languageFromString("How are you?中國。妳好！", id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_CHINESE_TRADITIONAL);

    BOOST_CHECK(analyzer->languageFromString("にほん。こんにちは。中国。Hello.", id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_JAPANESE);

    BOOST_CHECK(analyzer->languageFromString("한국.안녕하 세요.中国。Hello.", id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_KOREAN);

    BOOST_CHECK(analyzer->languageFromString("?@#$%", id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_UNKNOWN);

    BOOST_CHECK(analyzer->languageFromString("", id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_UNKNOWN);

    BOOST_CHECK(analyzer->languageFromString(NULL, id) == false);
}

BOOST_AUTO_TEST_CASE(checkLanguageFromFile)
{
    Analyzer* analyzer = getAnalyzer();

    LanguageID id;

    string fileName = createTestFile("en.txt", "Hello. How are you? 中国");
    BOOST_CHECK(analyzer->languageFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_ENGLISH);

    fileName = createTestFile("cs.txt", "中国。你好！How are you?");
    BOOST_CHECK(analyzer->languageFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_CHINESE_SIMPLIFIED);

    fileName = createTestFile("ct.txt", "How are you?中國。妳好！");
    BOOST_CHECK(analyzer->languageFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_CHINESE_TRADITIONAL);

    fileName = createTestFile("jp.txt", "にほん。こんにちは。中国。Hello.");
    BOOST_CHECK(analyzer->languageFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_JAPANESE);

    fileName = createTestFile("kr.txt", "한국.안녕하 세요.中国。Hello.");
    BOOST_CHECK(analyzer->languageFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_KOREAN);

    fileName = createTestFile("symbol.txt", "?@#$%");
    BOOST_CHECK(analyzer->languageFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_UNKNOWN);

    fileName = createTestFile("empty.txt", "");
    BOOST_CHECK(analyzer->languageFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, LANGUAGE_ID_UNKNOWN);

    BOOST_CHECK(analyzer->languageFromFile("", id) == false);
    BOOST_CHECK(analyzer->languageFromFile(NULL, id) == false);
}

BOOST_AUTO_TEST_SUITE_END() 
