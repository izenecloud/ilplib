#include "KNlpTokenizeTestFixture.h"
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <fstream>

namespace bfs = boost::filesystem;

namespace
{
const string kTestDir = "knlp_test";
const string kDictPath = kTestDir + "/knlp_dict.txt";
const string kEncodeType = "utf-8";

void writeFile(const std::string& filePath, const std::string& content)
{
    std::ofstream ofs(filePath.c_str());
    ofs << content;
}

void splitTokens(const std::string& str, std::vector<std::string>& tokens)
{
    std::istringstream iss(str);
    std::string token;

    while (iss >> token)
    {
        tokens.push_back(token);
    }
}

}

KNlpTokenizeTestFixture::KNlpTokenizeTestFixture()
    : dictMinScore_(0)
{
    bfs::remove_all(kTestDir);
    bfs::create_directories(kTestDir);
}

void KNlpTokenizeTestFixture::init(const std::string& dictContent)
{
    initScoreMap_(dictContent);

    writeFile(kDictPath, dictContent);

    tokenizer_.reset(new ilplib::knlp::Tokenize(kDictPath));
}

void KNlpTokenizeTestFixture::initScoreMap_(const std::string& dictContent)
{
    std::istringstream iss(dictContent);
    std::string token;
    double score;

    while (iss >> token >> score)
    {
        scoreMap_[token] = score;
    }

    dictMinScore_ = scoreMap_["[MIN]"];
}

void KNlpTokenizeTestFixture::checkFMM(const std::string& raw, const std::string& gold)
{
    BOOST_TEST_MESSAGE("checkFMM, raw: " << raw << ", gold: " << gold);

    std::vector<std::string> goldTokens;
    splitTokens(gold, goldTokens);

    std::string norm(raw);
    ilplib::knlp::Normalize::normalize(norm);

    std::vector<std::pair<KString, double> > tokenScores;
    tokenizer_->fmm(KString(norm), tokenScores);

    std::size_t tokenNum = tokenScores.size();
    BOOST_REQUIRE_EQUAL(tokenNum, goldTokens.size());

    for (std::size_t i = 0; i < tokenNum; ++i)
    {
        std::string token = tokenScores[i].first.get_bytes(kEncodeType);
        BOOST_CHECK_EQUAL(token, goldTokens[i]);

        double actualScore = tokenScores[i].second;
        TokenScoreMap::const_iterator it = scoreMap_.find(token);
        double goldScore = it != scoreMap_.end() ? it->second : dictMinScore_;

        BOOST_TEST_MESSAGE("check token: " << token <<
                           ", actual score: " << actualScore <<
                           ", goldScore: " << goldScore);

        BOOST_CHECK_EQUAL(actualScore, goldScore);
    }
}
