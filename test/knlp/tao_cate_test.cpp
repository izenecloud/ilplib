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

#include "knlp/fmm.h"
#include "knlp/normalize.h"
#include "knlp/cate_classify.h"
#include "am/hashtable/khash_table.hpp"
#include "am/util/line_reader.h"

using namespace std;
using namespace ilplib::knlp;

Fmm* tkn;
CateClassify* cat;

void classify(std::string str)
{
    KString kstr(str);
    ilplib::knlp::Normalize::normalize(kstr);
    CateClassifyScoreDict::makeitclean(kstr);
    std::vector<std::pair<KString,double> > v;
    try{
        tkn->fmm(kstr, v);
    }catch(...){}

    std::stringstream ss;
    std::map<KString, double> m = cat->classify_stage_2(v, ss, true);
    vector<pair<double,KString> > dv;
    for(std::map<KString, double>::iterator it=m.begin();it!=m.end();++it)
        dv.push_back(make_pair(it->second, it->first));
    sort(dv.begin(), dv.end(), std::greater<pair<double,KString> >());
    std::cout<<str<<"########\n";
    for(uint32_t i=0; i<dv.size()&&i<6; ++i)
          cout<<"@"<<dv[i].second<<":"<<dv[i].first<<", ";
    cout<<"\n";
    cout<<"\n"<<ss.str()<<endl;
}
int main(int argc,char * argv[])
{
    if (argc < 5)
    {
        std::cout<<argv[0]<<" [tokenize dict] [category dict] [term2cates dict] [corpus 1] ....\n";
        return 0;
    }

    tkn = new Fmm(argv[1]);
    cat = new CateClassify(argv[3], argv[2]);

    string corpus;
    if (argc > 4)
        corpus = argv[4];
    if(corpus.length()>0 && freopen (corpus.c_str(), "r", stdin) == NULL);

    string line;
    while(!std::getline(std::cin, line).eof())
    {
        classify(line);
    }

    return 0;
}
