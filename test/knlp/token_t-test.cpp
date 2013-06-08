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
#include "am/hashtable/khash_table.hpp"
#include "am/util/line_reader.h"
#include "net/seda/queue.hpp"

#include <boost/thread/thread.hpp>

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib::am;
using namespace izenelib::am::util;
using namespace izenelib;

/* 
 * mu(t) = N(t)/N
 * X(c, t) = N(c, t)/N(c)
 * Delta(c, t) = sqrt((pow((N(c)-N(c,t))/N(c), 2)*(N(c)-N(c,t))+N(c,t)*pow(N(c,t)/N(c),2))/(N(c)-1))
 *
 * (X(c,t)- mu(t))/Delta(c, t)*sqrt(N(c)-1)
 *
 * */

ilplib::knlp::Tokenize* tkn;
KStringHashTable<string,uint32_t>*	Nt;
KStringHashTable<string,uint32_t>* Nc;
KStringHashTable<string,uint32_t>* Nct;
std::set<string> cates;
std::vector<string> tks;
uint32_t N = 0;
typedef KStringHashTable<string,uint32_t> freq_t;

std::pair<double,double>
t_test(const string& c, const string& t)
{
	double nt = 0, nct = 0, nc = 0;
	{
		uint32_t* f = Nt->find(t);
		if (f) nt = *f;
	}
	{
		string ct = t;ct+='\t';ct+=c;
		uint32_t* f = Nct->find(ct);
		if (f) nct = *f;
	}
	{
		uint32_t* f = Nc->find(c);
		IASSERT(f);
		if (f) nc = *f;
	}
	//std::cout<<c<<":"<<t<<":"<<nt<<":"<<nct<<":"<<nc<<":"<<nct/nc<<":"<<nt/N<<std::endl;

	return make_pair((nct/nc - nt/N)*sqrt(nc-1)/sqrt(((nc-nct)*pow((nc-nct)/nc,2)+nct*nct*nct/nc/nc)/(nc-1)), pow((nc-nct+0.5)/(nct+0.5), 1.04));
}

void calculate_stage(EventQueue<std::pair<string*,string*> >* out)
{
	while(true)
	{
		uint64_t e = -1;
		std::pair<string*,string*> p(NULL,NULL);
		out->pop(p, e);
		string* t = p.first;
		string* c = p.second;
		if (t == NULL || c == NULL)
		  break;
		
		//add Nc
		if (cates.find(*c) == cates.end())
		{
			cates.insert(*c);
			Nc->insert(*c, 1);
		}
		else{
			uint32_t* f = Nc->find(*c);
			IASSERT(f);
			(*f)++;
		}
		
		N++;

		{//Nt
			uint32_t* f = Nt->find(*t);
			if (!f)
			{
				Nt->insert(*t,1);
				tks.push_back(*t);
			}
			else (*f)++;
		}{//Nct
			(*t) += '\t';(*t) += (*c);
			uint32_t* f = Nct->find(*t);
			if (!f)
				Nct->insert(*t,1);
			else (*f)++;
		}

		delete t, delete c;
	}
}

void tokenize_stage(EventQueue<std::pair<string*,string*> >* in, 
			EventQueue<std::pair<string*,string*> >* out )
{
	while(true)
	{	
		uint64_t e = -1;
		std::pair<string*,string*> p(NULL, NULL);
		in->pop(p, e);
		string* t = p.first;
		string* c = p.second;
		if (t == NULL || c == NULL)
		  break;
		ilplib::knlp::Normalize::normalize(*t);
		std::vector<KString> v = tkn->fmm(KString(*t));
		std::set<string> s;
		for ( uint32_t i=0; i<v.size(); ++i)
		  if (v[i].length() > 0 && KString::is_chinese(v[i][0]))
			  s.insert(v[i].get_bytes("utf-8"));
		
		for ( std::set<string>::iterator it=s.begin(); it!=s.end(); ++it)
			out->push(make_pair(new string(it->c_str()), new string(c->c_str())), e);
		delete t, delete c;
	}
}

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

	tkn = new ilplib::knlp::Tokenize(dictnm);
	Nt = new freq_t(tkn->size()*3, tkn->size()+3);
	Nc = new freq_t(300, 100);
	Nct = new freq_t(tkn->size()*3*100, tkn->size()*100+3);
	tks.reserve(tkn->size());

	EventQueue<std::pair<string*,string*> > in, out;
	uint32_t cpu_num = 4;
	std::vector<boost::thread*> token_ths;
	for ( uint32_t i=0; i<cpu_num; ++i)
	  token_ths.push_back(new boost::thread(&tokenize_stage, &in, &out));
	boost::thread cal_th(&calculate_stage, &out);

	for ( uint32_t i=0; i<cps.size(); ++i)
	{
		LineReader lr(cps[i]);
		char* line = NULL;
		while((line=lr.line(line))!=NULL)
		{
			if (strlen(line) == 0)continue;
			char* t = strchr(line, '\t');
			if (!t)continue;
			t++;
			if (strlen(t) == 0)continue;

			in.push(make_pair(new std::string(line, t-line-1), new string(t)), -1);
		}
	}

	for ( uint32_t i=0; i<cpu_num; ++i)
	  in.push(make_pair<string*,string*>(NULL, NULL), -1);
	for ( uint32_t i=0; i<cpu_num; ++i)
	  token_ths[i]->join(),delete token_ths[i];
	out.push(make_pair<string*,string*>(NULL, NULL), -1);
	cal_th.join();

	for ( uint32_t i=0; i<tks.size(); ++i)
	{
		std::vector<std::pair<std::pair<double,double>, string> > v;
		v.reserve(cates.size());
		for ( std::set<string>::iterator it=cates.begin(); it!=cates.end(); ++it)
		{
			std::pair<double,double> p = t_test(*it, tks[i]);
			v.push_back(make_pair(p, *it));
			std::cout<<"-1111111\t"<<p.first<<"\t"<<p.second<<"\t"<<tks[i]<<"\t"<<*it<<std::endl;;
		}
		std::vector<std::pair<std::pair<double,double>, string> >::iterator it = max_element(v.begin(), v.end());
		std::cout<<it->first.first*it->first.second<<"\t"<<tks[i]<<"\t"<<it->second<<std::endl;
	}

	return 0;
}
