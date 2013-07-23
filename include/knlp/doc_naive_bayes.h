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
#include<time.h>
#include <functional>

#include "types.h"
#include "am/hashtable/khash_table.hpp"
#include "util/string/kstring.hpp"
#include "normalize.h"
#include "knlp/string_patterns.h"
#include "knlp/fmm.h"

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

            static void makeitclean(KString& kstr)
            {
                KString m1("【"), m2("】"), m3("送");
                uint32_t t = kstr.find(m1);
                if (t < kstr.length())
                {
                    uint32_t t2 = kstr.find(m2);
                    if (t2!=(uint32_t)-1 && t2+1 < kstr.length() && t2 > t)
                    {
                        KString kk;
                        if(t > 0)kstr.substr(0, t);
                        kk += kstr.substr(t2+1);
                        kstr = kk;
                    }
                }
                t = kstr.find(m3);
                if (t < kstr.length())
                {
                    KString k;
                    if (t > 0)k = kstr.substr(0, t);
                    uint32_t t2 = kstr.index_of(' ', t);
                    if (t2!=(uint32_t)-1 && t2+1 < kstr.length())
                         k += kstr.substr(t2+1);
                    if(k.length() > 0)kstr = k;
                }

            }

            static std::map<KString, double>
			  classify_multi_level(
			    VectorDictionary* t2cs,
			    const KString tk, std::stringstream& sss, bool dolog=false)
              {
                  if (dolog) sss <<"preclassify: "<< tk<<std::endl;
                  vector<char*>** cats = t2cs->value(tk, false);
                  if(!cats || (*cats)->size()%3 != 0)
                      return std::map<KString, double>();

                  vector<char*>* cts = *cats;
                  std::vector<KString> ct_nm;ct_nm.reserve(cts->size());
                  std::vector<double> ct_p;ct_p.reserve(cts->size());
                  std::vector<uint8_t> ct_lev;ct_lev.reserve(cts->size());
                  for (uint32_t i=0;i<cts->size();i+=3)if(strlen(cts->at(i))>0)
                  {
                      ct_p.push_back(atof(cts->at(i+2)));
                      ct_lev.push_back(cts->at(i+1)[0]-'0'-1);
                      ct_nm.push_back(KString(cts->at(i)));
                  }

                  KString lastc("R"), lastcc;
                  for (uint32_t t=0;t<3;++t)
                  {
                      double maxv = (double)std::numeric_limits<int>::min();
                      uint32_t maxi = -1;
                      for (uint32_t i=0;i<ct_nm.size();i++)
                          if (t == ct_lev[i] && maxv < ct_p[i] && ct_nm[i].find(lastc) == 0)
                              maxv = ct_p[i], maxi = i;
                      if(maxi < ct_nm.size())lastcc=lastc, lastc = ct_nm[maxi];
                  }
                  
                  if (lastcc == KString("R"))
                      lastcc = lastc.substr(0, lastc.length()-1);
                  std::map<KString, double> r;
                  for (uint32_t j=0;j<ct_nm.size();++j)
                      if (ct_nm[j].find(lastcc) == 0 && ct_nm[j].length() > lastcc.length())
                          r[ct_nm[j]] = (300.+ct_p[j])/300.;
                  
                  return r;
              }

            static std::map<KString, double>
			  classify_multi_level(
			    DigitalDictionary* cat,
			    VectorDictionary* t2cs,
			    VectorDictionary* pct,
			    std::vector<std::pair<KString,double> > v, std::stringstream& sss, bool dolog=false)
              {
                  //Fmm::gauss_smooth(v);
                  {
                      std::vector<std::pair<double,KString> > vv;
                      for (uint32_t j=0;j<v.size();++j)
                          vv.push_back(make_pair(v[j].second, v[j].first));
                      std::sort(vv.begin(),vv.end(), std::greater<std::pair<double,KString> >());
                      v.clear();
                      for (uint32_t j=0;j<vv.size();++j)
                          if(j==0 ||(j>0 && !(vv[j].second == vv[j-1].second)))
                              v.push_back(make_pair(vv[j].second, vv[j].first));
                      for (uint32_t T=4;T>=3;T--)
                      {
                          KString top3;
                          for (uint32_t j=0;j<v.size()&&j<T;++j)
                                  top3 += v[j].first;
                          std::map<KString, double> r = classify_multi_level(pct, top3, sss, dolog);
                          if (r.size())return r;
                          if (T >= v.size())break;
                      }
                  }
                  std::vector<KString> tks;
                  std::vector<double> sc;
                  uint32_t SCALE=7;
                  double sum = 0;
                  for (uint32_t j=0;j<v.size()&&j<SCALE;++j)
                      sum += v[j].second;
                  for (uint32_t j=0;j<v.size()&&j<SCALE;++j)
                  {
                      double f = v[j].second/sum;
                      if (tks.size() ==0 ||  f > 0.001)
                          tks.push_back(v[j].first),
                            sc.push_back(1);//f*SCALE);
                      if(dolog)sss<<v[j].first<<":"<<f<<" ";
                  }
                  if(dolog)sss<<"\n";
                  SCALE = tks.size();

                  izenelib::am::KStringHashTable<std::string, uint32_t> cat_dict(10000, 5000);
                  std::vector<KString> ct_nm;
                  std::vector<double> ct_p;
                  std::vector<uint8_t> ct_lev;
                  std::vector<uint32_t> ct_hit;
                  std::vector<double> ct_va;
                  std::vector<std::string> ct_log;
                  for (uint32_t j=0;j<tks.size();++j)
                  {
                      vector<char*>** cts = t2cs->value(tks[j], false);
                      if(!cts || (*cts)->size()%3 != 0)continue;
                      vector<char*>* cats = *cts;
                      for (uint32_t i=0;i<cats->size();i+=3)if(strlen(cats->at(i))>0)
                      {
                          double tc = atof(cats->at(i+2))*sc[j];
                          uint32_t* idx = cat_dict.find(cats->at(i));
                          if (!idx)
                          {
                              KString cnm(cats->at(i));
                              double c = cat->value(cnm, true);
                              if (c == (double)std::numeric_limits<int>::min())IASSERT(false);
                              ct_nm.push_back(cnm);
                              ct_p.push_back(c);
                              ct_lev.push_back(cats->at(i+1)[0]-'0'-1);
                              ct_hit.push_back(sc[j]);
                              ct_va.push_back(log((c+500000)/8000000)+tc);
                              cat_dict.insert(cats->at(i), ct_hit.size()-1);
                              if(dolog){stringstream ss;ss << tks[j].get_bytes("utf-8")<<":"<<tc;ct_log.push_back(ss.str());}
                              continue;
                          }
                          ct_va[*idx] += tc;
                          ct_hit[*idx] += sc[j];
                          //ct_hit[*idx] ++;
                          if(dolog){stringstream ss;ss <<"\t"<< tks[j].get_bytes("utf-8")<<":"<<tc;ct_log[*idx] += ss.str();}
                      }
                  }
                  for (uint32_t i=0;i<ct_hit.size();++i)
                  {
                      //double pen = log(1./(ct_p[i]*1.15 + 300000))*(tks.size()-ct_hit[i]);
                      double pen = log(1./(ct_p[i] + 3000000))*(SCALE-ct_hit[i]);
                      ct_va[i] += pen;
                      if(dolog)sss<<ct_nm[i]<<"\t"<<ct_va[i]<<"\t"<<ct_log[i]<<"\tpen="<<pen<<"="
                          <<log(1./(ct_p[i] + 3000000))<<"*"<<SCALE-ct_hit[i]<<"+"<<log((ct_p[i]+500000)/8000000)<<std::endl;
                  }
                  //std::cout<<tks.size()<<":"<<ct_va.size()<<"-"<<o<<"FFFFFFFFFFFFFF\n"; 

                  KString lastc("R");uint32_t lasti = -1;
                  KString lastcc;
                  for (uint32_t i=0;i<3;++i)
                  {
                      double maxv = (double)std::numeric_limits<int>::min();
                      uint32_t maxi = -1;
                      for (uint32_t j=0;j<ct_nm.size();++j)
                      {
                          //cout<<i<<":"<<ct_nm[j]<<" "<<ct_va[j]<<" "<<(int)ct_lev[j]<<" "<<maxv<<std::endl;
                          if (i == (uint32_t)ct_lev[j] && maxv < ct_va[j]
                            && ct_nm[j].find(lastc) == 0 && ct_nm[j].length() > lastc.length())
                              maxv = ct_va[j], maxi = j;
                      }
                      if(maxi < ct_nm.size())lastcc=lastc, lastc = ct_nm[maxi], lasti=i;
                      //cout<<lastcc<<"=="<<lastc<<endl;
                  }

                  if (lastcc == KString("R"))
                      lastcc = lastc.substr(0, lastc.length()-1);
                  std::map<KString, double> r;
                  for (uint32_t j=0;j<ct_nm.size();++j)
                      if (ct_nm[j].find(lastcc) == 0 && ct_nm[j].length() > lastcc.length())
                          r[ct_nm[j]] = (300.+ct_va[j])/300.;
                  
                  return r;
              }

            
            static std::map<KString, double>
			  classify_multi_level(
			    DigitalDictionary* cat,
			    DigitalDictionary* t2c,
			    Dictionary* t2cs,
			    std::vector<std::pair<KString,double> > v, std::stringstream& ss, bool dolog=false)
              {
                  timeval start, end; 
                  double sum = 0;
                  for (uint32_t j=0;j<v.size();++j)
                      sum += v[j].second;
                  for (uint32_t j=0;j<v.size();++j)
                  {
                      v[j].second /= sum;
                      if(dolog)ss<<v[j].first<<":"<<v[j].second<<" ";
                  }
                  if(dolog)ss<<"\n";

                  gettimeofday(&start, NULL); 
                  std::vector<std::map<KString, double> >  cates(5, std::map<KString, double>());
                  for (uint32_t j=0;j<v.size();++j)
                  {
                      char* c = t2cs->value(v[j].first, false);
                      if(!c)continue;
                      KString ct(c);
                      vector<KString> kv = ct.split(' ');
                      for (uint32_t i=0;i<kv.size();++i)if(kv[i].length()>0)
                      {
                          uint32_t lev = (uint32_t)(kv[i][kv[i].length()-1]-'0'-1);
                          if (lev >= cates.size())continue;
                          kv[i] = kv[i].substr(0, kv[i].length()-1);
                          if(cates[lev].find(kv[i])== cates[lev].end())
                          {
                              double c = cat->value(kv[i], true);
                              if (c == (double)std::numeric_limits<int>::min())continue;
                              cates[lev][kv[i]] = c;
                          }
                      }
                  }
                  gettimeofday(&end, NULL); 
                  //std::cout<<"[TIME]:"<<1000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000<<" ms\n";

                  gettimeofday(&start, NULL); 
                  for (uint32_t i=1;i<cates.size();++i)
                      if (cates[i-1].size()==0 && cates[i].size()>0)
                      {
                          std::cout<<"[ERROR]: "<<i<<cates[i].begin()->first<<", category level didn't align\n";
                          for (uint32_t j=0;j<v.size();++j)
                              std::cout<<v[j].first<<"\t";std::cout<<std::endl;
                      }
                  KString lastc("R");uint32_t lasti = -1;
                  for (uint32_t i=0;i<cates.size();++i)
                      if (cates[i].size() == 0)break;
                      else
                  {
                      std::vector<double> pt;pt.reserve(v.size());
                      for(uint32_t j=0;j<v.size();++j)
                      {
                          KString k = v[j].first;
                          k += ' ';
                          k += lastc;
                          double f = t2c->value(k, true);
                          if (f == (double)std::numeric_limits<int>::min())
                              f = 0;
                          pt.push_back(f);
                      }

                      double max = (double)std::numeric_limits<int>::min();KString maxv;
                      for (std::map<KString, double>::iterator it=cates[i].begin(); it!=cates[i].end();++it)
                          if (it->first.find(lastc) == 0)
                          {
                              if(dolog)ss <<"【"<< it->first<<"】\t";
                              double c = it->second;
                              it->second = 0;
                              for (uint32_t j=0;j<v.size();++j)
                                  if (pt[j] > 20)
                              {
                                  KString k = v[j].first;
                                  k += ' ';
                                  k += it->first;
                                  double f = t2c->value(k, true);
                                  if (f == (double)std::numeric_limits<int>::min()
                                    || f < (3-i)*3)
                                      f = 0;

                                  it->second += log((f+10)/(c+1000000));
                                  if(dolog)ss<<v[j].first<<"{"<<f<<", "<<pt[j]<<", "<<c<<", "<<it->second<<"}\t";
                              }
                              it->second += log(c+100000/30000000);//(c+100000)/(3000000));
                              if (it->second > max)
                                  max = it->second,maxv=it->first;
                              if(dolog)ss <<it->second<<std::endl;
                          }
                          else it->second = 0;
                      if (max > (double)std::numeric_limits<int>::min())
                      {
                          lastc = maxv;
                          if(dolog)ss << "max: "<<maxv<<"="<<max<<std::endl;
                          lasti = i;
                      }
                  }
                  gettimeofday(&end, NULL); 
                  //std::cout<<"[TIME]:"<<1000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000<<" ms\n";

                  std::map<KString, double> r;
                  if (lasti >= cates.size())return r;
                  for (std::map<KString, double>::iterator it=cates[lasti].begin(); it!=cates[lasti].end();++it)
                      if (it->second != 0)r[it->first] = (200.+it->second)/200.;
                  return r;
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
                if (!find && tc < 0.3 && t > BASE*2)
                {
                    r = -1*(c*t)/tc/BASE;
                }else{
                    r = tc/(c*t);
                    if (find) r = r *2;
                }
                r = r * (c+100000)/(300000);
                ss<<tk<<"==>"<<ca<<": tc="<<tc<<" c="<<c<<" t="<<t<<" r="<<r<<std::endl;
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
						const std::vector<std::string>& corpus, bool bigterm=true, uint32_t cpu_num=11)
			{
				ilplib::knlp::Fmm tkn(dictnm);
                ilplib::knlp::GarbagePattern gp(dictnm+".garbage");
				EventQueue<std::pair<string*, string*> > in;
				EventQueue<std::pair<KString*, double> > out;
				std::vector<boost::thread*> token_ths;
				for ( uint32_t i=0; i<cpu_num; ++i)
				  token_ths.push_back(new boost::thread(&tokenize_stage, &in, &out, &tkn, bigterm));

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
							in.push(make_pair(new std::string(gp.clean(std::string(line, t-line-1)).c_str()), 
							     new string(t)), -1);
							//in.push(make_pair(new std::string(line, t-line-1),new string(t)), -1);
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
				/*for ( uint32_t i=0; i<v.size(); i++)
				{
					int32_t ty = StringPatterns::string_type(v[i].first);
					if (ty == 2 && v[i].first.length() > 1)
					  ;//v[i].first = KString("[[NUMBERS]]");
					else if(ty == 3 && v[i].first.length() > 1)
					  ;//v[i].first == KString("[[NUGLISH]]");
					else if (v[i].first.length()==0
					  ||ty == 4 || (v[i].first.length()<=1 && (ty == 2||ty == 3)))
					{
						v.erase(v.begin()+i);
						--i;
					}
				}*/

				double s = 0;
				for ( uint32_t i=0; i<v.size(); i++)
				    s += v[i].second;
				for ( uint32_t i=0; i<v.size(); i++)
                {
                    if (v[i].second <= 0)std::cout<<"SSSSSSSSS"<<v[i].first<<"::::::::::\n";
                    assert( v[i].second > 0);
				    v[i].second /= s;
                }
				return std::set<std::pair<KString,double> > (v.begin(), v.end());
			}

            static int cmp_pair(const std::pair<KString,double>& a, const std::pair<KString,double>& b)
            {
                if (a.second > b.second)
                    return 1;
                return 0;
            }

			static void tokenize_stage(EventQueue<std::pair<string*,string*> >* in, 
						EventQueue<std::pair<KString*,double> >* out, 
						ilplib::knlp::Fmm* tkn, bool bigterm=true)
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
                        //cout<<"[Exception]: "<<*t<<std::endl;
                        delete t, delete c;
                        continue;
                    }
                    KString ca(*c);
                    KString ti(*t);
                    std::vector<std::pair<KString,double> >  v;
                    tkn->fmm(ti, v);
                    //std::set<std::pair<KString,double> > s = normalize_tokens(v);

                    if (bigterm)
                    {
                        std::sort(v.begin(), v.end(), DocNaiveBayes::cmp_pair);
                        for (uint32_t i=1;i < v.size();++i)
                            if (v[i].first == v[i-1].first 
                              || (v[i].first.length()==1 
                                  &&(KString::is_english(v[i].first[0]) ||KString::is_numeric(v[i].first[0]))))
                                v.erase(v.begin()+i), --i;
                        /*for (std::set<std::pair<KString,double> >::iterator it=s.begin();it!=s.end();++it)
                        {
                            assert(it->second > 0);
                            v.push_back(make_pair(it->second,it->first));
                        }
                        std::sort(v.begin(), v.end(), std::greater<std::pair<double,KString> >());*/
                        static const uint32_t WIDTH = 4;
                        for (uint32_t f = 0;f<WIDTH;f++)
                            for (uint32_t t=f;t<WIDTH && t<v.size();t++)
                        {
                            KString* k=  new KString("=<>=");
                            for (uint32_t i=f;i<=t;++i)
                                (*k) += v[i].first;
                            (*k) += '|';
                            (*k) += ca;
                            out->push(make_pair(k, v[f].second), e);
                        }
                    }

                    /*for (std::set<std::pair<KString,double> >::iterator it=s.begin();it!=s.end();++it)
                    {
                        assert(it->second > 0);
                        KString k = it->first;k += '|';k+=ca;
                        out->push(make_pair(new KString(k.get_bytes(), k.get_bytes()+k.length()), it->second), e);
                    }*/
                    for (uint32_t i=0;i < v.size();++i)
                    {
                        assert(v[i].second > 0);
                        KString k = v[i].first;k += '|';k+=ca;
                        out->push(make_pair(new KString(k.get_bytes(), k.get_bytes()+k.length()), v[i].second), e);
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

                    std::vector<KString> ct = p.first->split('|');
					double  s = p.second;
					if(ct.size() == 2 && ct[0].length() > 0 && ct[1].length() > 0 )
					    printf("%s\t%s\t%.5f\n", ct[0].get_bytes("utf-8").c_str(),  ct[1].get_bytes("utf-8").c_str(), s);
					delete p.first;
					continue;

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
				}
			}
		};
	}}
#endif
/*
./merge_cates.sh taobao_json.out.1 > taobao_json.out.1.bkk
gawk -F"\t" '
{
    if (index($2, "数码>手机>") == 0 && index($2, "数码>笔记本>") == 0 && index($2, "数码>平板电脑>") == 0)print;
    else{
        if(index($2,"数码>手机>Apple@苹果")==0)print;
        else if(index($1,"苹果")>0||index($1,"Apple")>0)print;
        if (index($2,"数码>数码配件>手机配件")!=0)
        {
            N++;
            if (N % 2 == 0)print;
        }
    }
}' taobao_json.out.1.bkk |sed -e 's/【[^【】]\+】//g' -e 's/[送赠][^ ]\+ / /g' -e 's/[送赠][^ ]\+$//g'> taobao_json.out.1.bk
export TOKEN_DICT="./brand.term"
#rm "$TOKEN_DICT.prefix"
./fill_naive_bayes $TOKEN_DICT nb taobao_json.out.1.bk|awk -F"[\t 　]" '{if(NF==3)print}' > taobao_json.out.2
export CORPUS="./taobao_json.out.2";
gawk -F"\t" '{
    split($2, ca, ">");
    L = length(ca)
    R = "R";N[$1"\t"R]++;
    for (i=1;i<=L;i++)
    {
        R=R">"ca[i];
        N[$1"\t"R]++;
    }
}
function min(a, b){ if (a < b)return a; else return b;}
END{
    for(k in N)
    {
        split(k, f, "\t")
        AB = f[1];cate=f[2];
        split(cate, ca, ">")
        L = length(ca)
        if (L == 1)continue;
        R = "R";
        for (i=2;i<L;++i)
           R = R">"ca[i];
        if (N[AB"\t"R] < 10)continue;
        p = N[k]*1.0/N[AB"\t"R]#if ( p== 1)print k":"R"="N[k]"="N[AB"\t"R];
        ent[AB"\t"R] -= p*log(p);
    }
    for (e in ent)
    {
        split(e, a, "\t");
        AB = a[1];
        L = gsub(">",">", a[2]);
        if (!(AB in W))W[AB] = ent[e]+(L-1)*0.8;
        else W[AB] = min(W[AB], ent[e]+(L-1)*0.8);
    }
    m = 10000000
    for (k in W)
    {
        v = 10**(4-W[k])
        if (m > v)m = v;
        print k"\t"v;
    }
    print "[MIN]\t"m/10
}
' $CORPUS > $CORPUS.termweight;
awk -F"\t" '
NR==FNR{a[$1]=1}
NR>FNR{b[$1]=$2}
END{for(i in a)if(i in b)print i"\t"b[i];else{print i"\t"b["[MIN]"]*5}}' "$TOKEN_DICT" $CORPUS.termweight > "$TOKEN_DICT.bk"
./fill_naive_bayes "$TOKEN_DICT.bk" nb taobao_json.out.1.bk|awk -F"[\t 　]" '{if(NF==3)print}' > $CORPUS;
gawk -F"\t" '
{
    split($2, ca, ">");
    L = length(ca)
    R = "R";
    for (i=1;i<=L;i++)
    {
        R=R">"ca[i];
        #N[R]++;
        N[R] += $3;
    }
}
END{
    for (k in N)
        print k"\t"N[k]
}
' $CORPUS > $CORPUS.cat;
gawk -F"\t" '
function level(ca)
{
    g = gsub(">", ">", ca);
    return g;
}
{
    split($2, ca, ">");
    L = length(ca)
    R = "R";#N[$1" "R]++;
    for (i=1;i<=L;i++)
    {
        R=R">"ca[i];
        #N[$1" "R]++;
        N[$1" "R] += $3;
        ca[R] += $3;
    }
}
END{
    for (k in N)
    {
        split(k, ar, " ");
        if (ar[1] in tc)tc[ar[1]]=tc[ar[1]]"\t"
        tc[ar[1]]=tc[ar[1]]""ar[2]"\t"level(ar[2])"\t"log((N[k]*0.9+1)/(ca[ar[2]]*1.15+300000));
        #print k"\t"N[k]
    }
    for (t in tc)
        print t"\t"tc[t]
}
' $CORPUS > $CORPUS.term.cats;

gawk -F"\t" '
function level(ca)
{
    g = gsub(">", ">", ca);
    return g;
}
{
    split($2, ca, ">");
    L = length(ca)
    R = "R";
    N[$1"\t"R]+=$3;
    for (i=1;i<=L;i++)
    {
        R=R">"ca[i];
        N[$1"\t"R] += $3;
    }
}
END{
    for (k in N)
    {
        split(k, ar, "\t");
        split(ar[2], ca, ">");
        L = length(ca);
        if (L < 1)
            continue;
        R = "R";
        for(i=2;i<L;i++)
            R = R">"ca[i];

        if (ar[1] in tc)tc[ar[1]]=tc[ar[1]]"\t"
        tc[ar[1]]=tc[ar[1]]""ar[2]"\t"level(ar[2])"\t"log((N[k]+1)/(N[ar[1]"\t"R+300]));
        #print k"\t"N[k]
    }
    for (t in tc)
        print t"\t"tc[t]
}
' $CORPUS > $CORPUS.bigterm.cats;

sort  $CORPUS.term.cat| \
gawk -F'[\t ]' '
function level(ca)
{
    g = gsub(">", ">", ca);
    return g;
}
{
    ca[$2] += $3;
    if (last != $1 && length(last) > 0)
    {
        tc[last] = cats;
        last = $1
        #cats = $2""level($2)
        cats = $2"|"level($2)"|"$3;
    }else{
        last = $1
        if (length(cats)>0)cats=cats" "
        cats = cats""$2"|"level($2)"|"$3;
        #cats = cats" "$2""level($2)
    }
}
END{
    tc[last] = cats;
    for (t in tc)
    {
        #print t"\t"tc[t]
        split(tc[t], ar, " ")
        va = "";
        for (i=1;i<=length(ar);++i)
        {
            split(ar[i], arr, "|")
            if(length(va)>0)va=va"\t";
            va = va""arr[1]"\t"arr[2]"\t"log((arr[3]*0.8+2)/(ca[arr[1]]*1.15+300000)
        }
        print t"\t"va
    }
}'  > $CORPUS.term.cats
 * */
