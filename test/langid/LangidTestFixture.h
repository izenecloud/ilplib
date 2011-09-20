///
/// @file LangidTestFixture.h
/// @brief fixture class for language identification test
/// @author Jun Jiang
/// @date Created 2011-09-19
///

#ifndef LANGID_TEST_FIXTURE_H
#define LANGID_TEST_FIXTURE_H

#include <langid/langid.h>
#include <util/ustring/UString.h>

#include <vector>
#include <string>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>

NS_ILPLIB_LANGID_BEGIN

class LangidTestFixture
{
public:
    LangidTestFixture();
    ~LangidTestFixture();

    ilplib::langid::Analyzer* getAnalyzer() { return analyzer_; }

    std::string createTestFile(const char* fileName, const char* content) const;

    void checkEncoding(const char* str, EncodingID gold);

    void checkPrimaryLanguage(const char* str, LanguageID gold);

    void checkLanguageList(
        const std::vector<string>& languageBlocks,
        const std::vector<LanguageID>& goldLangIDs
    );

    void checkSegmentLanguage(
        const std::vector<string>& languageBlocks,
        const std::vector<LanguageID>& goldLangIDs
    );

    void checkTokenizeSentence(const vector<string>& sentVec);

private:
    void checkTokenizeUTF8String_(const vector<string>& sentVec);
    void checkTokenizeUString_(const vector<izenelib::util::UString>& sentVec);

private:
    ilplib::langid::Analyzer* analyzer_;
    ilplib::langid::Knowledge* knowledge_;
    const boost::filesystem::path testDir_;
};

NS_ILPLIB_LANGID_END

#endif // LANGID_TEST_FIXTURE_H
