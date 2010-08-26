/**
 * @author Wei
 */

#include <boost/test/unit_test.hpp>
#include <util/ustring/UString.h>

#include <la/analyzer/TokenAnalyzer.h>

#include "test_def.h"

using namespace la;
using namespace std;
using namespace izenelib::util;

BOOST_AUTO_TEST_SUITE( TokenAnalyzerTest )

BOOST_AUTO_TEST_CASE(test_normal)
{
    TokenAnalyzer analyzer;
    TermList termList;
    analyzer.analyze(UString("互联网 中国 2010。", UString::UTF_8), termList);

    BOOST_CHECK_EQUAL(termList.size(), 4U);
    BOOST_CHECK_EQUAL( termList[0].text_, UString("互联网", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[1].text_, UString("中国", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[2].text_, UString("2010", UString::UTF_8) );
    BOOST_CHECK_EQUAL( termList[3].text_, PLACE_HOLDER );
}

BOOST_AUTO_TEST_SUITE_END()
