/*
 * =====================================================================================
 *
 *       Filename:  tokenize_util.cc
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  01/16/2013 11:10:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), kevin.hu@b5m.com
 *        Company:  iZeneSoft.com
 *
 * =====================================================================================
 */
#include <string>
#include <algorithm>
#include<iostream>
#include<fstream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "knlp/normalize.h"
#include "knlp/fmm.h"
#include "knlp/string_patterns.h"

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib;
void printHelp()
{
    cout<< "./normalize_util\n"<<"\t-p\t pattern path\n \
		\t-o\toutput file\n\
		\t-c\tcorpus path\n\
		\t-d\ttokenize and sort, token dict.\n\
		\t-t\ttop N\n\
		\t-s\ttokenize and split with space, token dict.\n";
}

bool cmp(const std::pair<KString, double>&a, const std::pair<KString, double>&  b)
{
    if (a.second > b.second)
        return true;
    return false;
}

int main(int argc,char * argv[])
{
    if (argc == 1)
    {
        printHelp();
        return 0;
    }

    char c = '?';
    string dictnm;
    string output;
    Fmm* fmm = NULL;
    bool split=false;
    uint32_t top = -1;
    vector<string> corpus;
    while ((c = getopt (argc, argv, "p:o:c:d:s:t:")) != -1)
        switch (c)
        {
        case 'p':
            dictnm = optarg;
            break;
        case 'o':
            output = optarg;
            break;
        case 'c':
            corpus.push_back(optarg);
            break;
        case 'd':
            fmm = new Fmm(optarg);
            break;
        case 't':
            top = atoi(optarg);
            break;
        case 's':
            fmm = new Fmm(optarg);
            split=true;
            break;
        case '?':
            printHelp();
        }

    ofstream of;
    if (output.length())of.open(output.c_str());

    GarbagePattern gp(dictnm);
    uint32_t t = 0;
    while(1)
    {
        if (t >= corpus.size() && corpus.size()>0)
            break;
        if (t<corpus.size())
        {
            if(freopen (corpus[t].c_str(), "r", stdin) == NULL)
              ;
            ++t;
        }
        string line;
        while(!std::getline(std::cin, line).eof())
        {
            const char* t = strchr(line.c_str(), '\t');
            string v;
            if (t)
            {
                v = t;
                line = line.substr(0, t-line.c_str());
            }
            line = gp.clean(line);
            try{
                KString L(line);ilplib::knlp::Normalize::normalize(L);
                KString k;
                if (fmm){
                    std::vector<std::pair<KString, double> > v;
                    fmm->fmm(L, v);
                    if(!split)std::sort(v.begin(), v.end(), cmp);
                    for (uint32_t i=0;i<v.size() && i<top;++i)
                    {
                        if (split && k.length()> 0 )k += ' ';
                        k += v[i].first;
                    }
                }
                if (output.length())
                {
                    if(!fmm)
                        of<<L<<v<<std::endl;
                    else{
                        of<<k<<v<<std::endl;
                    }
                }
                else{
                    if (fmm)
                        cout<<k<<v<<std::endl;
                    else
                        cout<<L<<v<<std::endl;
                }
            }
            catch(...){
                std::cout<<"Exception:"<<line<<"\n";
           }
        }
    }

    if (output.length())of.close();

    return 0;
}
