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
			double  classify(const KString& d, const CATE_T& ca)
			{
				KString doc =d;
				ilplib::knlp::Normalize::normalize(doc);
                std::cout<<category_id(ca)<<std::endl;
				double* c = cate_.find(category_id(ca));
				if (!c)
				  return (double)std::numeric_limits<int>::min();

                std::vector<std::pair<KString,double> > v;
                tkn_->fmm(doc, v);
				std::set<std::pair<KString,double> > tks = normalize_tokens(v);
				double sc = *c;//(*s)*tks.size()*-1.;
				for (  std::set<std::pair<KString,double> >::iterator it=tks.begin(); it!=tks.end(); ++it)
				{
					KString kstr = concat(it->first, ca);
					double* s = t2c_.find(kstr);
                    if(s)std::cout<<*s<<":"<<*c<<";;;;;;;\n";
					if (s == NULL)
					  sc += (-100-*c)*it->second*tks.size()*5;
					else sc += (*s-*c)*it->second*tks.size()*5;
				}
				return sc;
			}

			template <
				class CATE_T
				>
			double  classify(std::vector<std::pair<KString,double> > v, const CATE_T& ca)
			{
				double* c = cate_.find(category_id(ca));
				if (!c)
				  return (double)std::numeric_limits<int>::min();

                std::set<std::pair<KString,double> > tks = normalize_tokens(v);
				double sc = *c;//(*s)*tks.size()*-1.;
				for (  std::set<std::pair<KString,double> >::iterator it=tks.begin(); it!=tks.end(); ++it)
				{
					KString kstr = concat(it->first, ca);
					double* s = t2c_.find(kstr);
                    //if(s)std::cout<<*s<<";;;;;;;\n";
					if (s == NULL)
					  sc += (-100-*c)*it->second*tks.size()*5;
					else sc += (*s-*c)*it->second*tks.size()*5;
				}
				return sc;
			}

            static std::map<KString, double>
			  classify(
			    DigitalDictionary* cat,
			    DigitalDictionary* term,
			    DigitalDictionary* t2c,
			    Dictionary* t2cs,
			    std::vector<std::pair<KString,double> > v, std::stringstream& ss)
              {
                  static const double BASE = 1000;
                  double sum = 0;
                  for (uint32_t j=0;j<v.size();++j)
                      sum += v[j].second;
                  for (uint32_t j=0;j<v.size();++j)
                  {
                      v[j].second /= sum;
                      //v[j].second = sqrt(v[j].second);
                      ss<<v[j].first<<":"<<v[j].second<<" ";
                  }
                  ss<<"\n";

                  std::vector<double> pt;pt.reserve(v.size());
                  for(uint32_t i=0; i<v.size();++i)
                  {
                      double f = term->value(v[i].first, true);
                      if (v.size()>1 && (f < BASE+50 || v[i].second < 0.01))
                      {
                          v.erase(v.begin()+i);
                          --i;
                      }else
                          pt.push_back(f);
                  }
                  std::map<KString, double> m;
                  for (uint32_t j=0;j<v.size();++j)
                  {
                      char* c = t2cs->value(v[j].first, false);
                      if(!c)continue;
                      KString ct(c);
                      vector<KString> kv = ct.split(' ');
                      for (uint32_t i=0;i<kv.size();++i)
                          if(m.find(kv[i])== m.end())
                          {
                              double c = cat->value(kv[i], true);
                              if (c == (double)std::numeric_limits<int>::min())continue;
                              m[kv[i]] = c;
                              //std::cout<<v[j].first<<" "<<kv[i]<<std::endl;
                          }
                  }

                  for (std::map<KString, double>::iterator it=m.begin();it!=m.end();it++)
                  {
                      double c = it->second;
                      it->second = 0;
                      for (uint32_t j=0;j<v.size();++j)
                      {
                          KString k = v[j].first;
                          k += ' ';
                          k += it->first;
                          double f = t2c->value(k, true);
                          if (f == (double)std::numeric_limits<int>::min())
                              f = 0;
                          
                          it->second += relevance(v[j].first, it->first, f, c, pt[j],BASE, ss)*v[j].second;
                      }
                  }
                  return m;
              }

            static double relevance(const KString& tk, const KString& ca, 
              double tc, double c, double t, double BASE, std::stringstream& ss)
            {
                bool find = (ca.find(tk) != (uint32_t)-1);
                double r = 0;
                if (!find && tc < 0.1 && t > BASE*2)
                {
                    r = -1*(c*t)/tc/BASE;
                }else{
                    r = tc/(c*t);
                    if (find) r*=7;
                }
                ss<<tk<<"==>"<<ca<<":"<<tc<<" "<<c<<" "<<t<<" =="<<r<<std::endl;
                return r;
            }

			static std::map<KString, double>
			  classify(
			    DigitalDictionary* cat,
			    DigitalDictionary* t2c,
			    Dictionary* t2cs,
			    std::vector<std::pair<KString,double> > v)
              {
                  double sum = 0;
                  for (uint32_t j=0;j<v.size();++j)
                      sum += v[j].second;
                  const uint32_t TOTAL = v.size()*8;
                  for (uint32_t j=0;j<v.size();++j)
                  {
                      v[j].second /= sum;
                      v[j].second = (int)(v[j].second*TOTAL+0.5);
                      cout<<v[j].first<<":"<<v[j].second<<" ";
                  }
                  cout<<"\n";

                  std::map<KString, double> m;
                  for (uint32_t j=0;j<v.size();++j)
                  {
                      char* c = t2cs->value(v[j].first, false);
                      if(!c)continue;
                      KString ct(c);
                      vector<KString> kv = ct.split(' ');
                      for (uint32_t i=0;i<kv.size();++i)
                          if(m.find(kv[i])== m.end())
                          {
                              double c = cat->value(kv[i], true);
                              if (c == (double)std::numeric_limits<int>::min())continue;
                              m[kv[i]] = c;
                              //std::cout<<v[j].first<<" "<<kv[i]<<std::endl;
                          }
                  }

                  for (std::map<KString, double>::iterator it=m.begin();it!=m.end();it++)
                  {
                      double c = it->second;
                      for (uint32_t j=0;j<v.size();++j)
                      {
                          KString k = v[j].first;
                          k += ' ';
                          k += it->first;
                          double f = t2c->value(k, true);
                          if (f == (double)std::numeric_limits<int>::min())
                              it->second += (-50 - c)*v[j].second;//continue;
                          else
                          {
                              double h = (f-c)*v[j].second;
                              it->second += h;
                              std::cout<<v[j].first<<"xxxx"<<it->first<<":"<<f<<" "<<h<<"     "<<it->second<<":"<<c<<std::endl;
                          }
                      }
                  }
                  return m;
              }

			static void train(const std::string& dictnm, const std::string& output,
						const std::vector<std::string>& corpus, uint32_t cpu_num=11)
			{
				ilplib::knlp::Tokenize tkn(dictnm);
				EventQueue<std::pair<string*, string*> > in;
				EventQueue<std::pair<KString*, double> > out;
				std::vector<boost::thread*> token_ths;
				for ( uint32_t i=0; i<cpu_num; ++i)
				  token_ths.push_back(new boost::thread(&tokenize_stage, &in, &out, &tkn));

				uint32_t N = 0;
				KStringHashTable<KString, double> cates(2000, 1000);
				KStringHashTable<KString, double> t2c(tkn.size()*300, tkn.size()*100);
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
				out.push(make_pair<KString*,double>(NULL, 0), -1);
				cal_th.join();

				for(KStringHashTable<KString, double>::iterator it = t2c.begin(); it!=t2c.end(); ++it)
				  *it.value() = log((*it.value()+0.01)/(N+100));
				for(KStringHashTable<KString, double>::iterator it = cates.begin(); it!=cates.end(); ++it)
				  *it.value() = log((*it.value()+0.01)/(N+1));

				t2c.persistence(output+".t2c");
				cates.persistence(output+".cate");
			}

			static std::set<std::pair<KString,double> > normalize_tokens(std::vector<std::pair<KString,double> >  v)
			{
				for ( uint32_t i=0; i<v.size(); i++)
				{
					int32_t ty = StringPatterns::string_type(v[i].first);
					if (ty == 2)
					  v[i].first = KString("[[NUMBERS]]");
					else if(ty == 3)
					  v[i].first == KString("[[NUGLISH]]");
					else if (ty == 4)
					{
						v.erase(v.begin()+i);
						--i;
					}
				}

				double s = 0;
				for ( uint32_t i=0; i<v.size(); i++)
				    s += v[i].second;
				for ( uint32_t i=0; i<v.size(); i++)
				    v[i].second /= s;
				return std::set<std::pair<KString,double> > (v.begin(), v.end());
			}

			static void tokenize_stage(EventQueue<std::pair<string*,string*> >* in, 
						EventQueue<std::pair<KString*,double> >* out, 
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
					try{
                        ilplib::knlp::Normalize::normalize(*t);
                        ilplib::knlp::Normalize::normalize(*c);
                    }catch(...)
                    {
                        delete t, delete c;
                        continue;
                    }
                    KString ca(*c);
                    std::vector<std::pair<KString,double> >  v;
                    tkn->fmm(KString(*t), v);
                    std::set<std::pair<KString,double> > s = normalize_tokens(v);
                    for (std::set<std::pair<KString,double> >::iterator it=s.begin();it!=s.end();++it)
                    {
                        KString k = it->first;k += ' ';k+=ca;
                        out->push(make_pair(new KString(k.get_bytes(), k.get_bytes()+k.length()), it->second), e);
                    }

					delete t, delete c;
				}
			}

			static KString concat(const KString& tk, const KString& ca)
			{
				uint32_t c = category_id(ca);
				KString r = tk;
				r += '\t';
				uint16_t n = (uint16_t) c;
				r += n;
				c = c >> 16;
				n = (uint16_t) c;
				r += n;
				return r;
			}

			static KString concat(const KString& tk, uint32_t c)
			{
				KString r = tk;
				r += '\t';
				uint16_t n = (uint16_t) c;
				r += n;
				c = c >> 16;
				n = (uint16_t) c;
				r += n;
				return r;
			}

			static uint32_t category_id(uint32_t ca)
			{
				return ca;
			}

			static uint32_t category_id(const KString& ca)
			{
			    uint32_t t = -1;//ca.find('>');
				return izenelib::util::HashFunction<std::string>::generateHash32(ca.substr(0, t).get_bytes("utf-8"));
			}

			static void calculate_stage(EventQueue<std::pair<KString*,double> >* out,
						KStringHashTable<KString, double>* Nc, 
						KStringHashTable<KString, double>* Ntc)
			{
				while(true)
				{
					uint64_t e = -1;
					std::pair<KString*,double> p(NULL,0);
					out->pop(p, e);
					if (p.first == NULL)
					  break;

                    std::vector<KString> ct = p.first->split(' ');
					double  s = p.second;
					printf("%s\t%s\t%.5f\n", ct[0].get_bytes("utf-8").c_str(),  ct[1].get_bytes("utf-8").c_str(), s);

					{
						//add Nc
						uint32_t cid = category_id(ct[1]);
						double* f = Nc->find(cid);
						if (!f)
						  Nc->insert(cid, s);
						else
						  (*f) += s;
					}
					{//Nct
					    KString tt = concat(ct[0], ct[1]);
						double* f = Ntc->find(tt);
						if (!f)
						  Ntc->insert(tt, s);
						else (*f) += s;
					}
					
					delete p.first;
				}
			}
		};
	}}
#endif

