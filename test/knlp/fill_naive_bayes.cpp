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

#include "knlp/fmm.h"
#include "knlp/normalize.h"
#include "knlp/doc_naive_bayes.h"
#include "am/hashtable/khash_table.hpp"
#include "am/util/line_reader.h"

using namespace std;
using namespace ilplib::knlp;

int main(int argc,char * argv[])
{
    if (argc < 6)
    {
        std::cout<<argv[0]<<" [tokenize dict] [garbage dict] [syn dict] [output] [corpus 1] [corpus 2] ....\n\t[format]: doc\\tcategory\n";
        return 0;
    }

    string dictnm = argv[1];
    string garbagenm = argv[2];
    string synnm = argv[3];
    string output = argv[4];
    std::vector<std::string> cps;

    for ( int32_t i=5; i<argc; ++i)
      cps.push_back(argv[i]);
    DocNaiveBayes::train(output, dictnm, garbagenm, synnm, cps);
    return 0;
}
