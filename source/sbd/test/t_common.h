#ifndef _SBD_T_COMMON_H_
#define _SBD_T_COMMON_H_


#include <ilplib.hpp>

using namespace sbd;

void print(vector<Token>& tokens)
{
	for (int i=0; i<tokens.size(); ++i)
	{
		cout << tokens[i].tok << ", ";
		cout << tokens[i].type << ", ";
		cout << tokens[i].parastart << ", ";
		cout << tokens[i].linestart << ", ";
		cout << tokens[i].abbr << ", ";
		cout << tokens[i].ellipsis << ", ";
		cout << tokens[i].sentbreak << ", ";
		cout << tokens[i].period_final << endl;
	}
}

void print(vector<string>& sents)
{
	for (int i=0; i<sents.size(); ++i)
	{
		cout << "[" <<  i << "]"  << sents[i] << endl;
	}
}

#endif
