#include<iostream>
#include<stdio.h>
#include<string>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "knlp/normalize.h"
#include "knlp/datrie.h"
#include "am/util/line_reader.h"

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib::am::util;
//char st[10000010];
string st;
int main()
{
KString a("我们a和b");
for(size_t i = 0; i< a.length(); ++i)
    cout<<(int)a[i]<<endl;
time_t t1,t2;
t2 = clock();
    string file_name = "data/syn.dict";
    DATrie dict(file_name,2);
    vector<pair<KString, double> > term;
    KString res;
dict.find_syn(KString("health"), res);
cout<<res<<endl;
return 0;
//    KString kstr("爱步");
//cout<<dict.check_term(kstr)<<' '<<dict.find_word(kstr)<<endl;

t1 = clock();
cout<<"get dict time ="<<(double)(t1-t2)/1000000<<endl;
/*
freopen("testda", "w", stdout);
KString kstr = KString("架子鼓");
cout<<dict.find_word(kstr)<<endl;
kstr = KString("架鼓");
cout<<dict.find_word(kstr)<<endl;
*/
LineReader lr("test");
size_t tot = 0;
char* line = NULL;
    while((line=lr.line(line))!=NULL)
    {
        ++tot;
//if(tot == 100)break;        
//        KString kstr(line);
//        vector<pair<string, double> > term(dict.token(kstr));
//        vector<tuple<size_t, size_t, double> > term(dict.token(kstr));
//        vector<pair<KString, double> > term;
//        dict.token(kstr, term);
          
/*
cout<<kstr<<endl;
for(size_t i = 0; i < term.size(); ++i)
    cout<<term[i].first<<' '<<term[i].second<<endl;
cout<<endl;
*/
    }
t2 = clock();
printf("process %zu queries, time = %lf\n",tot, (double)(t2 - t1) / 1000000);
    
    return 0;
}

