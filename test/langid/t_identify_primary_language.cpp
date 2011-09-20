///
/// @file t_identify_primary_language.cpp
/// @brief test identifying primary language from string and file
/// @author Jun Jiang
/// @date Created 2011-09-19
///

#include "LangidTestFixture.h"

#include <boost/test/unit_test.hpp>

using namespace ilplib::langid;

BOOST_FIXTURE_TEST_SUITE(LangidTest, LangidTestFixture)

BOOST_AUTO_TEST_CASE(checkIdentifyLanguageFail)
{
    Analyzer* analyzer = getAnalyzer();

    LanguageID id;
    BOOST_CHECK(analyzer->languageFromString(NULL, id) == false);
    BOOST_CHECK(analyzer->languageFromFile(NULL, id) == false);
    BOOST_CHECK(analyzer->languageFromFile("", id) == false);
}

BOOST_AUTO_TEST_CASE(checkIdentifyPrimaryLanguage)
{
    checkPrimaryLanguage("Hello. How are you? 中国", LANGUAGE_ID_ENGLISH);
    checkPrimaryLanguage("中国。你好！How are you?", LANGUAGE_ID_CHINESE_SIMPLIFIED);
    checkPrimaryLanguage("How are you?中國。妳好！", LANGUAGE_ID_CHINESE_TRADITIONAL);
    checkPrimaryLanguage("にほん。こんにちは。中国。Hello.", LANGUAGE_ID_JAPANESE);
    checkPrimaryLanguage("한국.안녕하 세요.中国。Hello.", LANGUAGE_ID_KOREAN);
    checkPrimaryLanguage("?@#$%", LANGUAGE_ID_UNKNOWN);
    checkPrimaryLanguage("", LANGUAGE_ID_UNKNOWN);
}

BOOST_AUTO_TEST_SUITE_END() 
