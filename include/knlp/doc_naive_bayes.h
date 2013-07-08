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
			  classify_multi_level(
			    DigitalDictionary* cat,
			    DigitalDictionary* t2c,
			    Dictionary* t2cs,
			    std::vector<std::pair<KString,double> > v, std::stringstream& ss)
              {
                  double sum = 0;
                  for (uint32_t j=0;j<v.size();++j)
                      sum += v[j].second;
                  for (uint32_t j=0;j<v.size();++j)
                  {
                      v[j].second /= sum;
                      ss<<v[j].first<<":"<<v[j].second<<" ";
                  }
                  ss<<"\n";

                  std::vector<std::map<KString, double> >  cates(5, std::map<KString, double>());
                  for (uint32_t j=0;j<v.size();++j)
                  {
                      char* c = t2cs->value(v[j].first, false);
                      if(!c)continue;
                      KString ct(c);
                      vector<KString> kv = ct.split(' ');
                      for (uint32_t i=0;i<kv.size();++i)
                      {
                          uint32_t lev = (uint32_t)(kv[i][kv[i].length()-1]-'0'-1);
                          if (lev == (uint32_t)-1)continue;
                          kv[i] = kv[i].substr(0, kv[i].length()-1);
                          if(cates[lev].find(kv[i])== cates[lev].end())
                          {
                              double c = cat->value(kv[i], true);
                              if (c == (double)std::numeric_limits<int>::min())continue;
                              cates[lev][kv[i]] = c;
                          }
                      }
                  }

                  for (uint32_t i=1;i<cates.size();++i)
                      if (cates[i-1].size()==0 && cates[i].size()>0)
                          std::cout<<"[ERROR]: "<<cates[i].begin()->first<<", category level didn't align\n";
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
                                    || f < (3-i)*4)
                                      f = 0;

                                  //if (f/pt[j] < 0.004)
                                  //    it->second += -100*v[j].second;
                                  //else
                                  //    it->second += ((f+1)*10000000/pt[j]/(c+10000))*v[j].second;
                                  it->second += log((f+1)/(c+100000));
                                  ss<<v[j].first<<">>>"<<it->first<<" ptc="<<f<<", pt="<<pt[j]<<", c="<<c<<", r="<<it->second<<std::endl;
                              }
                              it->second += log((c+100000)/(3000000));
                              //it->second = it->second*10+(c+100000)/(200000);
                              if (it->second > max)
                                  max = it->second,maxv=it->first;
                          }
                          else it->second = 0;
                      if (max > (double)std::numeric_limits<int>::min())
                      {
                          lastc = maxv;
                          ss << "max: "<<maxv<<"="<<max<<std::endl;
                          lasti = i;
                      }
                  }

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
/*
./merge_cates.sh taobao_json.out.1 |\
gawk -F"\t" '
{
    if (index($2, "数码>手机>") == 0 && index($2, "数码>笔记本>") == 0 && index($2, "数码>平板电脑>") == 0)print;
    else{
        for (i=0;i<2;++i){
            if(index($2,"数码>手机>Apple@苹果")==0)print;
            else
                if(index($1,"苹果")>0||index($1,"Apple")>0)print
        }
    }
}'|sed -e 's/【[^【】]\+】//g' -e 's/送[^ ]\+ / /g'> taobao_json.out.1.bk

./fill_naive_bayes etao.term nb taobao_json.out.1.bk > taobao_json.out.2
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
        if (L < 2)continue;
        R = "R";
        for (i=2;i<L;++i)
           R = R">"ca[i];
        if (N[AB"\t"R] < 150)continue;
        p = N[k]*1.0/N[AB"\t"R]#if ( p== 1)print k":"R"="N[k]"="N[AB"\t"R];
        ent[AB"\t"R] -= p*log(p);
    }
    for (e in ent)
    {
        split(e, a, "\t");
        AB = a[1];
        if (!(AB in W))W[AB] = ent[e];
        else W[AB] = min(W[AB], ent[e]);
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
awk -F"\t" 'NR==FNR{a[$1]=1}NR>FNR{if($1 in a)print}' etao.term $CORPUS.termweight > etao.term.bk
./fill_naive_bayes etao.term.bk nb taobao_json.out.1.bk > $CORPUS;
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
{
    split($2, ca, ">");
    L = length(ca)
    R = "R";N[$1" "R]++;
    for (i=1;i<=L;i++)
    {
        R=R">"ca[i];
        #N[$1" "R]++;
        N[$1" "R] += $3;
    }
}
END{
    for (k in N)
        print k"\t"N[k]
}
' $CORPUS > $CORPUS.term.cat;
sort -t ' ' -k1,1  $CORPUS.term.cat| \
gawk -F'[\t ]' '
function level(ca)
{
    g = gsub(">", ">", ca);
    return g;
}
{
    if (last != $1 && length(last) > 0)
    {
        tc[last] = cats;
        last = $1
        cats = $2""level($2);
    }else{
        last = $1
        cats = cats" "$2""level($2);
    }
}
END{
    tc[last] = cats;
    for (t in tc)
        print t"\t"tc[t]
}'  > $CORPUS.term.cats
 * */
