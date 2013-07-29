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
#include "knlp/doc_naive_bayes.h"
#include "am/hashtable/khash_table.hpp"
#include "am/util/line_reader.h"
#include "knlp/cate_classify.h"

using namespace std;
using namespace ilplib::knlp;

Fmm* tkn;
ilplib::knlp::GarbagePattern* gp;
DigitalDictionary* cat;
VectorDictionary* vt2cs;
VectorDictionary* pct;//p(c|T)
Dictionary* syn;//synonym
CateClassify* cc;

KString classify(std::string str, std::stringstream& ss)
{
    str = gp->clean(str);
    KString kstr(str);
    ilplib::knlp::Normalize::normalize(kstr);
    std::vector<std::pair<KString,double> > v;
    tkn->fmm(kstr, v);
    for (uint32_t i=0;i<v.size();i++)
        ss << v[i].first<<":"<<v[i].second<<" ";
    ss << std::endl;

//    std::map<KString, double> m = cc->classify_multi_level(v, ss, true);
    std::map<KString, double> m = DocNaiveBayes::classify_multi_level(cat,vt2cs,pct,v, ss, true, syn);
    if (m.size() == 0)return KString();
    //std::map<KString, double> m = DocNaiveBayes::classify(cat,term, t2c,t2cs,v, ss);
    vector<pair<double,KString> > dv;
    for(std::map<KString, double>::iterator it=m.begin();it!=m.end();++it)
        dv.push_back(make_pair(it->second, it->first));

    sort(dv.begin(), dv.end(), std::greater<pair<double,KString> >());
    for (uint32_t i=0;i<dv.size();++i)
        ss<<dv[i].first<<":"<<dv[i].second<<std::endl;
    return dv.begin()->second;
}

int main(int argc,char * argv[])
{
    if (argc < 7)
    {
        std::cout<<argv[0]<<" [tokenize dict] [category dict] [term2cates dict] [pct dict] [garbage dict] [syn dict] [corpus 1] ....\n";
        return 0;
    }

    tkn = new Fmm(argv[1]);
    gp = new ilplib::knlp::GarbagePattern(argv[5]);
    cat = new DigitalDictionary(argv[2]);
    vt2cs = new VectorDictionary(argv[3]);
    pct = new VectorDictionary(argv[4]);
    syn = new Dictionary(argv[6]);
//    cc = new CateClassify(argv[3], argv[2], argv[4]);

    string corpus;
    if (argc > 7)
        corpus = argv[7];
    if(corpus.length()>0 && freopen (corpus.c_str(), "r", stdin) == NULL);

    std::stringstream sss;
    uint32_t C = 0, R = 0;
    string line;
    while(!std::getline(std::cin, line).eof())
    {
        C++;
        uint32_t t = line.find('\t');
        std::stringstream ss;
        try{
        KString c = classify(line.substr(0, t), ss);
        t++;
        if (line[t] != 'R' && c.length()>2)
            c = c.substr(2);
        if (c == KString(line.substr(t)))
            R++;
        else
            sss<<line.substr(0, t)<<"==>>"<<c<<"\n"<<ss.str();
        }catch(...){}
    }

    cout<<"precision: "<< 1.*R/C<<endl<<sss.str()<<"\n";

    return 0;
}
