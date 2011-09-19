/**
 * @author Wei
 */

#include <boost/test/unit_test.hpp>

//#include <la/tokenizer/Tokenizer.h>
#include <ilplib.hpp>


using namespace la;
using namespace std;
using namespace izenelib::util;

static void print(Tokenizer& tokenizer)
{
    while(tokenizer.nextToken()) {
        cout << "next is true" << endl;
        UString ustr(tokenizer.getToken(), tokenizer.getLength());
        string sstr;
        ustr.convertString(sstr, UString::UTF_8);
        cout << sstr << endl;
    }
    cout <<  "next is false" << endl;
}

BOOST_AUTO_TEST_SUITE( TokenizerTest )

BOOST_AUTO_TEST_CASE(test_default)
{
    UString input(" boost\tunit test. ", UString::UTF_8);
    Tokenizer tokenizer;
    tokenizer.tokenize(input);

    BOOST_CHECK(tokenizer.nextToken());
    BOOST_CHECK_EQUAL( UString(tokenizer.getToken(), tokenizer.getLength()), UString("boost", UString::UTF_8) );
    BOOST_CHECK(tokenizer.nextToken());
    BOOST_CHECK_EQUAL( UString(tokenizer.getToken(), tokenizer.getLength()), UString("unit", UString::UTF_8) );
    BOOST_CHECK(tokenizer.nextToken());
    BOOST_CHECK_EQUAL( UString(tokenizer.getToken(), tokenizer.getLength()), UString("test", UString::UTF_8) );
    BOOST_CHECK(tokenizer.nextToken());
    BOOST_CHECK_EQUAL( UString(tokenizer.getToken(), tokenizer.getLength()), UString(".", UString::UTF_8) );
    BOOST_CHECK(!tokenizer.nextToken());
}

BOOST_AUTO_TEST_CASE(test_unite)
{
    UString input("boost unit-test. ", UString::UTF_8);
    Tokenizer tokenizer;
    TokenizeConfig configUnite;
    configUnite.addUnites("-");
    tokenizer.setConfig(configUnite);
    tokenizer.tokenize(input);

    BOOST_CHECK(tokenizer.nextToken());
    BOOST_CHECK_EQUAL( UString(tokenizer.getToken(), tokenizer.getLength()), UString("boost", UString::UTF_8) );
    BOOST_CHECK(tokenizer.nextToken());
    BOOST_CHECK_EQUAL( UString(tokenizer.getToken(), tokenizer.getLength()), UString("unittest", UString::UTF_8) );
    BOOST_CHECK(tokenizer.nextToken());
    BOOST_CHECK_EQUAL( UString(tokenizer.getToken(), tokenizer.getLength()), UString(".", UString::UTF_8) );
    BOOST_CHECK(!tokenizer.nextToken());
}


BOOST_AUTO_TEST_CASE(test_divide)
{
    UString input("我和你", UString::UTF_8);
    Tokenizer tokenizer;
    TokenizeConfig config;
    config.addDivides("和");
    tokenizer.setConfig(config);
    tokenizer.tokenize(input);

    BOOST_CHECK(tokenizer.nextToken());
    BOOST_CHECK_EQUAL( UString(tokenizer.getToken(), tokenizer.getLength()), UString("我", UString::UTF_8) );
    BOOST_CHECK(tokenizer.nextToken());
    BOOST_CHECK_EQUAL( UString(tokenizer.getToken(), tokenizer.getLength()), UString("和", UString::UTF_8) );
    BOOST_CHECK(tokenizer.nextToken());
    BOOST_CHECK_EQUAL( UString(tokenizer.getToken(), tokenizer.getLength()), UString("你", UString::UTF_8) );
    BOOST_CHECK(!tokenizer.nextToken());
}


BOOST_AUTO_TEST_SUITE_END()
