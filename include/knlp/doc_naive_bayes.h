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

#include "types.h"
#include "am/hashtable/khash_table.hpp"
#include "util/string/kstring.hpp"
#include "normalize.h"

using namespace izenelib::util;
using namespace izenelib::am;
using namespace izenelib::am::util;

namespace ilplib
{
namespace knlp
{
class DocNaiveBayes
{

    ilplib::knlp::Tokenize* tkn_;
    KStringHashTable<KString, double> cate_;
    KStringHashTable<KString, double> t2c_;
    std::vector<KString> cates_;
    public:
        DocNaiveBayes(ilplib::knlp::Tokenize* tkn, const std::vector<std::string>& cates, const std::string& nm)
            :tkn_(tkn)
        {
            cate_.load(nm+".cate");
            t2c_.load(nm+".t2c");
            for ( uint32_t i=0; i<cates.size(); ++i)
            {
                cates_.push_back(KString(cates[i]));
                if (!cate_.find(cates_.back()))
                  throw std::runtime_error("Some category are not in the set.");
            }
        }

        std::vector<std::pair<double,KString> >
            classify(KString doc)
            {
                std::vector<std::pair<double,KString> > r;
                ilplib::knlp::Normalize::normalize(doc);
                std::vector<KString> tks = tkn_->tokenize(doc);
                for ( uint32_t i=0; i<cates_.size(); ++i)
                {
                    double* s = cate_.find(cates_[i]);
                    IASSERT(s);
                    double sc = *s;
                    for ( uint32_t j=0; j<tks.size(); ++j)
                    {
                        KString kstr = tks[j];
                        kstr += '\t';
                        kstr += cates_[i];
                        s = t2c_.find(cates_[i]);
                        if (s == NULL)
                        {}
                        sc += *s;
                    }
                    r.push_back(std::pair<double,KString>(-1*sc, cates_[i]));
                }
                std::sort(r.begin(), r.end());
                return r;
            }

        static void train(const std::string& dictnm, const std::string& output,
                    const std::vector<std::string>& corpus, uint32_t parrallel=8)
		{
			ilplib::knlp::Tokenize tkn(dictnm);
			KStringHashTable<KString, double> cate_c;
			uint32_t cc = 0;
			{
				std::ofstream of(output.c_str());

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
						KString cat(t);
						double* f = cate_c.find(cat);
						if(!f)
						{
							cate_c.insert(cat, 1.);
							cc ++;
							continue;
						}
						(*f)++;

						KString doc(std::string(line, t-line-1));
						ilplib::knlp::Normalize::normalize(doc);
						std::vector<KString> v = tkn.tokenize(doc);
						for ( uint32_t i=0; i<v.size(); ++i)
						  if (v[i].length() > 0 && v[i][0]!=' ')
							of << v[i] << "\t" << t<<std::endl;
					}
				}
			}

			KStringHashTable<KString, double> cate_c4tkn;
			uint32_t L = 0;
			{
				LineReader lr(output);
				char* line = NULL;
				while((line=lr.line(line))!=NULL)
				{
					if (strlen(line) == 0)continue;
					char* t = strchr(line, '\t');
					if (!t)continue;
					t++;
					if (strlen(t) == 0)continue;
					L++;
					KString cat(t);
					double* f = cate_c4tkn.find(cat);
					if(!f)
					{
						cate_c4tkn.insert(cat, 1.);
						continue;
					}
					(*f)++;
				}
			}

			KStringHashTable<KString, double> t2c(tkn.size()*cc*3, tkn.size()*cc+3);//p(word|cate)
			{
				LineReader lr(output);
				char* line = NULL;
				while((line=lr.line(line))!=NULL)
				{
					if (strlen(line) == 0)continue;
					char* t = strchr(line, '\t');
					if (!t)continue;
					t++;
					if (strlen(t) == 0)continue;
					KString cat(t);
					double* cf = cate_c4tkn.find(cat);
					IASSERT(cf);
					KString tkn(std::string(line, t-line-1));
					tkn += '\t';
					tkn += cat;
					double * f = t2c.find(tkn);
					if (!f)t2c.insert(tkn, 1./(*cf));
					else (*f) += 1./(*cf);
				}
			}

            for(KStringHashTable<KString, double>::iterator it = t2c.begin(); it!=cate_c.end(); ++it)
              *it.value() = log(*it.value());
            for(KStringHashTable<KString, double>::iterator it = cate_c.begin(); it!=cate_c.end(); ++it)
                *it.value() = log(*it.value()/cc);
            
            t2c.persistence(output+".t2c");
            cate_c.persistence(output+".cate");
        }
};
}}
#endif

