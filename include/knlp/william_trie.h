#ifndef _ILPLIB_NLP_WILLIAM_TRIE_H_
#define _ILPLIB_NLP_WILLIAM_TRIE_H_

#include<iostream>
#include<stdio.h>
#include<string.h>
#include<string>
#include "types.h"
#include "util/string/kstring.hpp"
#include "knlp/normalize.h"

using namespace std;
using namespace izenelib;
using namespace izenelib::util;
using namespace ilplib::knlp;

namespace ilplib
{
	namespace knlp
	{
		class WilliamTrie
		{

			struct node{KString st;double value;};
			node dict[4000000];
			size_t dict_size;
			double MINVALUE;
			size_t tot_bi;
			size_t tot_len;
			size_t ch1[100001];

			public:
			WilliamTrie(string file_name)
			{
				for(size_t i = 0; i < 100000; ++i){ch1[i]=0;}
				tot_bi = 0;
				tot_len = 0;
				dict_size = 0;
				freopen(file_name.c_str(),"r",stdin);
				char st[1024];
				double value;
				while(scanf("%s\t%lf", st, &value)==2)
				{
					dict[dict_size].st = KString(st);
					ilplib::knlp::Normalize::normalize(dict[dict_size].st);
					ch1[dict[dict_size].st[0]] = 1;
					dict[dict_size++].value = value;
					if (!strcmp(st,"[MIN]")) MINVALUE = value;
				}
				sort(dict, dict+dict_size, WilliamTrie::cmp);
			}

			~WilliamTrie(){}

			static bool cmp(const node& x, const node& y)
			{
				return x.st < y.st;
			}

			size_t bisearch(size_t ind, uint16_t ch, size_t &p, size_t &q, double &value, const KString& k=KString(""))
			{
				++tot_bi;
				int head = p, tail = q, mid=0;
				uint16_t tmp;
				while(head <= tail)
				{
					mid = (head + tail) / 2;
					//if (ind == dict[mid].st.length())cout<<k<<"::::::::::::::::"<<dict[mid].st<<endl;        
					tmp = dict[mid].st[ind];
					if (tmp < ch)
					  head = mid + 1;
					else if (tmp == ch && (mid == (int)p || dict[mid-1].st[ind] != ch))
					{
						p = mid;
						break;
					}
					else
					{
						tail = mid - 1;
						if (tmp > ch) 
						  q = mid - 1;
					}
				}
				if (dict[mid].st[ind] != ch)
				  return 0;

				if (p==q || dict[p+1].st[ind] != ch)
				{
					value = dict[p].value;
					return 3;
				}

				++tot_bi;
				head = p; tail = q;
				while(head <= tail)
				{
					mid = (head + tail) / 2;
					tmp = dict[mid].st[ind];
					if (tmp > ch)
					  tail = mid - 1;
					else if (tmp == ch && (mid == (int)q || dict[mid+1].st[ind] != ch))
					{
						q = mid;
						break;
					}
					else
					  head = mid + 1;
				}
				if (dict[p].st.length() == ind+1)
				{
					value = dict[p].value;
					if (dict[++p].st[ind] != ch)
					  return 3;
					return 2;
				}
				else
				  return 1;
			}



			vector<pair<KString, double> > token(const KString st)
			{    
				size_t i = 0;
				size_t len = st.length();
				tot_len += len;    
				vector<pair<KString, double> > term;
				term.reserve(len);
				while(i < len)
				{
					size_t maxlen = 0, p = 0, q = dict_size - 1, flag = 0;
					double value = 0;
					//        while(i < len && st[i] == ' ') ++i;

					for (size_t j = 0; j < len - i; ++j)
					{
						if (st[j] == ' ') break;
						if (j == 0 && !ch1[st[i+j]]) 
						  break;
						else
						  flag = bisearch(j, st[i+j], p, q, value, st);        
						if (flag == 0)
						  break;
						else if (flag == 3)
						{
							maxlen = j;
							break;
						}
						else if (flag == 2)
						{
							maxlen = j;
						}
					}

					if (!maxlen)
					{
						term.push_back(make_pair(st.substr(i, 1), MINVALUE));
						i += 1;
					}
					else
					{    
						term.push_back(make_pair(st.substr(i, maxlen + 1), value));
						i += maxlen + 1;
					}
				}
				return term;
			}

			bool check_term(const KString st)const
			{
				size_t len = st.length();
				size_t maxlen = 0, p = 0, q = dict_size - 1, flag = 0;
				double value = 0;

				for (size_t j = 0; j < len; ++j)
				{
					flag = bisearch(j, st[j], p, q, value);        
					if (flag == 0 || flag == 3)
					  return 0;
					else if (flag == 2)
					  maxlen = j;
				}
				if (maxlen == len - 1)
				  return 1;
				else
				  return 0;
			}

			double score(const KString st)const
			{

				size_t len = st.length();
				size_t maxlen = 0, p = 0, q = dict_size - 1, flag = 0;
				double value = 0;

				for (size_t j = 0; j < len; ++j)
				{
					flag = bisearch(j, st[j], p, q, value, st);        
					if (flag == 0||flag == 3)
					  return MINVALUE;
					else if (flag == 2)
					  maxlen = j;
				}
				if (maxlen == len - 1)
				  return value;
				else
				  return MINVALUE;
			}

			size_t size() const
			{
				return dict_size;
			}

			double min() const
			{
				return MINVALUE;
			}

			size_t tot() const
			{
				return tot_bi;
			}

			size_t totlen() const
			{
				return tot_len;
			}

		};
	}
}
#endif

