/*
 * =====================================================================================
 *
 *       Filename:  grep.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013年06月06日 13时21分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), hukai.kevin@snda.com
 *        Company:  Snda.com
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
#include <vector>

#include "am/hashtable/khash_table.hpp"
#include "am/util/line_reader.h"

#include <boost/thread/thread.hpp>

using namespace std;
using namespace izenelib::am::util;
using namespace izenelib;


int main(int argc,char * argv[])
{
    if (argc < 4)
    {
        std::cout<<argv[0]<<" [corpus] [token1 token2] [max num]\n";
        return 0;
    }

	int32_t MAX_N = atoi(argv[3]);
	std::string tks(argv[2]);
	std::vector<std::string> vstr;
	uint32_t last = 0;
	for ( uint32_t i=0; i<tks.length(); ++i)
	  if (tks[i] == ' ' && i > last)
	  {
		  vstr.push_back(tks.substr(last, i-last));
		  last = i + 1;
	  }
	  else if (i == last)last++;
	  
	if (last < tks.length())
	  vstr.push_back(tks.substr(last));

	LineReader lr(argv[1], 1000000000);
	char* line = NULL;
	while(MAX_N>0 && (line=lr.line(line))!=NULL)
	{
		char* low = line;
		while(*low)
		{
			if(isupper(*low))
			  *low = tolower(*low);
			++low;
		}
		bool good = true;
		for ( uint32_t i=0; i<vstr.size(); ++i)
		  if ((low = strstr(line, vstr[i].c_str()))==NULL)
		  {
			  good = false;
			  break;
		  }
		if (good)
		{
			std::cout<<line<<std::endl;
			MAX_N --;
		}
	}
}
