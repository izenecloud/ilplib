///
/// @file t_tokenize_sentence.cpp
/// @brief test tokenizing sentence
/// @author Jun Jiang
/// @date Created 2011-09-19
///

#include "LangidTestFixture.h"

#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace boost;
using namespace ilplib::langid;

BOOST_FIXTURE_TEST_SUITE(LangidTest, LangidTestFixture)

BOOST_AUTO_TEST_CASE(checkEmptyString)
{
    vector<string> sentVec;
    checkTokenizeSentence(sentVec);
}

BOOST_AUTO_TEST_CASE(checkNormal)
{
    vector<string> sentVec;
    sentVec.push_back("Hello, how are you?");
    sentVec.push_back("我很好，你好吗。");
    sentVec.push_back("中國,妳們好！");
    sentVec.push_back("こんにちは、宜しく。");
    sentVec.push_back("환영,안녕하세요.");
    checkTokenizeSentence(sentVec);
}

BOOST_AUTO_TEST_CASE(checkNewLine)
{
    vector<string> sentVec;
    sentVec.push_back("Hi!\n");
    sentVec.push_back("How are you\n");
    sentVec.push_back("\n");
    sentVec.push_back("你好吗？\n");
    checkTokenizeSentence(sentVec);
}

BOOST_AUTO_TEST_CASE(checkParenthesis)
{
    {
        vector<string> sentVec;
        sentVec.push_back("(\"Go.\") ");
        sentVec.push_back("(He did.)");
        checkTokenizeSentence(sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("(\"Go?\")\n");
        sentVec.push_back("(He did.)");
        checkTokenizeSentence(sentVec);
    }
}

BOOST_AUTO_TEST_CASE(checkFullStop)
{
    {
        vector<string> sentVec;
        sentVec.push_back("U.S.A. is");
        checkTokenizeSentence(sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("U.S.A? ");
        sentVec.push_back("He");
        checkTokenizeSentence(sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("3.4");
        checkTokenizeSentence(sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("c.d");
        checkTokenizeSentence(sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("the resp. leaders are");
        checkTokenizeSentence(sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("字.");
        sentVec.push_back("字");
        checkTokenizeSentence(sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("etc.");
        sentVec.push_back("它");
        checkTokenizeSentence(sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("etc.。");
        checkTokenizeSentence(sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("字。");
        sentVec.push_back("它");
        checkTokenizeSentence(sentVec);
    }
}

BOOST_AUTO_TEST_CASE(checkFullWithQuote)
{
    {
        vector<string> sentVec;
        sentVec.push_back("etc.)’ the");
        checkTokenizeSentence(sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("etc.)’ ");
        sentVec.push_back("The");
        checkTokenizeSentence(sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("etc.)’ ‘(the");
        checkTokenizeSentence(sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("etc.)’ ");
        sentVec.push_back("‘(The");
        checkTokenizeSentence(sentVec);
    }
}

BOOST_AUTO_TEST_SUITE_END() 
