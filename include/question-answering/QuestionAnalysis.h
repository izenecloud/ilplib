#ifndef QUESTION_ANALYSIS_H
#define QUESTION_ANALYSIS_H

#include <ilplib.h>

#include <util/ustring/UString.h>

#include <boost/unordered_set.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread/once.hpp>

#include <string>

namespace ilplib{ namespace qa{

class QuestionAnalysis
{
public:
    QuestionAnalysis();

    ~QuestionAnalysis();

public:
    static void load(const std::string& path);

    static void loadQuestionDict();

    bool isQuestion(const std::string& question);

    bool isQuestionTerm(const izenelib::util::UString& laUnit)
    {
        std::string term;
        laUnit.convertString(term, izenelib::util::UString::UTF_8);
        return questionWordList_.find(term) != questionWordList_.end();
    }

    bool isCandidateTerm(const std::string& posUnit)
    {
        return candidates_.find(posUnit) != candidates_.end();
    }

private:
    static boost::unordered_set<std::string> questionWordList_;

    static boost::unordered_set<std::string> candidates_;

    static std::string resourcePath_;
};

}}

#endif

