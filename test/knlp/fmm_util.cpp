/*
 * =====================================================================================
 *
 *       Filename:  fmm_util.cc
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
#include "knlp/fmm.h"
#include "knlp/normalize.h"
#include "am/hashtable/khash_table.hpp"
#include "am/util/line_reader.h"
#include "net/seda/queue.hpp"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib::am;
using namespace izenelib::am::util;
using namespace izenelib;

void printHelp()
{
    cout<< "./tokenize_util\n"<<"\t-d\t dictionary path\n \
		\t-c\tcorpus path\n\
		\t-n\tCPU number\n";
}

EventQueue<string*> in, out;

void tokenize_stage(Fmm* tkn)
{
	while(true)
	{	
		uint64_t e = -1;
		string* li = NULL;
		in.pop(li, e);
		if (li == NULL)
		    break;

		KString kstr(*li);delete li;
		ilplib::knlp::Normalize::normalize(kstr);
		std::vector<KString> v = tkn->fmm(kstr, false);
		KString o;
		for ( uint32_t i=0; i<v.size(); ++i)
		    o += v[i], o+= '\t';
		o += '\n';
        out.push(new string(o.get_bytes("utf-8").c_str()), -1);
	}
}

void print_stage()
{
    while(true)
    {
        uint64_t e = -1;
        string* tk = NULL;
        out.pop(tk, e);
        if (tk == NULL)
            break;
        std::cout<<*tk<<"\t";
        delete tk;
    }
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
    string SPLIT = "\t";
    string corpus;
    uint32_t cpu_num = 2;
    while ((c = getopt (argc, argv, "d:n:c:")) != -1)
        switch (c)
        {
        case 'd':
            dictnm = optarg;
            break;
        case 'c':
            corpus = optarg;
            break;
        case 'n':
            cpu_num = atoi(optarg);
            break;
        case '?':
            printHelp();
        }


    Fmm tok(dictnm);

	std::vector<boost::thread*> token_ths;
	for ( uint32_t i=0; i<cpu_num; ++i)
	  token_ths.push_back(new boost::thread(&tokenize_stage, &tok));
	boost::thread print_th(&print_stage);

    LineReader lr(corpus);
    char* line = NULL;
    while((line=lr.line(line))!=NULL)
    {
        if (strlen(line) == 0)continue;

        in.push(new std::string(line),-1);
    }

	for ( uint32_t i=0; i<cpu_num; ++i)
	  in.push(NULL, -1);
	for ( uint32_t i=0; i<cpu_num; ++i)
	  token_ths[i]->join(),delete token_ths[i];
	out.push(NULL, -1);
	print_th.join();

    return 0;
}
