#include <iostream>

#include "knlp/maxent/maxent_classify.h"

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib;
using namespace izenelib::util;


MaxentClassify* mc = NULL;
Fmm* fmm = NULL;
GarbagePattern* gp = NULL;

void printHelp()
{
    cout<< "./maxent_util\n"<<"\t-g\t pattern path\n \
		\t-t\ttraining corpus path\n\
		\t-p\ttesting corpus path\n\
		\t-m\tmodel name.\n\
		\t-d\ttoken dict.\n\
		\t-q\tuse query feature\n";
}


int main(int argc,char * argv[])
{
    if (argc == 1)
    {
        printHelp();
        return 0;
    }

    time_t starttm,endtm;
    char c = '?';
    string dictnm;
    string garbnm;
    string tcorpus;
    string pcorpus;
    string mdnm;
    bool query_feature = false;
    while ((c = getopt (argc, argv, "g:t:p:m:d:q")) != -1)
        switch (c)
        {
        case 'g':
            gp = new GarbagePattern(optarg);
            break;
        case 't':
            tcorpus = optarg;
            break;
        case 'p':
            pcorpus = optarg;
            break;
        case 'd':
            fmm = new Fmm(optarg);
            break;
        case 'm':
            mdnm = optarg;
            break;
        case 'q':
            query_feature = true;
            break;
        case '?':
            printHelp();
        }

    if (mdnm.length() == 0)
    {
        printHelp();
        return 0.;
    }

    size_t tot = 0, right = 0;

    if (tcorpus.length() > 0)
    {
        MaxentClassify::train(tcorpus, mdnm, fmm, gp, (query_feature ? 1:0));
        return 0;
    }

    if (pcorpus.length() > 0)
    {
        starttm = clock();
        char* st = NULL;
        mc = new MaxentClassify(mdnm, fmm, gp);
        
        LineReader lr(pcorpus);
        while ((st = lr.line(st)) != NULL)
        {
            ++tot;
            string str(st);
            
            int p = str.find("\t");
            string ti = str.substr(0, p);
            string cat = str.substr(p+1, str.length() - p - 1);
            if (cat.length() > 2 && cat[0]=='R')cat = cat.substr(2);
            std::stringstream ss;
            std::map<std::string, double> m;
            m = mc->classify(ti, ss, true, (query_feature ? 1:0));
            std::string ans;double max = -1000000;
            for (std::map<std::string, double>::const_iterator it=m.begin(); it!=m.end();++it)
                if (it->second > max)ans = it->first,max=it->second;

            if (ans.length()> 2 && ans[0]=='R')ans = ans.substr(2);
            if (strcmp(cat.c_str(), ans.c_str()) == 0) 
                ++right;
            else
                cout<<ti<<"\t"<<ans<<'\t'<<cat<<endl<<ss.str()<<std::endl;
        }
        cout<<"[Presicion]: "<<(double)right/tot<<endl;
        
        endtm = clock();
        cout<<"classify time = "<<(double)(starttm-endtm)/1000000<<endl;
    }
    
    return 0;
}

