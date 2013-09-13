/**
 * @file t_knlp_tokenize.cpp
 * @brief test ilplib::knlp::Tokenize in forward maximize match.
 */

#include "knlp/cate_classify.h"

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <ostream>

using namespace ilplib;
using namespace ilplib::knlp;

BOOST_AUTO_TEST_SUITE(DATrieTest)

void gen_cate_table(const char* file)
{
    std::ofstream of(file);
    of << "a1\t1\n"; 
    of << "a2\t2\n";
    of << "a 3\t3\n";
    of << "A1\t4\n";
    of << "B 4\t5\n";
    of << "a1>A1\t6\n";
    of << "a1>a2>a 3>A1\t9\n";
}

void gen_score_table(const char* file)
{
    std::ofstream of(file);
    of << "a\ta1\t0\t1.1\ta2\t0\t2.2\tA1\t0\t3.3\n";
    of << "3\ta1\t0\t1.4\n";
    of << "1 0\ta1\t0\t1.1\ta2\t0\t2.2\ta 3\t0\t5\ta1>A1\t0\t45\n";
    of << "a b c\ta2\t0\t0.1\tB 4\t0\t10\ta1>a2>a 3>A1\t0\t123\n";
    of << "aaaa\ta1\t0\t1.1\ta2\t0\t2.2\tA1\t0\t3.3\tA1\t0\t90\tB 4\t0\t5.5\n";
    of << "b\n";
}


BOOST_AUTO_TEST_CASE(testDATrie)
{
    gen_cate_table("./cate_table.tmp");
    gen_score_table("./score_table.tmp");
cout<<"gen ok"<<endl;    
    CateClassifyCateDict c_dict("./cate_table.tmp");
cout<<"c_dict ok"<<endl;    
    CateClassifyScoreDict dict("./score_table.tmp", &c_dict);
cout<<"dict ok"<<endl;

    BOOST_CHECK(c_dict.size() == 7);
    BOOST_CHECK(c_dict.cate_trans(KString("abc")) == (size_t)-1);
    BOOST_CHECK(c_dict.cate_trans(KString("A1")) == (size_t)0);
    BOOST_CHECK(c_dict.get_score(c_dict.cate_trans(KString("B 4"))) == 5);
    BOOST_CHECK(c_dict.get_score(c_dict.cate_trans(KString("a1>A1"))) == 6);
    BOOST_CHECK(c_dict.get_score(-1) == c_dict.NOT_FOUND_);

    BOOST_CHECK(dict.size() == 6);  
    BOOST_CHECK(dict.token_trans(KString("abc")) == (size_t)-1);
    BOOST_CHECK(dict.token_trans(KString("1 0")) == (size_t)0);
    BOOST_CHECK(dict.size(dict.token_trans(KString("1 0"))) == 4);
    BOOST_CHECK(dict.size(dict.token_trans(KString("b"))) == 0); 
    BOOST_CHECK(dict.size(dict.token_trans(KString("w"))) == c_dict.NOT_FOUND_);
    BOOST_CHECK(dict.size(dict.token_trans(KString("a b c"))) == 3);
    BOOST_CHECK(dict.get_score(dict.token_trans(KString("1 0")), 0) == 1.1);
    BOOST_CHECK(dict.get_score(dict.token_trans(KString("a")), 2) == 3.3);
    BOOST_CHECK(dict.get_score(4, 111133) == c_dict.NOT_FOUND_);
    
    BOOST_CHECK(c_dict.cate_trans(dict.get_ind(dict.token_trans(KString("1 0")), 0)) == KString("a1"));
    BOOST_CHECK(c_dict.cate_trans(dict.get_ind(dict.token_trans(KString("aaaa")), 4)) == KString("B 4"));
    BOOST_CHECK(c_dict.cate_trans(1111222222) == KString(""));


}

BOOST_AUTO_TEST_SUITE_END()
