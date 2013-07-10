#include<iostream>
#include<stdio.h>
#include<string>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "knlp/normalize.h"
#include<knlp/william_trie.h>
using namespace std;
using namespace ilplib::knlp;
//char st[10000010];
string st;
int main()
{
time_t t1,t2;
t2 = clock();
    string file_name = "sort.etao.term";
    WilliamTrie token(file_name);
    vector<pair<KString, double> > term;
/*
    KString kstr("我是以为");
cout<<token.check_term(kstr)<<' '<<token.score(kstr)<<endl;;
*/
t1 = clock();
cout<<"get dict time ="<<(double)(t1-t2)/1000000<<endl;
    freopen("new.cate.test","r",stdin);
    while(getline(cin,st))
    {
//        st[strlen(st)-1]=0;
        KString kstr(st);
        vector<pair<KString, double> > term(token.token(kstr));
//        token.token(kstr, term, len);
/*
cout<<kstr<<endl;
for(size_t i = 0; i < term.size(); ++i)
    cout<<term[i].first<<' '<<term[i].second<<' ';
cout<<endl;
*/
    }
cout<<"tot bi = "<<token.tot()<<endl;
cout<<"tot len = "<<token.totlen()<<endl;
t2 = clock();
cout<<"token time ="<<(double)(t2-t1)/1000000<<endl;
    
    return 0;
}

