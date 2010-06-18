#ifndef QUESTION_ANSWERING_H
#define QUESTION_ANSWERING_H

#include <ilplib.h>

#include <string>

namespace ilplib{ namespace qa{

class QuestionAnswering
{
public:
    QuestionAnswering();

    ~QuestionAnswering();

public:
    void questionAnalyzer(const std::string& question, std::string& result);
};

}}

#endif

