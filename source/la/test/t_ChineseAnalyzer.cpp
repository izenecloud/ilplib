/**
 * @author Wei
 */

#include <iostream>
#include <fstream>

#include <boost/test/unit_test.hpp>
#include <boost/thread/thread.hpp>

#include <ir/id_manager/IDManager.h>
#include <util/ustring/UString.h>

#include <la/analyzer/ChineseAnalyzer.h>

#include "test_def.h"

using namespace la;
using namespace std;
using namespace izenelib::util;
using namespace izenelib::ir::idmanager;

static KnowledgeDir cmaKnowledgeDir(CMA_KNOWLEDGE, path("cma_knowledge")/"utf8");

class ChineseAnalyzerFixture {
public:
        ChineseAnalyzerFixture() :
            analyzer(cmaKnowledgeDir.getDir()),
            idm("ChineseAnalyzerTest")
        {
            analyzer.setLabelMode();
            analyzer.setAnalysisType(ChineseAnalyzer::minimum_match);
        }

        ~ChineseAnalyzerFixture()
        {
          idm.close();
          directory_iterator end_itr; // default construction yields past-the-end
          for ( directory_iterator itr( "." ); itr != end_itr; ++itr )
          {
              if(!is_directory(itr->status()) && itr->leaf().substr(0,
                    sizeof("ChineseAnalyzerTest_")-1) == "ChineseAnalyzerTest_") {
                    remove(itr->path());
                }
          }
        }

        inline void check(const Term & term, const string & text, const size_t offset, const unsigned andOrBit, unsigned int level )
        {
            BOOST_CHECK_EQUAL( term.text_, UString(text, UString::UTF_8) );
            BOOST_CHECK_EQUAL( term.wordOffset_, offset);
            BOOST_CHECK_EQUAL( term.getAndOrBit(), andOrBit);
            BOOST_CHECK_EQUAL( term.getLevel(), level);
        }

        void regularTests() {
            BOOST_CHECK_EQUAL(termList.size(), termIdList.size());

            TermList::iterator it = termList.begin();
            TermIdList::iterator jt = termIdList.begin();
            for(; it!=termList.end() && jt!=termIdList.end(); it++, jt++ ) {
                unsigned int termid;
                idm.getTermIdByTermString(it->text_, termid);
                BOOST_CHECK_EQUAL(termid, jt->termid_);
                BOOST_CHECK_EQUAL(it->wordOffset_, jt->wordOffset_);
            }
        }

        void print() {
            TermList::iterator it = termList.begin();
            for(; it!=termList.end(); it++ ) {
                std::string printable;
                it->text_.convertString(printable, UString::UTF_8);
                cout << printable << endl;
            }
        }

        ChineseAnalyzer analyzer;
        IDManager idm;
        TermList termList;
        TermIdList termIdList;
};

BOOST_FIXTURE_TEST_SUITE( Chinese_Analyzer, ChineseAnalyzerFixture )

BOOST_AUTO_TEST_CASE(test_normal)
{
    const string sstr("互联网");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 5U);
    check(termList[0], "互", 0U, Term::AND, 0U);
    check(termList[1], "互联", 1U, Term::AND, 0U);
    check(termList[2], "联", 2U, Term::AND, 0U);
    check(termList[3], "联网", 3U, Term::AND, 0U);
    check(termList[4], "网", 4U, Term::AND, 0U);
    regularTests();
}


BOOST_AUTO_TEST_CASE(test_english)
{
    const string sstr("测试使用Boost Unit Tests");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 5U);
    check(termList[0], "测试", 0U, Term::AND, 0U);
    check(termList[1], "使用", 1U, Term::AND, 0U);
    check(termList[2], "boost", 2U, Term::AND, 0U);
    check(termList[3], "unit", 3U, Term::AND, 0U);
    check(termList[4], "tests", 4U, Term::AND, 0U);
    regularTests();
}


BOOST_AUTO_TEST_CASE(test_casesensitive)
{
    analyzer.setCaseSensitive(true, false);

    const string sstr("测试使用Boost Unit Tests");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 5U);
    check(termList[0], "测试", 0U, Term::AND, 0U);
    check(termList[1], "使用", 1U, Term::AND, 0U);
    check(termList[2], "Boost", 2U, Term::AND, 0U);
    check(termList[3], "Unit", 3U, Term::AND, 0U);
    check(termList[4], "Tests", 4U, Term::AND, 0U);

    regularTests();
}


BOOST_AUTO_TEST_CASE(test_casesensitive_with_lower)
{
    analyzer.setCaseSensitive(true, true);

    const string sstr("测试使用Boost Unit Tests");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 8U);
    check(termList[0], "测试", 0U, Term::AND, 0U);
    check(termList[1], "使用", 1U, Term::AND, 0U);
    check(termList[2], "Boost", 2U, Term::AND, 0U);
    check(termList[3], "boost", 2U, Term::OR, 1U);
    check(termList[4], "Unit", 3U, Term::AND, 0U);
    check(termList[5], "unit", 3U, Term::OR, 1U);
    check(termList[6], "Tests", 4U, Term::AND, 0U);
    check(termList[7], "tests", 4U, Term::OR, 1U);
    regularTests();
}


BOOST_AUTO_TEST_CASE(test_stemming)
{
    analyzer.setExtractEngStem(true);

    const string sstr("测试使用Boost Unit Tests");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 6U);
    check(termList[0], "测试", 0U, Term::AND, 0U);
    check(termList[1], "使用", 1U, Term::AND, 0U);
    check(termList[2], "boost", 2U, Term::AND, 0U);
    check(termList[3], "unit", 3U, Term::AND, 0U);
    check(termList[4], "tests", 4U, Term::AND, 0U);
    check(termList[5], "test", 4U, Term::OR, 1U);
    regularTests();
}


BOOST_AUTO_TEST_CASE(test_synonym)
{
    analyzer.setExtractSynonym(true);
    analyzer.setSynonymUpdateInterval(1);

    cmaKnowledgeDir.appendFile("synonym.txt", "单元 unit\n");
    boost::this_thread::sleep( boost::posix_time::seconds(2) ); // wait for updating synonym dict

    const string sstr("测试使用Boost Unit Tests");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 6U);
    check(termList[0], "测试", 0U, Term::AND, 0U);
    check(termList[1], "使用", 1U, Term::AND, 0U);
    check(termList[2], "boost", 2U, Term::AND, 0U);
    check(termList[3], "unit", 3U, Term::AND, 0U);
    check(termList[4], "单元", 3U, Term::OR, 1U);
    check(termList[5], "tests", 4U, Term::AND, 0U);

    regularTests();
}

BOOST_AUTO_TEST_CASE(test_specialchar)
{
    const string sstr("测试，使用《Boost Unit Tests》。");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 9U);
    check(termList[0], "测试", 0U, Term::AND, 0U);
    check(termList[1], "<PH>", 1U, Term::AND, 0U);
    check(termList[2], "使用", 2U, Term::AND, 0U);
    check(termList[3], "<PH>", 3U, Term::AND, 0U);
    check(termList[4], "boost", 4U, Term::AND, 0U);
    check(termList[5], "unit", 5U, Term::AND, 0U);
    check(termList[6], "tests", 6U, Term::AND, 0U);
    check(termList[7], "<PH>", 7U, Term::AND, 0U);
    check(termList[8], "<PH>", 8U, Term::AND, 0U);
    regularTests();
}

//BOOST_AUTO_TEST_CASE(test_stopwords)
//{
//    const string sstr("双方合作后，将保持独立的搜索和广告业务");
//    const UString ustr(sstr, UString::UTF_8);
//    analyzer.analyze(Term(ustr), termList);
//    analyzer.analyze(&idm, Term(ustr), termIdList);
//
//    print();
//
//    regularTests();
//}

BOOST_AUTO_TEST_SUITE_END()
