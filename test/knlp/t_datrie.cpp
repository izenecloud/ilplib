/**
 * @file t_knlp_tokenize.cpp
 * @brief test ilplib::knlp::Tokenize in forward maximize match.
 */

#include "knlp/datrie.h"

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <ostream>

using namespace ilplib;
using namespace ilplib::knlp;

BOOST_AUTO_TEST_SUITE(DATrieTest)

void gen_dict(const char* nm)
{
    std::ofstream of(nm);
    of << "A\t0.1\n";
    of << "[MIN]\t0.01\n";
    of << "\t0.001\n";
    of << "   [MIN]     \t0.01\n";
    of << "C\t0.2\n";
    of << "ABC C\t0.4\n";
    of << " ABC C D\t1.6\n";
    of << " ABC \t2.6\n";
    of << "B\t3.6\n";
    of << "ABCD\t2\n";
    of << "E\t1\n";
}


BOOST_AUTO_TEST_CASE(testDATrie)
{
    gen_dict("./tmp.dict");
    DATrie da("./tmp.dict");

    BOOST_CHECK(da.size() == 9);
    BOOST_CHECK(da.score(KString("abc c d")) == 1.6);
    BOOST_CHECK(da.score(KString("abc")) == 2.6);
    BOOST_CHECK(da.score(KString("b")) == 3.6);
    BOOST_CHECK(da.score(KString("a")) == 0.1);
    BOOST_CHECK(da.score(KString("h")) == 0.01);
    BOOST_CHECK(da.score(KString("[min]")) == 0.01);
    BOOST_CHECK(da.min() == 0.01);

    std::vector<std::pair<KString, double> > v = da.token(KString("a abc dDdds abc c dc "));
    for(size_t i = 0; i < v.size(); ++i)
        cout<<i<<' '<<v[i].first<<' '<<v[i].second<<endl;
//    cout<<da.dict_[da.find_word(KString("d"))].kstr<<' '<<da.score(KString("d"))<<endl;
    BOOST_CHECK(v.size() == 13);
    BOOST_CHECK(v[0].first == KString("a") && v[0].second == 0.1);
    BOOST_CHECK(v[1].first == KString(" ") && v[1].second == 0.01);
    BOOST_CHECK(v[2].first == KString("abc") && v[2].second == 2.6);
    BOOST_CHECK(v[3].first == KString(" ") && v[3].second == 0.01);
    BOOST_CHECK(v[4].first == KString("d") && v[4].second == 0.01);
    BOOST_CHECK(v[5].first == KString("D") && v[5].second == 0.01);
    BOOST_CHECK(v[6].first == KString("d") && v[6].second == 0.01);
    BOOST_CHECK(v[7].first == KString("d") && v[7].second == 0.01);
    BOOST_CHECK(v[8].first == KString("s") && v[8].second == 0.01); 
    BOOST_CHECK(v[9].first == KString(" ") && v[9].second == 0.01);
    BOOST_CHECK(v[10].first == KString("abc c d") && v[10].second == 1.6);
    BOOST_CHECK(v[11].first == KString("c") && v[11].second == 0.2);
    BOOST_CHECK(v[12].first == KString(" ") && v[12].second == 0.01);

    v = da.sub_token(KString("ac"));
    BOOST_CHECK(v.size() == 2);
    BOOST_CHECK(v[0].first == KString("a") && v[0].second == 0.1);
    BOOST_CHECK(v[1].first == KString("c") && v[1].second == 0.2);

    v = da.sub_token(KString("abc"));
    BOOST_CHECK(v.size() == 1);
    BOOST_CHECK(v[0].first == KString("abc") && v[0].second == 2.6);
//    BOOST_CHECK(v[1].first == KString("b") && v[1].second == 3.6);
//    BOOST_CHECK(v[2].first == KString("c") && v[2].second == 0.2);

    v = da.sub_token(KString("abcd"));
    BOOST_CHECK(v.size() == 1);
    BOOST_CHECK(v[0].first == KString("abcd") && v[0].second == 2);

    v = da.sub_token(KString("abce"));
    BOOST_CHECK(v.size() == 2);
    BOOST_CHECK(v[0].first == KString("abc") && v[0].second == 2.6);
    BOOST_CHECK(v[1].first == KString("e") && v[1].second == 1);



    v = da.token(KString(""));
    BOOST_CHECK(v.size() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
