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
    BOOST_CHECK_EQUAL( termList[0].text_, UString("互", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[0].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[1].text_, UString("互联", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[2].text_, UString("联", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[3].text_, UString("联网", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[4].text_, UString("网", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].wordOffset_, 2U);

    regularTests();
}


BOOST_AUTO_TEST_CASE(test_english)
{
    const string sstr("测试使用Boost Unit Tests");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 5U);
    BOOST_CHECK_EQUAL( termList[0].text_, UString("测试", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].text_, UString("使用", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].text_, UString("boost", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].text_, UString("unit", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].text_, UString("tests", UString::UTF_8) );

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
    BOOST_CHECK_EQUAL( termList[0].text_, UString("测试", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].text_, UString("使用", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].text_, UString("Boost", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].text_, UString("Unit", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].text_, UString("Tests", UString::UTF_8) );

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
    BOOST_CHECK_EQUAL( termList[0].text_, UString("测试", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].text_, UString("使用", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].text_, UString("Boost", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].text_, UString("boost", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].text_, UString("Unit", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[5].text_, UString("unit", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[6].text_, UString("Tests", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[7].text_, UString("tests", UString::UTF_8) );

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
    BOOST_CHECK_EQUAL( termList[0].text_, UString("测试", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].text_, UString("使用", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].text_, UString("boost", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].text_, UString("unit", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].text_, UString("tests", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[5].text_, UString("test", UString::UTF_8) );

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
    BOOST_CHECK_EQUAL( termList[0].text_, UString("测试", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].text_, UString("使用", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].text_, UString("boost", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].text_, UString("unit", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].text_, UString("单元", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[5].text_, UString("tests", UString::UTF_8) );

    regularTests();
}

BOOST_AUTO_TEST_CASE(test_specialchar)
{
    const string sstr("测试，使用《Boost Unit Tests》。");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 9U);
    BOOST_CHECK_EQUAL( termList[0].text_, UString("测试", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].text_, la::PLACE_HOLDER );
    BOOST_CHECK_EQUAL( termList[2].text_, UString("使用", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].text_, la::PLACE_HOLDER );
    BOOST_CHECK_EQUAL( termList[4].text_, UString("boost", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[5].text_, UString("unit", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[6].text_, UString("tests", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[7].text_, la::PLACE_HOLDER );
    BOOST_CHECK_EQUAL( termList[8].text_, la::PLACE_HOLDER );

    regularTests();
}

BOOST_AUTO_TEST_SUITE_END()
