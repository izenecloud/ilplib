/**
 * @file KNlpTokenizeTestFixture.h
 * @brief fixture class to test ilplib::knlp::Tokenize on forward maximize match.
 */

#ifndef KNLP_TOKENIZE_TEST_FIXTURE_H
#define KNLP_TOKENIZE_TEST_FIXTURE_H

#include <knlp/tokenize.h>
#include <string>
#include <map>
#include <boost/scoped_ptr.hpp>

class KNlpTokenizeTestFixture
{
public:
    KNlpTokenizeTestFixture();

    void init(const std::string& dictContent);

    void checkFMM(const std::string& raw, const std::string& gold);

private:
    void initScoreMap_(const std::string& dictContent);

private:
    typedef std::map<std::string, double> TokenScoreMap;
    TokenScoreMap scoreMap_;

    double dictMinScore_;

    boost::scoped_ptr<ilplib::knlp::Tokenize> tokenizer_;
};

#endif // KNLP_TOKENIZE_TEST_FIXTURE_H
