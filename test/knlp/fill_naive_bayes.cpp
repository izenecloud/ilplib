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
    {
        ilplib::knlp::Tokenize tkn(dictnm);
        DocNaiveBayes nb(&tkn, output);
        KString line("iphone");
        //KString line("iPhone4 8G手机 支持移动/联通 双摄像头-黑色、白色");
        std::cout<<nb.classify(line, KString("手机数码>手机配件>手机保护套"))<<"手机数码>手机配件>手机保护套\n";
        std::cout<<nb.classify(line, KString("图书音像>教育>考试"))<<"图书音像>教育>考试\n";
        std::cout<<nb.classify(line, KString("汽车用品"))<<"汽车用品\n";
        std::cout<<nb.classify(line, KString("手机数码>手机配件>手机充电器"))<<"手机数码>手机配件>手机充电器\n";
        std::cout<<nb.classify(line, KString("手机数码>手机通讯>手机"))<<"手机数码>手机通讯>手机\n";
        std::cout<<nb.classify(line, KString("家居家装>生活日用>缝纫用品"))<<"家居家装>生活日用>缝纫用品\n";
        std::cout<<nb.classify(line, KString("服饰鞋帽>配饰"))<<"服饰鞋帽>配饰\n";
        std::cout<<nb.classify(line, KString("图书音像>教育>学习"))<<"图书音像>教育>学习\n";
    }
    return 0;
}
