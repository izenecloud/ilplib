///
/// @file t_identify_multi_language.cpp
/// @brief test identifying multiple languages from string and file
/// @author Jun Jiang
/// @date Created 2011-09-19
///

#include "LangidTestFixture.h"

#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace boost;
using namespace ilplib::langid;

namespace
{

void generateLanguageBlocks(
    vector<string>& languageBlocks,
    vector<LanguageID>& blockLangIDs,
    vector<LanguageID>& goldLangIDs
)
{
    languageBlocks.push_back("中國。\n妳們好！");
    blockLangIDs.push_back(LANGUAGE_ID_CHINESE_TRADITIONAL);

    languageBlocks.push_back("중국. 환영.\n안녕하세요.");
    blockLangIDs.push_back(LANGUAGE_ID_KOREAN);

    languageBlocks.push_back("ちゅうごく。シャンハイ。\nいらっしゃい.今日は!");
    blockLangIDs.push_back(LANGUAGE_ID_JAPANESE);

    languageBlocks.push_back("China. Shanghai. Beijing. Hong Kong.@#$%\nHow are you?");
    blockLangIDs.push_back(LANGUAGE_ID_ENGLISH);

    languageBlocks.push_back("中国。上海。北京。香港。\n世界欢迎你. 你们好！");
    blockLangIDs.push_back(LANGUAGE_ID_CHINESE_SIMPLIFIED);

    goldLangIDs.push_back(LANGUAGE_ID_CHINESE_SIMPLIFIED);
    goldLangIDs.push_back(LANGUAGE_ID_ENGLISH);
    goldLangIDs.push_back(LANGUAGE_ID_JAPANESE);
    goldLangIDs.push_back(LANGUAGE_ID_KOREAN);
    goldLangIDs.push_back(LANGUAGE_ID_CHINESE_TRADITIONAL);
}

}

BOOST_FIXTURE_TEST_SUITE(LangidTest, LangidTestFixture)

BOOST_AUTO_TEST_CASE(checkLanguageListFail)
{
    Analyzer* analyzer = getAnalyzer();
    vector<LanguageID> idVec;

    BOOST_CHECK(analyzer->languageListFromString(NULL, idVec) == false);
    BOOST_CHECK(analyzer->languageListFromFile("", idVec) == false);
    BOOST_CHECK(analyzer->languageListFromFile(NULL, idVec) == false);
}

BOOST_AUTO_TEST_CASE(checkLanguageUnknown)
{
    std::vector<string> languageBlocks;
    std::vector<LanguageID> goldLangIDs;

    goldLangIDs.push_back(LANGUAGE_ID_UNKNOWN);
    checkLanguageList(languageBlocks, goldLangIDs); 

    languageBlocks.push_back("?@#$%");
    checkLanguageList(languageBlocks, goldLangIDs); 
}

BOOST_AUTO_TEST_CASE(checkMultipleLanguage)
{
    std::vector<string> languageBlocks;
    std::vector<LanguageID> blockLangIDs;
    std::vector<LanguageID> goldLangIDs;

    generateLanguageBlocks(languageBlocks, blockLangIDs, goldLangIDs);
    checkLanguageList(languageBlocks, goldLangIDs); 
}

BOOST_AUTO_TEST_CASE(checkSegmentFail)
{
    Analyzer* analyzer = getAnalyzer();
    vector<LanguageRegion> regionVec;

    BOOST_CHECK(analyzer->segmentString(NULL, regionVec) == false);
    BOOST_CHECK(analyzer->segmentFile("", regionVec) == false);
    BOOST_CHECK(analyzer->segmentFile(NULL, regionVec) == false);
}

BOOST_AUTO_TEST_CASE(checkSegmentUnknown)
{
    std::vector<string> languageBlocks;
    std::vector<LanguageID> blockLangIDs;

    checkSegmentLanguage(languageBlocks, blockLangIDs);

    languageBlocks.push_back("?@#$%");
    blockLangIDs.push_back(LANGUAGE_ID_UNKNOWN);
    checkSegmentLanguage(languageBlocks, blockLangIDs);
}

BOOST_AUTO_TEST_CASE(checkSegmentString)
{
    std::vector<string> languageBlocks;
    std::vector<LanguageID> blockLangIDs;
    std::vector<LanguageID> goldLangIDs;

    generateLanguageBlocks(languageBlocks, blockLangIDs, goldLangIDs);
    checkSegmentLanguage(languageBlocks, blockLangIDs);
}

BOOST_AUTO_TEST_SUITE_END() 
