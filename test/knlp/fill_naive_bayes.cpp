/*
 * =====================================================================================
 *
 *       Filename:  fill_naive_bayes.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2013年05月24日 14时09分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), kevin.hu@b5m.com
 *        Company:  B5M.com
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
#include "knlp/normalize.h"
#include "knlp/doc_naive_bayes.h"
#include "am/hashtable/khash_table.hpp"
#include "am/util/line_reader.h"

using namespace std;
using namespace ilplib::knlp;

int main(int argc,char * argv[])
{
    if (argc < 4)
    {
        std::cout<<argv[0]<<" [tokenize dict] [output] [corpus 1] [corpus 2] ....\n\t[format]: doc\\tcategory\n";
        return 0;
    }

    string dictnm = argv[1];
    string output = argv[2];
    std::vector<std::string> cps;

    for ( int32_t i=3; i<argc; ++i)
      cps.push_back(argv[i]);
    DocNaiveBayes::train(dictnm, output, cps);

    return 0;
}
