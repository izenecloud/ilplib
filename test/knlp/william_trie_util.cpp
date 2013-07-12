#include<iostream>
#include<stdio.h>
#include<string>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "knlp/normalize.h"
#include "knlp/william_trie.h"
#include "am/util/line_reader.h"

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib::am::util;
//char st[10000010];
string st;
int main()
{
time_t t1,t2;
t2 = clock();
    string file_name = "brand.term";
    WilliamTrie token(file_name);
    vector<pair<KString, double> > term;
/*
    KString kstr("我是以为");
cout<<token.check_term(kstr)<<' '<<token.score(kstr)<<endl;;
*/
t1 = clock();
cout<<"get dict time ="<<(double)(t1-t2)/1000000<<endl;

LineReader lr("cate.test");    
char* line = NULL;
    while((line=lr.line(line))!=NULL)
    {
        KString kstr(line);
        vector<pair<KString, double> > term(token.token(kstr));
//        token.token(kstr);

cout<<kstr<<endl;
for(size_t i = 0; i < term.size(); ++i)
    cout<<term[i].first<<' '<<term[i].second<<endl;
cout<<endl;

    }
cout<<"tot bi = "<<token.tot()<<endl;
cout<<"tot len = "<<token.totlen()<<endl;
t2 = clock();
cout<<"token time ="<<(double)(t2-t1)/1000000<<endl;
    
    return 0;
}

