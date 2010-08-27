/**
 * @author Wei
 */

#include <boost/test/unit_test.hpp>
#include <util/ustring/UString.h>

//#include <la/analyzer/CharAnalyzer.h>
#include <ilplib.hpp>

#include "test_def.h"

using namespace la;
using namespace std;
using namespace izenelib::util;

BOOST_AUTO_TEST_SUITE( AnalyzerTest )


BOOST_AUTO_TEST_CASE(test_NotExtractSpecialChar)
{
    CharAnalyzer analyzer;
    analyzer.setExtractSpecialChar(false);
    TermList termList;
    analyzer.analyze(UString("互联网(the Internet)中国 2010。", UString::UTF_8), termList);

    BOOST_CHECK_EQUAL(termList.size(), 8U);
    BOOST_CHECK_EQUAL( termList[0].text_, UString("互", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].text_, UString("联", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].text_, UString("网", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].text_, UString("the", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].text_, UString("Internet", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[5].text_, UString("中", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[6].text_, UString("国", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[7].text_, UString("2010", UString::UTF_8) );
}

BOOST_AUTO_TEST_CASE(test_NotConvertToPlaceholder)
{
    CharAnalyzer analyzer;
    analyzer.setExtractSpecialChar(true, false);
    TermList termList;
    analyzer.analyze(UString("互联网(the Internet)中国 2010。", UString::UTF_8), termList);


    BOOST_CHECK_EQUAL(termList.size(), 11U);
    BOOST_CHECK_EQUAL( termList[0].text_, UString("互", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].text_, UString("联", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].text_, UString("网", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].text_, UString("(", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[4].text_, UString("the", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[5].text_, UString("Internet", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[6].text_, UString(")", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[7].text_, UString("中", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[8].text_, UString("国", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[9].text_, UString("2010", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[10].text_, UString("。", UString::UTF_8) );
}

BOOST_AUTO_TEST_SUITE_END()
