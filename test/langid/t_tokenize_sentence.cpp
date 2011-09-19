///
/// @file t_tokenize_sentence.cpp
/// @brief test tokenizing sentence
/// @author Jun Jiang
/// @date Created 2011-09-19
///

#include "LangidTestFixture.h"

#include <boost/test/unit_test.hpp>
#include <string>

using namespace std;
using namespace boost;
using namespace ilplib::langid;

namespace
{

void checkTokenize(Analyzer* analyzer, const vector<string>& sentVec)
{
    string combine;
    for (vector<string>::const_iterator it = sentVec.begin();
        it != sentVec.end(); ++it)
    {
        combine += *it;
    }

    int i = 0;
    const char* p = combine.c_str();
    string sent;
    while(int len = analyzer->sentenceLength(p))
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

}

BOOST_FIXTURE_TEST_SUITE(LangidTest, LangidTestFixture)

BOOST_AUTO_TEST_CASE(checkNormal)
{
    Analyzer* analyzer = getAnalyzer();

    vector<string> sentVec;
    sentVec.push_back("Hello, how are you?");
    sentVec.push_back("我很好，你好吗。");
    sentVec.push_back("中國,妳們好！");
    sentVec.push_back("こんにちは、宜しく。");
    sentVec.push_back("환영,안녕하세요.");
    checkTokenize(analyzer, sentVec);
}

BOOST_AUTO_TEST_CASE(checkNewLine)
{
    Analyzer* analyzer = getAnalyzer();

    vector<string> sentVec;
    sentVec.push_back("Hi!\n");
    sentVec.push_back("How are you\n");
    sentVec.push_back("\n");
    sentVec.push_back("你好吗？\n");
    checkTokenize(analyzer, sentVec);
}

BOOST_AUTO_TEST_CASE(checkParenthesis)
{
    Analyzer* analyzer = getAnalyzer();

    {
        vector<string> sentVec;
        sentVec.push_back("(\"Go.\") ");
        sentVec.push_back("(He did.)");
        checkTokenize(analyzer, sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("(\"Go?\")\n");
        sentVec.push_back("(He did.)");
        checkTokenize(analyzer, sentVec);
    }
}

BOOST_AUTO_TEST_CASE(checkFullStop)
{
    Analyzer* analyzer = getAnalyzer();

    {
        vector<string> sentVec;
        sentVec.push_back("U.S.A. is");
        checkTokenize(analyzer, sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("U.S.A? ");
        sentVec.push_back("He");
        checkTokenize(analyzer, sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("3.4");
        checkTokenize(analyzer, sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("c.d");
        checkTokenize(analyzer, sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("the resp. leaders are");
        checkTokenize(analyzer, sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("字.");
        sentVec.push_back("字");
        checkTokenize(analyzer, sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("etc.");
        sentVec.push_back("它");
        checkTokenize(analyzer, sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("etc.。");
        checkTokenize(analyzer, sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("字。");
        sentVec.push_back("它");
        checkTokenize(analyzer, sentVec);
    }
}

BOOST_AUTO_TEST_CASE(checkFullWithQuote)
{
    Analyzer* analyzer = getAnalyzer();

    {
        vector<string> sentVec;
        sentVec.push_back("etc.)’ the");
        checkTokenize(analyzer, sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("etc.)’ ");
        sentVec.push_back("The");
        checkTokenize(analyzer, sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("etc.)’ ‘(the");
        checkTokenize(analyzer, sentVec);
    }

    {
        vector<string> sentVec;
        sentVec.push_back("etc.)’ ");
        sentVec.push_back("‘(The");
        checkTokenize(analyzer, sentVec);
    }
}

BOOST_AUTO_TEST_SUITE_END() 
