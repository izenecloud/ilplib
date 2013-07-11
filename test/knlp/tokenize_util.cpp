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
#include<iostream>
#include<fstream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "knlp/tokenize.h"
#include "knlp/fmm.h"

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib;
void printHelp()
{
    cout<< "./tokenize_util\n"<<"\t-d\t dictionary path\n \
		\t-o\toutput file\n\
		\t-c\tcorpus path\n\
		\t-f\toutput with score\n\
		\t-t\tsplit charactor. White space is default charactor\n";
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
    bool outputScore = false;
    string SPLIT = "\t";
    vector<string> corpus;
    while ((c = getopt (argc, argv, "d:o:c:t:f")) != -1)
        switch (c)
        {
        case 'd':
            dictnm = optarg;
            break;
        case 'o':
            output = optarg;
            break;
        case 'c':
            corpus.push_back(optarg);
            break;
        case 'f':
            outputScore = true;
            break;
        case 't':
            SPLIT = optarg;
            break;
        case '?':
            printHelp();
        }

    ofstream of;
    if (output.length())of.open(output.c_str());

    Fmm tok(dictnm);
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
            std::vector<std::pair<KString, double> > r;
            tok.fmm(KString(line),r);
            for ( uint32_t i=0; i<r.size(); ++i)
            {
                if (output.length())
                {
                    of<<r[i].first;
                    if (outputScore)of<<SPLIT<<r[i].second;
                    of<<SPLIT;
                }
                else
                {
                    cout<<r[i].first;
                    if (outputScore)cout<<SPLIT<<r[i].second;
                    cout<<SPLIT;
                }
            }
            if (output.length())of<<std::endl;
            else cout<<std::endl;
        }
    }

    if (output.length())of.close();

    return 0;
}
