/*
 * =====================================================================================
 *
 *       Filename:  token_unigram_train.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/14/2013 05:00:00 PM
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

using namespace std;
using namespace ilplib::knlp;
void printHelp()
{
	cout<< "./tokenize_unigram_train\n"<<"\t-d\t dictionary path\n\t-o\toutput file\n\t-c\tcorpus path\n";
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
	vector<string> corpus;
	while ((c = getopt (argc, argv, "d:o:c:")) != -1)
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
			case '?':
				printHelp();
	   }

	Tokenize::train(dictnm, corpus, output);
	return 0;
}

