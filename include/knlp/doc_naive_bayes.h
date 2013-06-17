/*
 * =====================================================================================
 *
 *       Filename:  doc_naive_bayes.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013年05月24日 15时11分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), kevin.hu@b5m.com
 *        Company:  B5M.com
 *
 * =====================================================================================
 */

#ifndef _ILPLIB_NLP_DOC_NAIVE_BAYES_H_
#define _ILPLIB_NLP_DOC_NAIVE_BAYES_H_

#include <string>
#include <vector>
#include <list>
#include <cstdlib>
#include <algorithm>
#include <istream>
#include <ostream>
#include <limits>

#include "types.h"
#include "am/hashtable/khash_table.hpp"
#include "util/string/kstring.hpp"
#include "normalize.h"
#include "knlp/string_patterns.h"

using namespace izenelib::util;
using namespace izenelib::am;
using namespace izenelib::am::util;
using namespace izenelib;

namespace ilplib
{
	namespace knlp
	{
		class DocNaiveBayes
		{

			ilplib::knlp::Tokenize* tkn_;
			KIntegerHashTable<uint32_t, double> cate_;
			KStringHashTable<KString, double> t2c_;
			public:
			DocNaiveBayes(ilplib::knlp::Tokenize* tkn, const std::string& nm)
				:tkn_(tkn)
			{
				cate_.load(nm+".cate");
				t2c_.load(nm+".t2c");
			}

			template <
				class CATE_T
				>
			double  classify(const KString& d, const CATE_T& c)
			{
				KString doc =d;
				ilplib::knlp::Normalize::normalize(doc);
				double* s = cate_.find(category_id(c));
				if (!s)
				  return std::numeric_limits<double>::min();

				std::set<KString> tks = normalize_tokens(tkn_->fmm(doc));
				double sc = (*s)*(int32_t)(tks.size()-1)*-1.;
				for (  std::set<KString>::iterator it=tks.begin(); it!=tks.end(); ++it)
				{
					KString kstr = concat(*it, c);
					s = t2c_.find(kstr);
					if (s == NULL)
					  sc += log(1.0/10000000);
					else sc += *s;
				}
				return sc;
			}

			template <
				class CATE_T
				>
			double  classify(std::vector<KString> v, const CATE_T& c)
			{
				double* s = cate_.find(category_id(c));
				if (!s)
				  return std::numeric_limits<double>::min();

				std::set<KString> tks = normalize_tokens(v);
				double sc = (*s)*(int32_t)(tks.size()-1)*-1.;
				for (  std::set<KString>::iterator it=tks.begin(); it!=tks.end(); ++it)
				{
					KString kstr = concat(*it, c);
					s = t2c_.find(kstr);
					if (s == NULL)
					  sc += log(1.0/10000000);
					else sc += *s;
				}
				return sc;
			}

			static void train(const std::string& dictnm, const std::string& output,
						const std::vector<std::string>& corpus, uint32_t cpu_num=8)
			{
				ilplib::knlp::Tokenize tkn(dictnm);
				EventQueue<std::pair<string*, string*> > in;
				EventQueue<std::pair<KString*, KString*> > out;
				std::vector<boost::thread*> token_ths;
				for ( uint32_t i=0; i<cpu_num; ++i)
				  token_ths.push_back(new boost::thread(&tokenize_stage, &in, &out, &tkn));

				uint32_t N = 0;
				KStringHashTable<KString, double> cates;
				KStringHashTable<KString, double> t2c;
				boost::thread cal_th(&calculate_stage, &out, &cates, &t2c);

					for ( uint32_t i=0; i<corpus.size(); ++i)
					{
						LineReader lr(corpus[i]);
						char* line = NULL;
						while((line=lr.line(line))!=NULL)
						{
							if (strlen(line) == 0)continue;
							char* t = strchr(line, '\t');
							if (!t)continue;
							t++;
							if (strlen(t) == 0)continue;

							N++;
							in.push(make_pair(new std::string(line, t-line-1), new string(t)), -1);
						}
					}
				for ( uint32_t i=0; i<cpu_num; ++i)
				  in.push(make_pair<string*,string*>(NULL, NULL), -1);
				for ( uint32_t i=0; i<cpu_num; ++i)
				  token_ths[i]->join(),delete token_ths[i];
				out.push(make_pair<KString*,KString*>(NULL, NULL), -1);
				cal_th.join();

				for(KStringHashTable<KString, double>::iterator it = t2c.begin(); it!=t2c.end(); ++it)
				  *it.value() = log((*it.value()+1)/(N+10000));
				for(KStringHashTable<KString, double>::iterator it = cates.begin(); it!=cates.end(); ++it)
				  *it.value() = log((*it.value()+1)/(N+100));

				t2c.persistence(output+".t2c");
				cates.persistence(output+".cate");
			}

			static std::set<KString> normalize_tokens(std::vector<KString>  v)
			{
				for ( uint32_t i=0; i<v.size(); i++)
				{
					int32_t ty = StringPatterns::string_type(v[i]);
					if (ty == 2)
					  v[i] = KString("[[NUMBERS]]");
					else if(ty == 3)
					  v[i] == KString("[[NUGLISH]]");
					else if (ty == 4)
					{
						v.erase(v.begin()+i);
						--i;
					}
				}
				return std::set<KString>(v.begin(), v.end());
			}

			static void tokenize_stage(EventQueue<std::pair<string*,string*> >* in, 
						EventQueue<std::pair<KString*,KString*> >* out, 
						ilplib::knlp::Tokenize* tkn)
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
					std::set<KString> s = normalize_tokens(tkn->fmm(KString(*t)));

					for ( std::set<KString>::iterator it=s.begin(); it!=s.end(); ++it)
					  out->push(make_pair(new KString(*it), new KString(c->c_str())), e);
					delete t, delete c;
				}
			}

			static KString concat(const KString& tk, const KString& ca)
			{
				uint32_t c = category_id(ca);
				KString r = tk;
				r += '\t';
				r += ((uint16_t*)(&c))[0];
				r += ((uint16_t*)(&c))[1];
				return r;
			}

			static KString concat(const KString& tk, uint32_t c)
			{
				KString r = tk;
				r += '\t';
				r += ((uint16_t*)(&c))[0];
				r += ((uint16_t*)(&c))[1];
				return r;
			}

			static uint32_t category_id(uint32_t ca)
			{
				return ca;
			}

			static uint32_t category_id(const KString& ca)
			{
				return izenelib::util::HashFunction<std::string>::generateHash32(ca.get_bytes("utf-8"));
			}

			static void calculate_stage(EventQueue<std::pair<KString*,KString*> >* out,
						KStringHashTable<KString, double>* Nc, 
						KStringHashTable<KString, double>* Ntc)
			{
				while(true)
				{
					uint64_t e = -1;
					std::pair<KString*,KString*> p(NULL,NULL);
					out->pop(p, e);
					if (p.first == NULL && p.second == NULL)
					  break;

					KString* t = p.first;
					KString* c = p.second;
					{
						//add Nc
						uint32_t cid = category_id(*c);
						double* f = Nc->find(cid);
						if (!f)
						  Nc->insert(cid, 1);
						else
						  (*f)++;
					}
					{//Nct
						double* f = Ntc->find(concat(*t, *c));
						if (!f)
						  Ntc->insert(*t,1);
						else (*f)++;
					}

					delete t, delete c;
				}
			}
		};
	}}
#endif

