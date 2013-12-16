/**
 * @file t_knlp_tokenize.cpp
 * @brief test ilplib::knlp::Tokenize in forward maximize match.
 */

#include "knlp/kdictionary.h"
#include "knlp/datrie.h"

#include <boost/test/unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <ostream>

using namespace ilplib;
using namespace ilplib::knlp;

BOOST_AUTO_TEST_SUITE(KDictionaryTest)

void generate_valid_keys(std::size_t num_keys,
    std::set<std::string> *valid_keys) {
      std::vector<char> key;
      while (valid_keys->size() < num_keys) {
          key.resize(1 + (std::rand() % 8));
          for (std::size_t i = 0; i < key.size(); ++i) {
              key[i] = 'A' + (std::rand() % 26);
          }
          valid_keys->insert(std::string(&key[0], key.size()));
      }
  }

void generate_invalid_keys(std::size_t num_keys,
  const std::set<std::string> &valid_keys,
  std::set<std::string> *invalid_keys) {
    std::vector<char> key;
    while (invalid_keys->size() < num_keys) {
        key.resize(1 + (std::rand() % 8));
        for (std::size_t i = 0; i < key.size(); ++i) {
            key[i] = 'A' + (std::rand() % 26);
        }
        std::string generated_key(&key[0], key.size());
        if (valid_keys.find(generated_key) == valid_keys.end())
            invalid_keys->insert(std::string(&key[0], key.size()));
    }
}

void gen_dict(const std::string& nm, const std::set<std::string>& set)
{
    {
        boost::filesystem::remove(nm+".bin");
        boost::filesystem::remove(nm+".v");
        std::ofstream of(nm.c_str());
        for (std::set<std::string>::const_iterator it=set.begin();it!=set.end();++it)
            of << *it <<std::endl;
    }{
        boost::filesystem::remove(nm+".str.bin");
        boost::filesystem::remove(nm+".str.v");
        std::ofstream of((nm+".str").c_str());
        for (std::set<std::string>::const_iterator it=set.begin();it!=set.end();++it)
            of << *it << "\t" << *it<<std::endl;
    }{
        boost::filesystem::remove(nm+".int.bin");
        boost::filesystem::remove(nm+".int.v");
        std::ofstream of((nm+".int").c_str());
        for (std::set<std::string>::const_iterator it=set.begin();it!=set.end();++it)
            of << *it << "\t" << it->length()<<std::endl;
    }{
        boost::filesystem::remove(nm+".flo.bin");
        boost::filesystem::remove(nm+".flo.v");
        std::ofstream of((nm+".flo").c_str());
        for (std::set<std::string>::const_iterator it=set.begin();it!=set.end();++it)
            of << *it << "\t" << it->length()*1.1<<std::endl;
    }
}

#include <time.h>
void basic_check(const std::set<std::string>& valid_keys, 
  const std::set<std::string>& invalid_keys)
{
    KDictionary<> _d("./tmp.dict");
    KDictionary<uint32_t> int_d("./tmp.dict.int");
    KDictionary<float> flo_d("./tmp.dict.flo");
    KDictionary<const char*> str_d("./tmp.dict.str");

    clock_t s = clock();
    for (std::set<std::string>::const_iterator it=valid_keys.begin();it!=valid_keys.end();++it)
    {
        BOOST_CHECK(int_d.has_key(*it));
        BOOST_CHECK(flo_d.has_key(*it));
        BOOST_CHECK(str_d.has_key(*it));
        BOOST_CHECK(_d.has_key(*it));

        {
            uint32_t v = -1;
            BOOST_CHECK(int_d.value(*it, v) == 0);
            BOOST_CHECK(v == it->length());
        }
        
        {
            int32_t v = -1;
            BOOST_CHECK(_d.value(*it, v) > 0);
        }

        {
            const char* v = "";
            BOOST_CHECK(str_d.value(*it, v) == 0);
            BOOST_CHECK(std::string(v) == *it);
        }
        {
            float v = -1;
            BOOST_CHECK(flo_d.value(*it, v) == 0);
            BOOST_CHECK(abs(v - (it->length()*1.1))<0.000001);
        }
    }
    
    for (std::set<std::string>::const_iterator it=invalid_keys.begin();it!=invalid_keys.end();++it)
    {
        BOOST_CHECK(!_d.has_key(*it));
        BOOST_CHECK(!int_d.has_key(*it));
        BOOST_CHECK(!flo_d.has_key(*it));
        BOOST_CHECK(!str_d.has_key(*it));
    }

    clock_t e = clock();
    printf ("It tooks  %f seconds for %d times lookups.\n",((float)e-s)/1000000, (int)(invalid_keys.size()*10+valid_keys.size()*0));
}

void datrie_basic_check(const std::set<std::string>& valid_keys, 
  const std::set<std::string>& invalid_keys)
{
    DATrie _d("./tmp.dict", 1);
    DATrie int_d("./tmp.dict.int", 0);
    DATrie flo_d("./tmp.dict.flo", 0);
    DATrie str_d("./tmp.dict.str", 2);

    clock_t s = clock();
    for (std::set<std::string>::const_iterator it=valid_keys.begin();it!=valid_keys.end();++it)
    {
        int_d.check_term(KString(*it));
        flo_d.check_term(KString(*it));
        str_d.check_term(KString(*it));
        _d.check_term(KString(*it));

        int_d.check_term(KString(*it));
        flo_d.check_term(KString(*it));
        str_d.check_term(KString(*it));
        _d.check_term(KString(*it));
    }
    
    for (std::set<std::string>::const_iterator it=invalid_keys.begin();it!=invalid_keys.end();++it)
    {
        int_d.check_term(KString(*it));
        flo_d.check_term(KString(*it));
        str_d.check_term(KString(*it));
        _d.check_term(KString(*it));
    }

    clock_t e = clock();
    printf ("It tooks  %f seconds for %d times lookups.\n",((float)e-s)/1000000, (int)(invalid_keys.size()*10+valid_keys.size()*0));
}

void fmm_check()
{
    boost::filesystem::remove("./tmp.bin");
    boost::filesystem::remove("./tmp.v");
    std::ofstream of("tmp");
    of <<"中\t1\n";
    of <<"中国\t2\n";
    of <<"中国人\t3\n";
    of <<"美\t4\n";
    of <<"美国\t5\n";
    of <<"美国人\t6\n";
    of <<"南京\t7\n";
    of <<"南京市\t8\n";
    of <<"市长\t9\n";
     of <<"长江\t10\n";
    of <<"大桥\t11\n";
    of <<"江大桥\t12\n";
    of <<"G\t13\n";
    of <<"Great Wall\t14\n";
    of <<"Great\t15\n";
    of <<"Wall\t16\n";
    of.close();

    KDictionary<> _d("./tmp");
    std::vector<std::pair<std::string, int32_t> > tks;
    std::vector<std::pair<std::string, int32_t> > subs;
    _d.fmm("中国的中国人美国的美国人，南京市长江大桥，Ggreat wall！！！", tks);
    _d.subtokens(tks, subs);
    for(uint32_t i=0;i<tks.size();i++)
        std::cout<<tks[i].first<<" "<<tks[i].second<<std::endl;
    for(uint32_t i=0;i<subs.size();i++)
        std::cout<<subs[i].first<<" "<<subs[i].second<<std::endl;

    BOOST_CHECK(tks[0].first.compare("中国") == 0);
    BOOST_CHECK(tks[1].first.compare("的") == 0);
    BOOST_CHECK(tks[2].first.compare("中国人") == 0);
    BOOST_CHECK(tks[3].first.compare("美国") == 0);
    BOOST_CHECK(tks[4].first.compare("的") == 0);
    BOOST_CHECK(tks[5].first.compare("美国人") == 0);
    BOOST_CHECK(tks[6].first.compare(",") == 0);
    BOOST_CHECK(tks[7].first.compare("南京市") == 0);
    BOOST_CHECK(tks[8].first.compare("长江") == 0);
    BOOST_CHECK(tks[9].first.compare("大桥") == 0);
    BOOST_CHECK(tks[10].first.compare(",") == 0);
    BOOST_CHECK(tks[11].first.compare("g") == 0);
    BOOST_CHECK(tks[12].first.compare("great wall") == 0);
    BOOST_CHECK(tks[13].first.compare("!") == 0);

    BOOST_CHECK(tks[0].second == 2);
    BOOST_CHECK(tks[1].second == 1);
    BOOST_CHECK(tks[2].second == 3);
    BOOST_CHECK(tks[3].second == 5);
    BOOST_CHECK(tks[4].second == 1);
    BOOST_CHECK(tks[5].second == 6);
    BOOST_CHECK(tks[6].second == 1);
    BOOST_CHECK(tks[7].second == 8);
    BOOST_CHECK(tks[8].second == 10);
    BOOST_CHECK(tks[9].second == 11);
    BOOST_CHECK(tks[10].second == 1);
    BOOST_CHECK(tks[11].second == 13);
    BOOST_CHECK(tks[12].second == 14);
    BOOST_CHECK(tks[13].second == 1);
    BOOST_CHECK(tks[14].second == 1);
}

BOOST_AUTO_TEST_CASE(KDicarionaryDATrie)
{
    const uint32_t DICT_SIZE = 10;
    std::set<std::string> valid_keys, invalid_keys;
    generate_valid_keys(DICT_SIZE, &valid_keys);
    generate_invalid_keys(DICT_SIZE, valid_keys, &invalid_keys);

/*
    std::vector<std::string> tmp;
    tmp.reserve(valid_keys.size()+1);
    for (std::set<std::string>::const_iterator it=valid_keys.begin();it!=valid_keys.end();++it)
        tmp.push_back(*it);

    gen_dict("./tmp.dict", valid_keys);

    basic_check(valid_keys, invalid_keys);
    basic_check(valid_keys, invalid_keys);
    //datrie_basic_check(valid_keys, invalid_keys);
    fmm_check();*/
}

BOOST_AUTO_TEST_SUITE_END()
