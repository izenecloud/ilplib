#include "knlp/cluster_detector.h"
#include <iostream>
#include "am/util/line_reader.h"
#include <boost/algorithm/string.hpp>
using namespace std;
using namespace ilplib::knlp;
using namespace izenelib::am;

GarbagePattern* gp = NULL;

void printHelp()
{
    cout<< "./cluster_util\n"<<"-g\tgarbage source file\n-d\tdict file\n-t\ttest file\n-s\tsyn dict\n";
}

int main(int argc,char * argv[])
{
    time_t t1,t2;
    char c = '?';
    string dict;
    string test;
    string syn_dict;

    while ((c = getopt (argc, argv, "g:d:t:s:")) != -1)
        switch (c)
        {
        case 'g':
            gp = new GarbagePattern(optarg);
            break;
        case 'd':
            dict = optarg;
            break;
        case 't':
            test = optarg;
            break;
        case 's':
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
    ClusterDetector cd(dict, syn_dict, gp);

    t1 = clock();
    char* st = NULL;
        
    LineReader lr(test);
    while ((st = lr.line(st)) != NULL)
    {
        
        string s(st), title, cate, att;

        size_t p = s.find("\t");
        if (p==string::npos)
            continue;
        title = s.substr(0, p);
        size_t q = s.find("\t", p+1);
        if(q!=p+1)cate = s.substr(p+1, q-p-1);
        if(q+1!=s.length())att = s.substr(q+1, s.length() - q - 1);
//        printf("title:%s\ncate:%s\natt%s\n", title.c_str(), cate.c_str(), att.c_str());
        string res = cd.cluster_detect(title, cate, att, 0);
        std::cout<<s<<"\n"<<res<<"\n\n";

        /*
        std::vector<std::string> str;
        boost::split(str, s, is_any_of("\t"));
        if(str.size()==3)
        {
            string res = cd.cluster_detect(str[0], str[1], str[2]);
            std::cout<<s<<"\n"<<res<<"\n\n";
        }
        */
    }
    t2=clock();
    cout<<(t2-t1)/1000000<<endl;
    
    return 0;
}

