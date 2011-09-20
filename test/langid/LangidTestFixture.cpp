///
/// @file LangidTestFixture.h
/// @brief implementation of LangidTestFixture
/// @author Jun Jiang
/// @date Created 2011-09-20
///

#include "LangidTestFixture.h"

#include <fstream>
#include <boost/filesystem.hpp>

using namespace std;
using namespace ilplib::langid;

namespace
{

// T should be std::string or izenelib::util::UString
template<typename T>
void combine(const vector<T>& source, T& target)
{
    for (typename vector<T>::const_iterator it = source.begin();
        it != source.end(); ++it)
    {
        target += *it;
    }
}

void convert(
    const vector<string>& source,
    vector<izenelib::util::UString>& target
)
{
    for (vector<string>::const_iterator it = source.begin();
        it != source.end(); ++it)
    {
        izenelib::util::UString ustr(*it, UString::UTF_8);
        target.push_back(ustr);
    }
}

void checkLangIdList(
    const vector<LanguageID>& resultLangIDs,
    const vector<LanguageID>& goldLangIDs
)
{
    BOOST_CHECK_EQUAL(resultLangIDs.size(), goldLangIDs.size());
    for (unsigned int i=0; i<resultLangIDs.size(); ++i)
    {
        BOOST_CHECK_EQUAL(resultLangIDs[i], goldLangIDs[i]);
    }
}

// T should be std::string or izenelib::util::UString
template<typename T>
void checkRegionList(
    const vector<LanguageRegion>& resultRegions,
    const std::vector<T>& languageBlocks,
    const std::vector<LanguageID>& goldLangIDs
)
{
    BOOST_CHECK_EQUAL(resultRegions.size(), languageBlocks.size());
    BOOST_CHECK_EQUAL(goldLangIDs.size(), languageBlocks.size());

    unsigned int pos = 0;
    for (unsigned int i=0; i<resultRegions.size(); ++i)
    {
        const LanguageRegion& region = resultRegions[i];
        BOOST_CHECK_EQUAL(region.languageID_, goldLangIDs[i]);
        BOOST_CHECK_EQUAL(region.start_, pos);
        BOOST_CHECK_EQUAL(region.length_, languageBlocks[i].length());

        pos += languageBlocks[i].length();
    }
}

}

LangidTestFixture::LangidTestFixture()
    : analyzer_(NULL)
    , knowledge_(NULL)
    , testDir_("LangidTestFixture")
{
    ilplib::langid::Factory* factory = ilplib::langid::Factory::instance();
    analyzer_ = factory->createAnalyzer();
    knowledge_ = factory->createKnowledge();

    const char* encodingModel = "../db/langid/model/encoding.bin";
    const char* languageModel = "../db/langid/model/language.bin";

    BOOST_REQUIRE(knowledge_->loadEncodingModel(encodingModel));
    BOOST_REQUIRE(knowledge_->loadLanguageModel(languageModel));

    analyzer_->setKnowledge(knowledge_);

    boost::filesystem::remove_all(testDir_);
    boost::filesystem::create_directory(testDir_);
}

LangidTestFixture::~LangidTestFixture()
{
    boost::filesystem::remove_all(testDir_);

    delete knowledge_;
    delete analyzer_;
}

std::string LangidTestFixture::createTestFile(const char* fileName, const char* content) const
{
    std::string path = (testDir_ / fileName).string();
    std::ofstream ofs(path.c_str());
    BOOST_REQUIRE(ofs);

    ofs << content;
    return path;
}

void LangidTestFixture::checkEncoding(const char* str, EncodingID gold)
{
    EncodingID id;
    BOOST_CHECK(analyzer_->encodingFromString(str, id));
    BOOST_CHECK_EQUAL(id, gold);

    string fileName = createTestFile("raw.txt", str);
    BOOST_CHECK(analyzer_->encodingFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, gold);
}

void LangidTestFixture::checkPrimaryLanguage(const char* str, LanguageID gold)
{
    LanguageID id;
    BOOST_CHECK(analyzer_->languageFromString(str, id));
    BOOST_CHECK_EQUAL(id, gold);

    string fileName = createTestFile("raw.txt", str);
    BOOST_CHECK(analyzer_->languageFromFile(fileName.c_str(), id));
    BOOST_CHECK_EQUAL(id, gold);

    //TODO: test UString
    //izenelib::util::UString ustr(str, UString::UTF_8);
    //BOOST_CHECK(analyzer_->languageFromString(ustr, id));
    //BOOST_CHECK_EQUAL(id, gold);
}

void LangidTestFixture::checkLanguageList(
    const std::vector<string>& languageBlocks,
    const std::vector<LanguageID>& goldLangIDs
)
{
    vector<LanguageID> idVec;
    string totalBlock;
    combine(languageBlocks, totalBlock);

    BOOST_CHECK(analyzer_->languageListFromString(totalBlock.c_str(), idVec));
    checkLangIdList(idVec, goldLangIDs);

    string fileName = createTestFile("raw.txt", totalBlock.c_str());
    BOOST_CHECK(analyzer_->languageListFromFile(fileName.c_str(), idVec));
    checkLangIdList(idVec, goldLangIDs);

    //TODO: test UString
    //izenelib::util::UString ustr(totalBlock, UString::UTF_8);
    //BOOST_CHECK(analyzer_->languageListFromString(ustr, idVec));
    //checkLangIdList(idVec, goldLangIDs);
}

void LangidTestFixture::checkSegmentLanguage(
    const std::vector<string>& languageBlocks,
    const std::vector<LanguageID>& goldLangIDs
)
{
    vector<LanguageRegion> regionVec;
    string totalBlock;
    combine(languageBlocks, totalBlock);

    BOOST_CHECK(analyzer_->segmentString(totalBlock.c_str(), regionVec));
    checkRegionList(regionVec, languageBlocks, goldLangIDs);

    string fileName = createTestFile("raw.txt", totalBlock.c_str());
    BOOST_CHECK(analyzer_->segmentFile(fileName.c_str(), regionVec));
    checkRegionList(regionVec, languageBlocks, goldLangIDs);

    //TODO: test UString
    //vector<izenelib::util::UString> ustrBlocks;
    //convert(languageBlocks, ustrBlocks);
    //izenelib::util::UString ustr(totalBlock, UString::UTF_8);
    //BOOST_CHECK(analyzer_->segmentString(ustr, regionVec));
    //checkRegionList(regionVec, ustrBlocks, goldLangIDs);
}

void LangidTestFixture::checkTokenizeSentence(const vector<string>& sentVec)
{
    checkTokenizeUTF8String_(sentVec);

    //TODO: test UString
    //vector<izenelib::util::UString> ustrVec;
    //convert(sentVec, ustrVec);
    //checkTokenizeUString_(ustrVec);
}

void LangidTestFixture::checkTokenizeUTF8String_(const vector<string>& sentVec)
{
    string totalBlock;
    combine(sentVec, totalBlock);

    int i = 0;
    const char* p = totalBlock.c_str();
    string sent;
    while(size_t len = analyzer_->sentenceLength(p))
    {
        sent.assign(p, len);
        BOOST_TEST_MESSAGE("sentence " << i
                           << ", len: " << len
                           << ", text: " << sent);

        BOOST_CHECK_EQUAL(sent, sentVec[i]);

        p += len;
        ++i;
    }

    BOOST_CHECK_EQUAL(i, sentVec.size());
}

void LangidTestFixture::checkTokenizeUString_(const vector<izenelib::util::UString>& sentVec)
{
    izenelib::util::UString totalBlock;
    combine(sentVec, totalBlock);

    int i = 0;
    size_t pos = 0;
    izenelib::util::UString sent;
    //while(size_t len = analyzer->sentenceLength(totalBlock, pos))
    while(size_t len = 0)
    {
        sent.assign(totalBlock, pos, len);
        cout << sent.empty();
        BOOST_TEST_MESSAGE("sentence " << i
                           << ", len: " << len);

        BOOST_CHECK_EQUAL(sent, sentVec[i]);

        pos += len;
        ++i;
    }

    BOOST_CHECK_EQUAL(i, sentVec.size());
}
