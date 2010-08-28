/**
 * @author Wei
 */

#include <iostream>
#include <fstream>

#include <boost/test/unit_test.hpp>
#include <boost/thread/thread.hpp>

#include <ir/id_manager/IDManager.h>
#include <util/ustring/UString.h>

//#include <la/analyzer/KoreanAnalyzer.h>
#include <ilplib.hpp>

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

        #define check(term, text, pos, offset, andOrBit, level ) \
        { \
            BOOST_CHECK_EQUAL( term.text_, UString(text, UString::UTF_8) ); \
            BOOST_CHECK_EQUAL( term.pos_, pos); \
            BOOST_CHECK_EQUAL( term.wordOffset_, offset); \
            BOOST_CHECK_EQUAL( term.getAndOrBit(), andOrBit); \
            BOOST_CHECK_EQUAL( term.getLevel(), level); \
        } \

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
    check(termList[0], "멀티터치스크린", Term::KoreanEojulPOS, 0U, Term::OR, 0U);
    check(termList[1], "멀티", "NFG", 0U, Term::AND, 1U);
    check(termList[2], "터치", "NFG", 0U, Term::AND, 1U);
    check(termList[3], "스크린", "NFG", 0U, Term::AND, 1U);
    check(termList[4], "기술인", Term::KoreanEojulPOS, 1U, Term::OR, 0U);
    check(termList[5], "기술", "NNG", 1U, Term::AND, 1U);

    regularTests();
}

BOOST_AUTO_TEST_CASE(test_noprime)
{
    analyzer.setAnalyzePrime(false);

    const string sstr("멀티터치스크린 기술인");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 4U);
    check(termList[0], "멀티", "NFG", 0U, Term::AND, 0U);
    check(termList[1], "터치", "NFG", 0U, Term::AND, 0U);
    check(termList[2], "스크린", "NFG", 0U, Term::AND, 0U);
    check(termList[3], "기술", "NNG", 1U, Term::AND, 0U);

    regularTests();
}

BOOST_AUTO_TEST_CASE(test_basic)
{
    analyzer.setAnalyzePrime(false);

    const string sstr("등이");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);


    BOOST_CHECK_EQUAL(termList.size(), 1U);
    check(termList[0], "등", "NNB", 0U, Term::AND, 0U);

    regularTests();
}


BOOST_AUTO_TEST_CASE(test_nbest)
{
    analyzer.setAnalyzePrime(false);

    const string sstr("멀티터치스크린 기술인");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 4U);
    check(termList[0], "멀티", "NFG", 0U, Term::AND, 0U);
    check(termList[1], "터치", "NFG", 0U, Term::AND, 0U);
    check(termList[2], "스크린", "NFG", 0U, Term::AND, 0U);
    check(termList[3], "기술", "NNG", 1U, Term::AND, 0U);

    regularTests();
}

BOOST_AUTO_TEST_CASE(test_english)
{
    const string sstr("윈도7 Windows7");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 6U);
    check(termList[0], "윈도7", Term::KoreanEojulPOS, 0U, Term::OR, 0U);
    check(termList[1], "윈도", "NFG", 0U, Term::AND, 1U);
    check(termList[2], "7", "SN", 0U, Term::AND, 1U);
    check(termList[3], "windows7", Term::KoreanEojulPOS, 1U, Term::OR, 0U);
    check(termList[4], "windows", Term::EnglishPOS, 1U, Term::AND, 1U);
    check(termList[5], "7", "SN", 1U, Term::AND, 1U);

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
    check(termList[0], "윈도7", Term::KoreanEojulPOS, 0U, Term::OR, 0U);
    check(termList[1], "윈도", "NFG", 0U, Term::AND, 1U);
    check(termList[2], "7", Term::DigitPOS , 0U, Term::AND, 1U);
    check(termList[3], "Windows7", Term::KoreanEojulPOS, 1U, Term::OR, 0U);
    check(termList[4], "Windows", Term::EnglishPOS, 1U, Term::AND, 1U);
    check(termList[5], "7", Term::DigitPOS , 1U, Term::AND, 1U);

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
    check(termList[0], "윈도7", Term::KoreanEojulPOS, 0U, Term::OR, 0U);
    check(termList[1], "윈도", "NFG", 0U, Term::AND, 1U);
    check(termList[2], "7", Term::DigitPOS , 0U, Term::AND, 1U);
    check(termList[3], "Windows7", Term::KoreanEojulPOS, 1U, Term::OR, 0U);
    check(termList[4], "Windows", Term::EnglishPOS, 1U, Term::AND, 1U);
    check(termList[5], "windows", Term::EnglishPOS, 1U, Term::OR, 2U);
    check(termList[6], "7", Term::DigitPOS, 1U, Term::AND, 1U);

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
    check(termList[0], "윈도7", Term::KoreanEojulPOS, 0U, Term::OR, 0U);
    check(termList[1], "윈도", "NFG", 0U, Term::AND, 1U);
    check(termList[2], "7", Term::DigitPOS, 0U, Term::AND, 1U);
    check(termList[3], "windows7", Term::KoreanEojulPOS , 1U, Term::OR, 0U);
    check(termList[4], "windows", Term::EnglishPOS, 1U, Term::AND, 1U);
    check(termList[5], "window", Term::EnglishPOS, 1U, Term::OR, 2U);
    check(termList[6], "7", Term::DigitPOS, 1U, Term::AND, 1U);

    regularTests();
}


BOOST_AUTO_TEST_CASE(test_synonym)
{
//    analyzer.setExtractSynonym(true);
//    analyzer.setSynonymUpdateInterval(100);
//
////    kmaKnowledgeDir.appendFile("synonym.txt", "윈도 windows\n");
////    boost::this_thread::sleep( boost::posix_time::seconds(2) ); // wait for updating synonym dict
//
//    const string sstr("윈도7 Windows7");
//    const UString ustr(sstr, UString::UTF_8);
//    analyzer.analyze(Term(ustr), termList);
//    analyzer.analyze(&idm, Term(ustr), termIdList);
//
//    print();
//
//    BOOST_CHECK_EQUAL(termList.size(), 7U);
//    check(termList[0], "윈도7", 0U, Term::OR, 0U);
//    check(termList[1], "윈도", 0U, Term::AND, 1U);
//    check(termList[2], "7", 0U, Term::AND, 1U);
//    check(termList[3], "Windows7", 1U, Term::OR, 0U);
//    check(termList[4], "windows", 1U, Term::AND, 1U);
//    check(termList[5], "윈도", 1U, Term::AND, 1U);
//    check(termList[6], "7", 1U, Term::AND, 1U);
//
//    regularTests();
}

BOOST_AUTO_TEST_CASE(test_specialchar)
{
    const string sstr("'윈도7', \"Windows7\".");
    const UString ustr(sstr, UString::UTF_8);
    analyzer.analyze(Term(ustr), termList);
    analyzer.analyze(&idm, Term(ustr), termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 12U);
    check(termList[0], "<PH>", Term::SpecialCharPOS, 0U, Term::AND, 0U);
    check(termList[1], "윈도7", Term::KoreanEojulPOS, 1U, Term::OR, 0U);
    check(termList[2], "윈도", "NFG", 1U, Term::AND, 1U);
    check(termList[3], "7", Term::DigitPOS, 1U, Term::AND, 1U);
    check(termList[4], "<PH>", Term::SpecialCharPOS, 2U, Term::AND, 0U);
    check(termList[5], "<PH>", Term::SpecialCharPOS, 3U, Term::AND, 0U);
    check(termList[6], "<PH>", Term::SpecialCharPOS, 4U, Term::AND, 0U);
    check(termList[7], "windows7", Term::KoreanEojulPOS, 5U, Term::OR, 0U);
    check(termList[8], "windows", Term::EnglishPOS, 5U, Term::AND, 1U);
    check(termList[9], "7", Term::DigitPOS, 5U, Term::AND, 1U);
    check(termList[10], "<PH>", Term::SpecialCharPOS, 6U, Term::AND, 0U);
    check(termList[11], "<PH>", Term::SpecialCharPOS, 7U, Term::AND, 0U);

    regularTests();
}

BOOST_AUTO_TEST_SUITE_END()
