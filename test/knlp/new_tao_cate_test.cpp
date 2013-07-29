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
#include <tuple>

#include "knlp/new_fmm.h"
#include "knlp/normalize.h"
#include "knlp/new_cate_classify.h"
#include "am/hashtable/khash_table.hpp"
#include "am/util/line_reader.h"


using namespace std;
using namespace ilplib::knlp;

Fmm* tkn;
CateClassify* worker;

void classify_vote(const std::string& str)
{
    KString kstr = KString(str);
    CateClassifyScoreDict::makeitclean(kstr);
    ilplib::knlp::Normalize::normalize(kstr);
//    std::vector<std::pair<KString, double> > v;
    std::vector<std::tuple<size_t, size_t, double> > v;
    try{
        tkn->fmm(kstr, v);
    }catch(...){}
/*
cout<<kstr<<endl;    
for(size_t i = 0; i < v.size(); ++i)
{
KString kkk = kstr.substr(get<0>(v[i]), get<1>(v[i])-get<0>(v[i]));
  cout<<get<1>(v[i])<<' '<<kkk.length()<<' '<<kkk<<endl;
}
//    cout<<v[i].first<<' '<<v[i].second<<'\t';
cout<<endl;
return;
*/
    std::stringstream ss;
    std::map<KString, double> m;
    m = worker->classify_multi_level(kstr,v,ss);
//    m = worker->classify_multi_level(v,ss,0);
//cout<<ss.str()<<endl;
/*    
    vector<pair<double,KString> > dv;
    for(std::map<KString, double>::iterator it=m.begin();it!=m.end();++it)
        dv.push_back(make_pair(it->second, it->first));
    sort(dv.begin(), dv.end(), std::greater<pair<double,KString> >());
    std::cout<<str<<"########\n";
    for(uint32_t i=0; i<dv.size()&&i<6; ++i)
          cout<<"@"<<dv[i].second<<":"<<dv[i].first<<", ";
    cout<<"\n";
    cout<<"\n"<<ss.str()<<endl;
*/    
}


int main(int argc,char * argv[])
{
    if (argc < 4)
    {
        std::cout<<argv[0]<<" [tokenize dict] [category dict] [term2cates dict] [corpus 1] ....\n";
        return 0;
    }
time_t time1,time2;
time1 = clock();
    tkn = new Fmm(argv[1]);

time2 = clock();
printf("read resource time = %lf\n",(double)(time2 - time1)/1000000);
    

    worker = new CateClassify(string(argv[3]), string(argv[2]));//, string(argv[3]));

time1 = clock();
printf("prepare time = %lf\n",(double)(time1 - time2)/1000000);


    string line;
size_t times = 0;    
    LineReader lr(argv[4]);
    char* ss = NULL;
    while((ss = lr.line(ss))!=NULL)
    {
        line = string(ss);
        ++times;
        classify_vote(line);
//        if (times%10000==0) cout<<times<<endl;
//if (times == 1000) break;        
    }
time2 = clock();
printf("tot term = %zu\ntot loop = %zu\n",worker->get_term(), worker->get_loop());
printf("classify %zu time = %lf\n",times,(double)(time2 - time1)/1000000);
    return 0;
}

