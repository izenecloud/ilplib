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
#include "knlp/attr_tokenize.h"
#include "knlp/string_patterns.h"

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib;
void printHelp()
{
    cout<< "./attr_token_util\n"
	<<"\t-c\tcorpus path\n\
	\t-f\toutput score\n\
		\t-d\ttokenize dict dir.\n";
}

int main(int argc,char * argv[])
{
    if (argc == 1)
    {
        printHelp();
        return 0;
    }

    char c = '?';
    AttributeTokenize* token = NULL;
    bool score_f=false;
    vector<string> corpus;
    while ((c = getopt (argc, argv, "c:d:f")) != -1)
        switch (c)
        {
        case 'f':
            score_f = true;
            break;
        case 'c':
            corpus.push_back(optarg);
            break;
        case 'd':
            token = new AttributeTokenize(optarg);
            break;
        case '?':
            printHelp();
        }

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
	 try{
		std::vector<std::pair<std::string, int> > tks;
		token->tokenize(line, tks);
		for (uint32_t i=0;i<tks.size();i++)
		{
		    std::cout<<tks[i].first<<"\t";
		    if (score_f)
			std::cout<<tks[i].second<<"\t";
		}
		std::cout<<std::endl;
            }
            catch(...){
                std::cout<<"Exception:"<<line<<"\n";
           }
        }
    }

    return 0;
}
