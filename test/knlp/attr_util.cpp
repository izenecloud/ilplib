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
    cout<< "./attr_util\n"<<"-s\tsource file\n-t\ttest file\n";
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
    string source;
    string test;
    while ((c = getopt (argc, argv, "s:t")) != -1)
        switch (c)
        {
        case 's':
            source = optarg;
            break;
        case 't':
            test = optarg;
            break;
        case '?':
            printHelp();
        }

    if (source.length() == 0)
    {
        printHelp();
        return 0;
    }

    AttributeNormalize an;
    t1 = clock();
    char* st = NULL;
        
    LineReader lr(source);
    while ((st = lr.line(st)) != NULL)
    {
        string str(st);
        str.erase(0,11);
        string res = an.attr_normalize(str);
//        printf("%s\n%s\n\n",str.c_str(),res.c_str());
        std::cout<<str<<"\n"<<res<<"\n\n";
    }
    t2=clock();
    cout<<(t2-t1)/1000000<<endl;
    
    return 0;
}

