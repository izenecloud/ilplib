/*
 * =====================================================================================
 *
 *       Filename:  fill_idf.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013年05月24日 08时55分41秒
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
#include "am/hashtable/khash_table.hpp"
#include "am/util/line_reader.h"

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib::am;
using namespace izenelib::am::util;

int main(int argc,char * argv[])
{
	if (argc < 4)
	{
		std::cout<<argv[0]<<" [tokenize dict] [output] [corpus 1] [corpus 2] ....\n";
		return 0;
	}

	string dictnm = argv[1];
	string output = argv[2];

	ilplib::knlp::Tokenize tkn(dictnm);
	KStringHashTable<KString, double> idft(tkn.size()*3, tkn.size()+3);

	uint32_t docn = 0;
	for ( int32_t i=3; i<argc; ++i)
	{
		LineReader lr(argv[i]);
		char* line = NULL;
		while((line=lr.line(line))!=NULL)
		{
			if (strlen(line) == 0)continue;
			docn++;
			KString kstr(line);
			ilplib::knlp::Normalize::normalize(kstr);
			std::vector<KString> v = tkn.tokenize(kstr);
			std::set<KString> set(v.begin(), v.end());
			for ( std::set<KString>::const_iterator it=set.begin(); it!=set.end(); ++i)
			{
				double* f = idft.find(*it);
				if (f)(*f)++;
				else idft.insert(*it, 1);
			}
		}
	}


	for(KStringHashTable<KString, double>::iterator it = idft.begin();it!=idft.end();++it)
	{
		double t = *it.value();
		*it.value() = log((docn-t+0.5)/(t+0.5));
	}
	idft.insert(KString("[[NONE]]"), log((docn+0.5)/0.5));

	idft.persistence(output);

	return 0;
}
