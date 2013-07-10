/*
 * =====================================================================================
 *
 *       Filename:  fill_naive_bayes.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2013年05月24日 14时09分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), kevin.hu@b5m.com
 *        Company:  B5M.com
 *
 * =====================================================================================
 */
#include <string>
#include<iostream>
#include<fstream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "knlp/tokenize.h"
#include "knlp/normalize.h"
#include "knlp/doc_naive_bayes.h"
#include "am/hashtable/khash_table.hpp"
#include "am/util/line_reader.h"

using namespace std;
using namespace ilplib::knlp;

Tokenize* tkn;
DigitalDictionary* cat;
DigitalDictionary* term;
DigitalDictionary* t2c;
Dictionary* t2cs;
std::vector<std::string> catv;

void classify(std::string str)
{
    ilplib::knlp::Normalize::normalize(str);
    std::vector<std::pair<KString,double> > v;
    tkn->fmm(KString(str), v);

    std::map<KString, double> m = DocNaiveBayes::classify(cat,t2c,t2cs,v);
    vector<pair<double,KString> > dv;
    for(std::map<KString, double>::iterator it=m.begin();it!=m.end();++it)
        dv.push_back(make_pair(it->second*-1, it->first));
    sort(dv.begin(), dv.end());
    std::cout<<"###################\n"<<str<<"\n";
    for(uint32_t i=0; i<dv.size()&&i<6; ++i)
          cout<<"@@"<<dv[i].second<<":"<<dv[i].first<<std::endl;
    cout<<"#################33\n";
}

void classify_vote(std::string str)
{
    ilplib::knlp::Normalize::normalize(str);
    DocNaiveBayes::makeitclean(str);
    std::vector<std::pair<KString,double> > v;
    tkn->fmm(KString(str), v);

    std::stringstream ss;
    std::map<KString, double> m = DocNaiveBayes::classify_multi_level(cat,t2c,t2cs,v, ss);
    //std::map<KString, double> m = DocNaiveBayes::classify(cat,term, t2c,t2cs,v, ss);
    vector<pair<double,KString> > dv;
    for(std::map<KString, double>::iterator it=m.begin();it!=m.end();++it)
        dv.push_back(make_pair(it->second*-1, it->first));
    sort(dv.begin(), dv.end());
    std::cout<<str<<"########\n";
    for(uint32_t i=0; i<dv.size()&&i<6; ++i)
          cout<<"@"<<dv[i].second<<":"<<dv[i].first<<", ";
    cout<<"\n";
    cout<<"\n"<<ss.str()<<endl;
}
int main(int argc,char * argv[])
{
    if (argc < 6)
    {
        std::cout<<argv[0]<<" [tokenize dict] [category dict] [term dict] [term2cate dict] [term2cates dict] [corpus 1] ....\n";
        return 0;
    }

    tkn = new Tokenize(argv[1]);
    cat = new DigitalDictionary(argv[2]);
    term = new DigitalDictionary(argv[3]);
    t2c = new DigitalDictionary(argv[4]);
    t2cs = new Dictionary(argv[5]);

    /*{
        char* li = NULL;
        LineReader lr(argv[5]);
        while((li=lr.line(li))!=NULL)
        {
            char* t = strchr(li, '\t');
            if (!t) t = strchr(li, ' ');
            if (!t)continue;
            catv.push_back(string(li, t-li));
        }
    }*/

    string corpus;
    if (argc > 6)
        corpus = argv[6];
    if(corpus.length()>0 && freopen (corpus.c_str(), "r", stdin) == NULL);

    string line;
    while(!std::getline(std::cin, line).eof())
    {
        //classify(line);
        classify_vote(line);
    }

    return 0;
}
