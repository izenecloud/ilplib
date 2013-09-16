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
    cout<< "./attr_util\n"<<"-t\ttest file\n-d\tsyn dict\n";
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
    string syn_dict;
    while ((c = getopt (argc, argv, "t:d:")) != -1)
        switch (c)
        {
        case 't':
            test = optarg;
            break;
        case 'd':
            syn_dict = optarg;
            break;
        case '?':
            printHelp();
        }

    if (test.empty())
    {
        printHelp();
        return 0;
    }

    AttributeNormalize *an = NULL;
    if(syn_dict.empty())
        an = new AttributeNormalize();
    else
        an = new AttributeNormalize(syn_dict);
    t1 = clock();
    char* st = NULL;
        
    LineReader lr(test);
    while ((st = lr.line(st)) != NULL)
    {
        string str(st), att, cate, title;
        vector<string> v;
        boost::split(v, str, boost::is_any_of("\t"));
        for(size_t i = 0; i < v.size(); ++i) if(!v[i].empty())
        {
            if(v[i][1]=='A')
                att=v[i].substr(11, v[i].length()-11);
            else if(v[i][1]=='C')
            {
                string tmp = v[i].substr(10, v[i].length()-10);
                int p = -1;
                for(size_t j = tmp.length()-1; j >= 0; --j)
                    if(tmp[j]=='>' || tmp[j]=='/')
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
<<<<<<< HEAD
        string res(an->attr_normalize(att, cate, 1));
        printf("%s\t%s\n",res.c_str(),title.c_str());
//        cout<<res<<"\t"<<title<<endl;
=======
        string res = an.attr_normalize(att, cate, 1);
        cout<<res<<"\t"<<title<<endl;
//        std::cout<<str<<"\n"<<res<<"\n\n";
>>>>>>> d32db9872daaaa8e8cecd85e7cbfa3a799a7c334
    }
    t2=clock();
    cout<<(t2-t1)/1000000<<endl;
    
    return 0;
}

