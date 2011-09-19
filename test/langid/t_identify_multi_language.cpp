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
const string LANG_BLOCK[] = {"中國。\n妳們好！",
                             "중국. 환영.\n안녕하세요.",
                             "ちゅうごく。シャンハイ。\nいらっしゃい.今日は!",
                             "China. Shanghai. Beijing. Hong Kong.@#$%\nHow are you?",
                             "中国。上海。北京。香港。\n世界欢迎你. 你们好！"};

const LanguageID LANG_ID[] = {LANGUAGE_ID_CHINESE_TRADITIONAL,
                              LANGUAGE_ID_KOREAN,
                              LANGUAGE_ID_JAPANESE,
                              LANGUAGE_ID_ENGLISH,
                              LANGUAGE_ID_CHINESE_SIMPLIFIED};

const unsigned int LANG_NUM = sizeof(LANG_ID) / sizeof(LanguageID);

const string MULTI_LANG_BLOCK = LANG_BLOCK[0] + LANG_BLOCK[1] + LANG_BLOCK[2]
                                + LANG_BLOCK[3] + LANG_BLOCK[4];

void checkLanguageIDList(const vector<LanguageID>& idVec)
{
    BOOST_CHECK_EQUAL(idVec.size(), LANG_NUM);
    // sorted by sentence count
    BOOST_CHECK_EQUAL(idVec[0], LANGUAGE_ID_CHINESE_SIMPLIFIED);
    BOOST_CHECK_EQUAL(idVec[1], LANGUAGE_ID_ENGLISH);
    BOOST_CHECK_EQUAL(idVec[2], LANGUAGE_ID_JAPANESE);
    BOOST_CHECK_EQUAL(idVec[3], LANGUAGE_ID_KOREAN);
    BOOST_CHECK_EQUAL(idVec[4], LANGUAGE_ID_CHINESE_TRADITIONAL);
}

void checkLanguageRegionList(const vector<LanguageRegion>& regionVec)
{
    BOOST_CHECK_EQUAL(regionVec.size(), LANG_NUM);

    unsigned int pos = 0;
    for (unsigned int i=0; i<LANG_NUM; ++i)
    {
        const LanguageRegion& region = regionVec[i];
        BOOST_TEST_MESSAGE("region " << i << ": " << LANG_BLOCK[i]);
        BOOST_CHECK_EQUAL(region.languageID_, LANG_ID[i]);
        BOOST_CHECK_EQUAL(region.start_, pos);
        BOOST_CHECK_EQUAL(region.length_, LANG_BLOCK[i].size());

        pos += LANG_BLOCK[i].size();
    }
}

}

BOOST_FIXTURE_TEST_SUITE(LangidTest, LangidTestFixture)

BOOST_AUTO_TEST_CASE(checkLanguageListFromString)
{
    Analyzer* analyzer = getAnalyzer();

    vector<LanguageID> idVec;
    BOOST_CHECK(analyzer->languageListFromString(MULTI_LANG_BLOCK.c_str(), idVec));
    checkLanguageIDList(idVec);

    BOOST_CHECK(analyzer->languageListFromString("?@#$%", idVec));
    BOOST_CHECK_EQUAL(idVec.size(), 1U);
    BOOST_CHECK_EQUAL(idVec[0], LANGUAGE_ID_UNKNOWN);

    BOOST_CHECK(analyzer->languageListFromString("", idVec));
    BOOST_CHECK_EQUAL(idVec.size(), 1U);
    BOOST_CHECK_EQUAL(idVec[0], LANGUAGE_ID_UNKNOWN);

    BOOST_CHECK(analyzer->languageListFromString(NULL, idVec) == false);
}

BOOST_AUTO_TEST_CASE(checkLanguageListFromFile)
{
    Analyzer* analyzer = getAnalyzer();

    vector<LanguageID> idVec;

    string fileName = createTestFile("multi.txt", MULTI_LANG_BLOCK.c_str());
    BOOST_CHECK(analyzer->languageListFromFile(fileName.c_str(), idVec));
    checkLanguageIDList(idVec);

    fileName = createTestFile("symbol.txt", "?@#$%");
    BOOST_CHECK(analyzer->languageListFromFile(fileName.c_str(), idVec));
    BOOST_CHECK_EQUAL(idVec.size(), 1U);
    BOOST_CHECK_EQUAL(idVec[0], LANGUAGE_ID_UNKNOWN);

    fileName = createTestFile("empty.txt", "");
    BOOST_CHECK(analyzer->languageListFromFile(fileName.c_str(), idVec));
    BOOST_CHECK_EQUAL(idVec.size(), 1U);
    BOOST_CHECK_EQUAL(idVec[0], LANGUAGE_ID_UNKNOWN);

    BOOST_CHECK(analyzer->languageListFromFile("", idVec) == false);
    BOOST_CHECK(analyzer->languageListFromFile(NULL, idVec) == false);
}

BOOST_AUTO_TEST_CASE(checkSegmentString)
{
    Analyzer* analyzer = getAnalyzer();

    vector<LanguageRegion> regionVec;
    BOOST_CHECK(analyzer->segmentString(MULTI_LANG_BLOCK.c_str(), regionVec));
    checkLanguageRegionList(regionVec);

    string text = "?@#$%";
    BOOST_CHECK(analyzer->segmentString(text.c_str(), regionVec));
    BOOST_CHECK_EQUAL(regionVec.size(), 1U);
    BOOST_CHECK_EQUAL(regionVec[0].languageID_, LANGUAGE_ID_UNKNOWN);
    BOOST_CHECK_EQUAL(regionVec[0].start_, 0U);
    BOOST_CHECK_EQUAL(regionVec[0].length_, text.size());

    BOOST_CHECK(analyzer->segmentString("", regionVec));
    BOOST_CHECK_EQUAL(regionVec.size(), 0U);

    BOOST_CHECK(analyzer->segmentString(NULL, regionVec) == false);
}

BOOST_AUTO_TEST_CASE(checkSegmentFile)
{
    Analyzer* analyzer = getAnalyzer();

    vector<LanguageRegion> regionVec;

    string fileName = createTestFile("multi.txt", MULTI_LANG_BLOCK.c_str());
    BOOST_CHECK(analyzer->segmentFile(fileName.c_str(), regionVec));
    checkLanguageRegionList(regionVec);

    string text = "?@#$%";
    fileName = createTestFile("symbol.txt", text.c_str());
    BOOST_CHECK(analyzer->segmentFile(fileName.c_str(), regionVec));
    BOOST_CHECK_EQUAL(regionVec.size(), 1U);
    BOOST_CHECK_EQUAL(regionVec[0].languageID_, LANGUAGE_ID_UNKNOWN);
    BOOST_CHECK_EQUAL(regionVec[0].start_, 0U);
    BOOST_CHECK_EQUAL(regionVec[0].length_, text.size());

    fileName = createTestFile("empty.txt", "");
    BOOST_CHECK(analyzer->segmentFile(fileName.c_str(), regionVec));
    BOOST_CHECK_EQUAL(regionVec.size(), 0U);

    BOOST_CHECK(analyzer->segmentFile("", regionVec) == false);
    BOOST_CHECK(analyzer->segmentFile(NULL, regionVec) == false);
}

BOOST_AUTO_TEST_SUITE_END() 
