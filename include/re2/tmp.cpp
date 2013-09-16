#include<iostream>
#include "re2/re2.h"
using std::cout;
using std::endl;
using std::string;
int main()
{
    re2::RE2 re("(\\w+):(\\d+)");
    string st="我\\们\"在你";
    string s,s1;
    int i;
    re2::RE2::FullMatch("ruby:1234", "(\\w+):(\\d+)", &s, &s1);
    cout<<st<<endl;
    RE2::GlobalReplace(&st, "[\"]+", " ");
    cout<<st<<endl;
    

    return 0;
}
