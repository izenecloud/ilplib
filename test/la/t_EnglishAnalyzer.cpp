/**
 * @author Wei
 */

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

//#include <la/analyzer/EnglishAnalyzer.h>
#include <ilplib.hpp>

using namespace la;
using namespace std;
using namespace izenelib::util;
using namespace izenelib::ir::idmanager;
using namespace boost::filesystem;

class EnglishAnalyzerFixture {
public:
        EnglishAnalyzerFixture() :
            idm("EnglishAnalyzerTest")
        {
        }

        ~EnglishAnalyzerFixture()
        {
          idm.close();
          directory_iterator end_itr; // default construction yields past-the-end
          for ( directory_iterator itr( "." ); itr != end_itr; ++itr )
          {
              if(!is_directory(itr->status()) && itr->leaf().substr(0,
                    sizeof("EnglishAnalyzerTest")-1) == "EnglishAnalyzerTest") {
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

        EnglishAnalyzer analyzer;
        IDManager idm;
        TermList termList;
        TermIdList termIdList;
};


BOOST_FIXTURE_TEST_SUITE( EnglishAnalyzerTest, EnglishAnalyzerFixture )

BOOST_AUTO_TEST_CASE(test_default)
{
    UString input("Boost unit tests.", UString::UTF_8);
    analyzer.analyze(input, termList);
    analyzer.analyze(&idm, input, termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 4U);
    BOOST_CHECK_EQUAL(termList[0].text_, UString("boost", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[0].wordOffset_, 0U);
    BOOST_CHECK_EQUAL(termList[1].text_, UString("unit", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[1].wordOffset_, 1U);
    BOOST_CHECK_EQUAL(termList[2].text_, UString("tests", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[2].wordOffset_, 2U);
    BOOST_CHECK_EQUAL(termList[3].text_, PLACE_HOLDER);
    BOOST_CHECK_EQUAL(termList[3].wordOffset_, 3U);

    regularTests();
}


BOOST_AUTO_TEST_CASE(test_casesensitive)
{
    analyzer.setCaseSensitive(true, false);

    UString input("Boost unit tests.", UString::UTF_8);
    analyzer.analyze(input, termList);
    analyzer.analyze(&idm, input, termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 4U);
    BOOST_CHECK_EQUAL(termList[0].text_, UString("Boost", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[0].wordOffset_, 0U);
    BOOST_CHECK_EQUAL(termList[1].text_, UString("unit", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[1].wordOffset_, 1U);
    BOOST_CHECK_EQUAL(termList[2].text_, UString("tests", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[2].wordOffset_, 2U);
    BOOST_CHECK_EQUAL(termList[3].text_, PLACE_HOLDER);
    BOOST_CHECK_EQUAL(termList[3].wordOffset_, 3U);

    regularTests();
}


BOOST_AUTO_TEST_CASE(test_casesensitive_with_lower)
{
    analyzer.setCaseSensitive(true, true);

    UString input("Boost unit tests.", UString::UTF_8);
    analyzer.analyze(input, termList);
    analyzer.analyze(&idm, input, termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 5U);
    BOOST_CHECK_EQUAL(termList[0].text_, UString("Boost", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[0].wordOffset_, 0U);
    BOOST_CHECK_EQUAL(termList[1].text_, UString("boost", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[1].wordOffset_, 0U);
    BOOST_CHECK_EQUAL(termList[2].text_, UString("unit", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[2].wordOffset_, 1U);
    BOOST_CHECK_EQUAL(termList[3].text_, UString("tests", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[3].wordOffset_, 2U);
    BOOST_CHECK_EQUAL(termList[4].text_, PLACE_HOLDER);
    BOOST_CHECK_EQUAL(termList[4].wordOffset_, 3U);

    regularTests();
}


BOOST_AUTO_TEST_CASE(test_stemming)
{
    analyzer.setExtractEngStem(true);

    UString input("Boost unit tests.", UString::UTF_8);
    analyzer.analyze(input, termList);
    analyzer.analyze(&idm, input, termIdList);

    BOOST_CHECK_EQUAL(termList.size(), 5U);
    BOOST_CHECK_EQUAL(termList[0].text_, UString("boost", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[0].wordOffset_, 0U);
    BOOST_CHECK_EQUAL(termList[1].text_, UString("unit", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[1].wordOffset_, 1U);
    BOOST_CHECK_EQUAL(termList[2].text_, UString("tests", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[2].wordOffset_, 2U);
    BOOST_CHECK_EQUAL(termList[3].text_, UString("test", UString::UTF_8));
    BOOST_CHECK_EQUAL(termList[3].wordOffset_, 2U);
    BOOST_CHECK_EQUAL(termList[4].text_, PLACE_HOLDER);
    BOOST_CHECK_EQUAL(termList[4].wordOffset_, 3U);

    regularTests();
}


BOOST_AUTO_TEST_SUITE_END()
