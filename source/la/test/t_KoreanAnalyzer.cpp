#include <iostream>
#include <fstream>

#include <boost/test/unit_test.hpp>
#include <boost/thread/thread.hpp>

#include <ir/id_manager/IDManager.h>
#include <util/ustring/UString.h>

#include <la/analyzer/KoreanAnalyzer.h>

#include "test_def.h"

using namespace la;
using namespace std;
using namespace izenelib::util;
using namespace izenelib::ir::idmanager;

static KnowledgeDir kmaKnowledgeDir(KMA_KNOWLEDGE, "kma_knowledge");

class KoreanAnalyzerFixture {
public:
        KoreanAnalyzerFixture() :
            analyzer(kmaKnowledgeDir.getDir()),
            idm("KoreanAnalyzerTest")
        {
            analyzer.setLabelMode();
        }

        ~KoreanAnalyzerFixture()
        {
          idm.close();
          directory_iterator end_itr; // default construction yields past-the-end
          for ( directory_iterator itr( "." ); itr != end_itr; ++itr )
          {
              if(!is_directory(itr->status()) && itr->leaf().substr(0,
                    sizeof("KoreanAnalyzerTest_")-1) == "KoreanAnalyzerTest_") {
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
                cout << printable << "," << it->wordOffset_ << endl;
            }
        }

        KoreanAnalyzer analyzer;
        IDManager idm;
        TermList termList;
        TermIdList termIdList;
};

BOOST_FIXTURE_TEST_SUITE( Korean_Analyzer, KoreanAnalyzerFixture )

//"타이코 일로터치가 초음파 멀티터치스크린 기술인 '인텔리터치 플러스'를 개발하고 '윈도7 멀티터치' 재료 인증을 획득했다고 11일 마감했다."

BOOST_AUTO_TEST_CASE(test_normal)
{
    const string sstr("멀티터치스크린 기술인");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 6U);
    BOOST_CHECK_EQUAL( termList[0].text_, UString("멀티터치스크린", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[0].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[1].text_, UString("멀티", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[2].text_, UString("터치", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[3].text_, UString("스크린", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[4].text_, UString("기술인", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[5].text_, UString("기술", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[5].wordOffset_, 1U);

    regularTests();
}

BOOST_AUTO_TEST_CASE(test_english)
{
    const string sstr("윈도7 Windows7");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 6U);
    BOOST_CHECK_EQUAL( termList[0].text_, UString("윈도7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[0].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[1].text_, UString("윈도", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[2].text_, UString("7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[3].text_, UString("Windows7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[4].text_, UString("windows", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[5].text_, UString("7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[5].wordOffset_, 1U);

    regularTests();
}

BOOST_AUTO_TEST_CASE(test_casesensitive)
{
    analyzer.setCaseSensitive(true, false);

    const string sstr("윈도7 Windows7");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 6U);
    BOOST_CHECK_EQUAL( termList[0].text_, UString("윈도7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[0].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[1].text_, UString("윈도", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[2].text_, UString("7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[3].text_, UString("Windows7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[4].text_, UString("Windows", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[5].text_, UString("7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[5].wordOffset_, 1U);

    regularTests();
}


BOOST_AUTO_TEST_CASE(test_casesensitive_with_lower)
{
    analyzer.setCaseSensitive(true, true);

    const string sstr("윈도7 Windows7");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 7U);
    BOOST_CHECK_EQUAL( termList[0].text_, UString("윈도7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[0].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[1].text_, UString("윈도", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[2].text_, UString("7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[3].text_, UString("Windows7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[4].text_, UString("Windows", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[5].text_, UString("windows", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[5].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[6].text_, UString("7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[6].wordOffset_, 1U);

    regularTests();
}


BOOST_AUTO_TEST_CASE(test_stemming)
{
    analyzer.setExtractEngStem(true);


    const string sstr("윈도7 Windows7");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 7U);
    BOOST_CHECK_EQUAL( termList[0].text_, UString("윈도7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[0].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[1].text_, UString("윈도", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[2].text_, UString("7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[3].text_, UString("Windows7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[4].text_, UString("windows", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[5].text_, UString("window", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[5].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[6].text_, UString("7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[6].wordOffset_, 1U);

    regularTests();
}


BOOST_AUTO_TEST_CASE(test_synonym)
{
    analyzer.setExtractSynonym(true);
    analyzer.setSynonymUpdateInterval(1);

    kmaKnowledgeDir.appendFile("synonym.txt", "윈도 windows\n");
    boost::this_thread::sleep( boost::posix_time::seconds(2) ); // wait for updating synonym dict

    const string sstr("윈도7 Windows7");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    print();

    BOOST_CHECK_EQUAL(termList.size(), 7U);
    BOOST_CHECK_EQUAL( termList[0].text_, UString("윈도7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[0].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[1].text_, UString("윈도", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[2].text_, UString("7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[3].text_, UString("Windows7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[4].text_, UString("windows", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[5].text_, UString("윈도", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[5].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[6].text_, UString("7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[6].wordOffset_, 1U);


    regularTests();
}

BOOST_AUTO_TEST_CASE(test_specialchar)
{
    const string sstr("'윈도7', \"Windows7\".");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 12U);
    BOOST_CHECK_EQUAL( termList[0].text_, PLACE_HOLDER );
    BOOST_CHECK_EQUAL( termList[0].wordOffset_, 0U);
    BOOST_CHECK_EQUAL( termList[1].text_, UString("윈도7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[2].text_, UString("윈도", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[3].text_, UString("7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].wordOffset_, 1U);
    BOOST_CHECK_EQUAL( termList[4].text_, PLACE_HOLDER );
    BOOST_CHECK_EQUAL( termList[4].wordOffset_, 2U);
    BOOST_CHECK_EQUAL( termList[5].text_, PLACE_HOLDER );
    BOOST_CHECK_EQUAL( termList[5].wordOffset_, 3U);
    BOOST_CHECK_EQUAL( termList[6].text_, PLACE_HOLDER );
    BOOST_CHECK_EQUAL( termList[6].wordOffset_, 4U);
    BOOST_CHECK_EQUAL( termList[7].text_, UString("Windows7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[7].wordOffset_, 5U);
    BOOST_CHECK_EQUAL( termList[8].text_, UString("windows", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[8].wordOffset_, 5U);
    BOOST_CHECK_EQUAL( termList[9].text_, UString("7", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[9].wordOffset_, 5U);
    BOOST_CHECK_EQUAL( termList[10].text_, PLACE_HOLDER );
    BOOST_CHECK_EQUAL( termList[10].wordOffset_, 6U);
    BOOST_CHECK_EQUAL( termList[11].text_, PLACE_HOLDER );
    BOOST_CHECK_EQUAL( termList[11].wordOffset_, 7U);

    regularTests();
}

BOOST_AUTO_TEST_SUITE_END()
