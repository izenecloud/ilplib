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
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "knlp/keyword_condition.h"
#include "knlp/fmm.h"
#include "knlp/string_patterns.h"

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib;
void printHelp()
{
    cout<< "./kw_condition_util\n"<<"\t-d\t resource path\n";
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
    while ((c = getopt (argc, argv, "d:")) != -1)
        switch (c)
        {
        case 'd':
            dictnm = optarg;
            break;
        case '?':
            printHelp();
        }

    KeywordCondition kc(dictnm);
    std::string line;
    while(!std::getline(std::cin, line).eof())
    {
        std::cout<<line<<"SSssss\n";
        std::vector<ConditionItem> condItems;
        std::vector<std::pair<std::string, std::string> > conds = kc.conditions(line, condItems);
        for (uint32_t i=0;i<conds.size();i++)
            std::cout<<conds[i].first<<"\t"<<conds[i].second<<std::endl;
    }

    return 0;
}
