///
/// @file LangidTestFixture.h
/// @brief fixture class for language identification test
/// @author Jun Jiang
/// @date Created 2011-09-19
///

#include <langid/langid.h>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <fstream>

class LangidTestFixture
{
public:
    LangidTestFixture()
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

    ~LangidTestFixture()
    {
        boost::filesystem::remove_all(testDir_);

        delete knowledge_;
        delete analyzer_;
    }

    ilplib::langid::Analyzer* getAnalyzer()
    {
        return analyzer_;
    }

    std::string createTestFile(const char* fileName, const char* content)
    {
        std::string path = (testDir_ / fileName).string();
        std::ofstream ofs(path.c_str());
        BOOST_REQUIRE(ofs);

        ofs << content;
        return path;
    }

private:
    ilplib::langid::Analyzer* analyzer_;
    ilplib::langid::Knowledge* knowledge_;
    const boost::filesystem::path testDir_;
};
