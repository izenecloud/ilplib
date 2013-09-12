#include <iostream>
#include "knlp/attr_normalize.h"
#include <boost/algorithm/string.hpp>
#include "am/util/line_reader.h"

using namespace std;
using namespace izenelib::am::util;
using namespace ilplib::knlp;
using namespace izenelib;
using namespace izenelib::util;


void printHelp()
{
    cout<< "./attr_util\n"<<"-t\ttest file\n";
}


int main(int argc,char * argv[])
{
    if (argc == 1)
    {
        printHelp();
        return 0;
    }

    time_t t1,t2;
    char c = '?';
    string test;
    while ((c = getopt (argc, argv, "t:")) != -1)
        switch (c)
        {
        case 't':
            test = optarg;
            break;
        case '?':
            printHelp();
        }

    if (test.length() == 0)
    {
        printHelp();
        return 0;
    }

    AttributeNormalize an;
    t1 = clock();
    char* st = NULL;
        
    LineReader lr(test);
    while ((st = lr.line(st)) != NULL)
    {
        string str(st), att, cate, title;
        vector<string> v;
        boost::split(v, str, is_any_of("\t"));
        for(size_t i = 0; i < v.size(); ++i) if(!v[i].empty())
        {
            if(v[i][1]=='A')
                att=v[i].substr(11, v[i].length()-11);
            else if(v[i][1]=='C')
            {
                string tmp = v[i].substr(10, v[i].length()-10);
                int p = -1;
                for(size_t j = tmp.length()-1; j >= 0; --j)
                    if(tmp[j]=='>')
                    {
                        p = j;
                        break;
                    }
                if (p>-1)
                    cate = tmp.substr(p+1, tmp.length()-p-1);
                else cate = tmp;
            }
            else if(v[i][1]=='T')title=v[i].substr(7, v[i].length()-7);
        }
//        cout<<att<<'\n'<<title<<'\n'<<cate<<'\n';
        string res = an.attr_normalize(att, 0, cate);
        cout<<res<<"\t"<<title<<endl;
//        std::cout<<str<<"\n"<<res<<"\n\n";
    }
    t2=clock();
    cout<<(t2-t1)/1000000<<endl;
    
    return 0;
}

