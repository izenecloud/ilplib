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
time_t t1,t2;
t2 = clock();
    string file_name = "brand.term";
    DATrie dict(file_name);
    vector<pair<KString, double> > term;

    KString kstr("爱步");
cout<<dict.check_term(kstr)<<' '<<dict.find_word(kstr)<<endl;

t1 = clock();
cout<<"get dict time ="<<(double)(t1-t2)/1000000<<endl;
/*
freopen("testda", "w", stdout);
KString kstr = KString("架子鼓");
cout<<dict.find_word(kstr)<<endl;
kstr = KString("架鼓");
cout<<dict.find_word(kstr)<<endl;
*/
LineReader lr("/opt/kevin-sf1r-bin/new_cate.test");    
size_t tot = 0;
char* line = NULL;
    while((line=lr.line(line))!=NULL)
    {
        ++tot;
//if(tot == 100)break;        
        KString kstr(line);
        vector<pair<KString, double> > term(dict.token(kstr));
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

