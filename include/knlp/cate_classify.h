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

#ifndef _ILPLIB_NLP_CATE_CLASSIFY_H_
#define _ILPLIB_NLP_CATE_CLASSIFY_H_

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
#include "util/string/kstring.hpp"
#include "normalize.h"
#include "knlp/datrie.h"

using namespace izenelib::util;
using namespace izenelib::am;
using namespace izenelib::am::util;
using namespace izenelib;

#define NOT_FOUND 0xFFFFFFFFFFFFFFFF

namespace ilplib
{
	namespace knlp
	{
	    class CateClassifyCateDict
        {
        private:
            typedef struct {KString kstr; double score, con1, con2;} cate_table_type;
            vector<cate_table_type> cate_table_;
            typedef struct {vector<size_t> child; size_t parent;} cate_graph_type;
            vector<cate_graph_type> cate_graph_;
            size_t size_;
        public:
            bool ischild(const size_t i, const size_t j)
            {
                const KString &kstr_i = cate_table_[i].kstr;
                const KString &kstr_j = cate_table_[j].kstr;
                size_t len_i = kstr_i.length(), len_j = kstr_j.length();
                if (len_i >= len_j)
                    return 0;
                if (kstr_i == kstr_j.substr(0,len_i) && kstr_j[len_i]=='>')
                {
                    for (size_t l = len_i + 1 ; l < len_j; ++l)
                        if (kstr_j[l]=='>')
                            return 0;
                    return 1;
                }
                return 0;
            }

            static bool cate_table_cmp(const cate_table_type& x, const cate_table_type& y)
            {
                return x.kstr < y.kstr;
            }

            CateClassifyCateDict(const std::string& file)
            {
                char* st = NULL;
                string s0, s1;
                cate_table_.clear();
                cate_table_.reserve(2000);
                LineReader lr(file.c_str());
                while((st = lr.line(st)) != NULL)
                {
                    s0 = string(st);
//                    Normalize::normalize(s0);
                    int p = s0.find("\t", 0);
                    if (p == -1) continue;
                    s1 = s0.substr(0, p);
                    cate_table_type tmp;
                    tmp.kstr = KString(s1);
//                    Normalize::normalize(tmp.kstr);
                    s1 = s0.substr(p+1, s0.length() - p);
                    tmp.score = atof(s1.c_str());
                    tmp.con1 = log((tmp.score + 500000) / 8000000);
                    tmp.con2 = log(1./(tmp.score + 300000));
                    cate_table_.push_back(tmp);
                }
                sort(cate_table_.begin(), cate_table_.end(), cate_table_cmp);

                cate_graph_.clear();
                cate_graph_.resize(cate_table_.size());
                for (size_t i = 0; i < cate_table_.size(); ++i)
                    cate_graph_[i].parent = 0;
                for (size_t i = 0; i < cate_table_.size(); ++i)
                    for (size_t j = 0; j < cate_table_.size(); ++j)
                    {
                        if (ischild(i,j))
                        {
                            cate_graph_[i].child.push_back(j);
                            cate_graph_[j].parent = i;
                        }
                    }
                
                size_ = cate_table_.size();
            }

            CateClassifyCateDict(const CateClassifyCateDict* c_dict)
            {
                *this = c_dict;
            }

            CateClassifyCateDict() {}

            ~CateClassifyCateDict() {}

            KString cate_trans(const size_t ind)
            {
                if (ind > size_) return KString("");
                return cate_table_[ind].kstr;
            }

            size_t cate_trans(const KString& kstr,size_t mode = 0)
            {
                int head = 0, tail = cate_table_.size() - 1, mid = 0;
                while (head <= tail)
                {
                    mid = (head+tail) / 2;
                    if (cate_table_[mid].kstr < kstr)
                        head = mid + 1;
                    else if (kstr < cate_table_[mid].kstr)
                        tail = mid - 1;
                    else 
                        break;
                }
                if (cate_table_[mid].kstr == kstr) 
                    return mid;
                else
                    return NOT_FOUND;
            }

            size_t size()
            {
                return size_;
            }

            size_t get_con1(const size_t i)
            {
                if (i > size_) return NOT_FOUND;
                return cate_table_[i].con1;
            }

            size_t get_con2(const size_t i)
            {
                if (i > size_) return NOT_FOUND;
                return cate_table_[i].con2;
            }

            size_t child_size(const size_t i)
            {
                if (i > size_) return NOT_FOUND;
                return cate_graph_[i].child.size();
            }

            size_t child(const size_t i, const size_t j)
            {
                if (i > size_) return NOT_FOUND;
                if (j > cate_graph_[i].child.size()) return NOT_FOUND;
                return cate_graph_[i].child[j];
            }

            size_t parent(const size_t i)
            {
                if (i > size_) return NOT_FOUND;
                return cate_graph_[i].parent;
            }

            KString get_kstr(const size_t i)
            {
                if (i > size_) return KString("");
                return cate_table_[i].kstr;
            }

            double get_score(const size_t i)
            {
                if (i > size_) return NOT_FOUND;
                return cate_table_[i].score;
            }


        };

		class CateClassifyScoreDict
		{
        private:
            typedef struct {size_t cate_ind; double score;} token_score_type;
            vector<vector<token_score_type> > token_score_;
            size_t size_;
            uint16_t first[65537];
            DATrie* DA_dict_;
            CateClassifyCateDict* c_dict_;
		public:
			CateClassifyScoreDict()
			{
            }

            CateClassifyScoreDict(const std::string& file1, CateClassifyCateDict* c_dict)
            {
                DA_dict_ = new DATrie(file1, 1);
                c_dict_ = c_dict;

			    for(size_t i = 0; i < 65537; ++i)first[i] = 0;
                char* st = NULL;
                size_t token_ind;
                string s0,s1;
                int p, p1;
size_t times = 0;
                token_score_.clear();
                token_score_.resize(DA_dict_->size());
                LineReader lr(file1.c_str());
                while((st = lr.line(st)) != NULL)//getline(cin,s0))
                {                    
//cout<<st<<endl;                    
                    s0 = string(st);
                    ++times;
//                    Normalize::normalize(s0);
                    s0 += "\t";
                    p = s0.find("\t", 0);
                    if (p == -1) continue;
                    s1 = s0.substr(0, p);
                    p1 = p + 1;
                    KString kstr(s1);
                    Normalize::normalize(kstr);
                    token_ind = token_trans(kstr);
                    if (token_ind == NOT_FOUND) 
                    {
                        cout<<"read error! "<<token_ind<<' '<<kstr<<endl;
                        continue;
                    }
                    token_score_[token_ind].clear();
                    token_score_[token_ind].reserve(64);
                    while(1)
                    {
                        token_score_type tmp;
                        p = s0.find("\t", p1);
                        if (p == -1) break;
                        s1 = s0.substr(p1, p - p1);
                        KString kstr(s1);
//                        Normalize::normalize(kstr);
                        tmp.cate_ind = c_dict_->cate_trans(kstr);
                        p1 = p + 1;

                        p = s0.find("\t", p1);
//                        s1 = s0.substr(p1, p - p1);
//                        tmp.level = atoi(s1.c_str());
                        p1 = p + 1;

                        p = s0.find("\t", p1);
                        s1 = s0.substr(p1, p - p1);
                        tmp.score = atof(s1.c_str());
                        p1 = p + 1;

                        token_score_[token_ind].push_back(tmp);
                    }
                    
                }
//cout<<"get big table ok\n";               
                size_ = token_score_.size();
			}

			~CateClassifyScoreDict() {}

            static void makeitclean(KString& kstr)
            {
                KString m1("【"), m2("】"), m3("送");
                uint32_t t = kstr.find(m1);
                if (t < kstr.length())
                {
                    uint32_t t2 = kstr.find(m2);
                    if (t2 < kstr.length() && t2 < t)
                    {
                        KString kk = kstr.substr(0, t);
                        kk += kstr.substr(t2+1);
                        kstr = kk;
                    }
                }
                t = kstr.find(m3);
                if (t < kstr.length())
                {
                    kstr = kstr.substr(0, t);
                    uint32_t t2 = kstr.index_of(' ', t);
                    if (t2 < kstr.length())
                         kstr = kstr.substr(t2+1);
                }

            }


            static bool token_dict_cmp(const KString& x, const KString& y)
            {
                return x < y;
            }


            size_t token_trans(const KString& kstr,size_t mode = 0)
            {                
                return DA_dict_->find_word(kstr);
            }

            size_t size()
            {
                return size_;
            }

            size_t size(const size_t i)
            {
                if (i > size_) return NOT_FOUND;
                return token_score_[i].size();
            }

            size_t get_ind(const size_t i, const size_t j)
            {
                if (i > size_) return NOT_FOUND;
                if (j > token_score_[i].size()) return NOT_FOUND;
                return token_score_[i][j].cate_ind;
            }

            double get_score(const size_t i, const size_t j)
            {
                if (i > size_) return NOT_FOUND;
                if (j > token_score_[i].size()) return NOT_FOUND;
                return token_score_[i][j].score;
            }


        };


        class CateClassify
        {
        private:
            CateClassifyScoreDict* dict1_;
            CateClassifyScoreDict* dict2_;
            CateClassifyCateDict* c_dict_;
            size_t tot_loop;
            size_t tot_term;
            typedef struct {size_t ind; double score;} cate_score_type;

        public:
            CateClassify() {}
            CateClassify(const std::string& file1, const std::string& file2, const std::string& file3 = "")
            {
                tot_loop = 0;
                tot_term = 0;
                c_dict_ = new CateClassifyCateDict(file2);
                dict1_ = new CateClassifyScoreDict(file1, c_dict_);
                if (file3 != "") dict2_ = new CateClassifyScoreDict(file3, c_dict_);
            }
            
            ~CateClassify() {}

            size_t get_loop()
            {
                return tot_loop;
            }

            size_t get_term()
            {
                return tot_term;
            }

            std::map<KString, double> classify_stage_1(
              const KString& kstr, stringstream& ss, bool dolog = false)
            {
                std::map<KString, double> res_map;
                  std::pair<KString, double> res;                
                  if (kstr == "" || dict2_ == NULL)
                      return res_map;
                  size_t token_ind = 0;
                  size_t cate_size = c_dict_->size();
                  std::vector<double> cate_score;
                  cate_score.resize(cate_size);
                  std::vector<size_t> hit;
                  hit.resize(cate_size);

//                      if (kstr.length() == 0) continue;
                  token_ind = dict2_->token_trans(kstr);
                  if (token_ind == NOT_FOUND)
                      return res_map;
                  size_t token_size = dict2_->size(token_ind);
++tot_term;                    
                  for (size_t j = 0; j < token_size; ++j)
                  {
++tot_loop;
                      cate_score[dict2_->get_ind(token_ind, j)] += dict2_->get_score(token_ind, j);
                      ++hit[dict2_->get_ind(token_ind, j)];
                  }
                      
                  
                  for (size_t i = 0; i < cate_size; ++i) if (cate_score[i] != 0)
                  {
//                      cate_score[i] += (v_size - hit[i]) * c_dict_->get_con2(i);
                      cate_score[i] += c_dict_->get_con1(i);
//                      cate_score[i].score = (cate_score[i].score + 500 ) / 500;
                  }

                  double max_score = -1234567;
                  size_t max_ind = 0, ind = 0;;
                  size_t find = 0;
                  for (size_t i = 0; i < cate_size; ++i)
                      if (cate_score[i] != 0 && cate_score[i] > max_score && c_dict_->parent(i) == 0)
                      {
                          max_score = cate_score[i];
                          max_ind = i;
                          find = 1;
                      } 
                  if (find == 0) return res_map;


                  std::vector<cate_score_type> mm, mm1;
                  cate_score_type tmp;
                  mm1.resize(3);
                  mm1[0].score = max_score;
                  mm1[1].score = -1234567;
                  mm1[2].score = -1234567;
                  mm1[0].ind = max_ind;
                  mm.resize(3);
                  while(find == 1)
                  {
                      find = 0;
                      ind = mm1[0].ind;
                      for (size_t i = 0; i < 3; ++i)
                      {
                          mm[i].ind = 0;
                          mm[i].score = -1234567;
                      }
                      for (size_t i = 0; i < c_dict_->child_size(ind); ++i)
                      {
                          double tmp_score = cate_score[c_dict_->child(ind, i)];
                          if (tmp_score != 0 && tmp_score > mm[2].score)
                          {
                              mm[2].score = tmp_score;
                              mm[2].ind = c_dict_->child(ind, i);
                              if (mm[2].score > mm[1].score) {tmp = mm[2]; mm[2] = mm[1]; mm[1] = tmp;}
                              if (mm[1].score > mm[0].score) {tmp = mm[1]; mm[1] = mm[0]; mm[0] = tmp;}
                              find = 1;
                          }
                      }
                      if (find == 0) break;
                      for (size_t i = 0; i < 3; ++i)
                          mm1[i] = mm[i];
                  }
                  for (size_t i = 0; i < mm1.size(); ++i)
                      if (fabs(mm1[i].score + 1234567) > 1e-6) 
                          res_map[c_dict_->get_kstr(mm1[i].ind)] = mm1[i].score;

if (dolog)                  
{
//ss<<res.first<<' '<<res.second<<endl;                 
ss<<kstr<<'\t';

for(size_t i = 0; i < cate_size; ++i)
{
    if (cate_score[i] == 0)continue;
    ss<<i<<' '<<c_dict_->get_kstr(i)<<' '<<cate_score[i]<<'\t';
//    ss<<i<<' '<<cate_table_[i].kstr<<' '<<cate_score[i]<<' '<<cate_table_[i].score<<' '<<cate_table_[i].con1<<' '<<cate_table_[i].con2<<' '<<hit[i]<<endl;
        token_ind = dict2_->token_trans(kstr);
        if (token_ind == NOT_FOUND) 
            ss<<kstr<<' '<<0<<'\t';
        else
        {
            size_t flag = 0;
            for (size_t k = 0; k < dict2_->size(token_ind); ++k)
                if (dict2_->get_ind(token_ind, k) == i)
                {
                    ss<<kstr<<' '<<dict2_->get_score(token_ind, k)<<'\t';
                    flag = 1;
                    break;
                }
            if (flag == 0)
                ss<<kstr<<' '<<0<<'\t';
        }
    ss<<endl;
}
} 

                return res_map;
            }



            std::map<KString, double> classify_stage_2(
			    const std::vector<std::pair<KString,double> >& v, stringstream& ss, bool dolog = false)
              {
                  std::map<KString, double> res_map;
                  std::pair<KString, double> res;                
                  if (v.size() == 0 || dict1_ == NULL)
                      return res_map;
                  size_t v_size = v.size();
                  size_t token_ind = 0;
                  size_t cate_size = c_dict_->size();
                  std::vector<double> cate_score;
                  cate_score.resize(cate_size);
                  std::vector<size_t> hit;
                  hit.resize(cate_size);

                  for (size_t i = 0; i < v_size; ++i)
                  {
                      const KString &kstr = v[i].first;
//                      if (kstr.length() == 0) continue;
                      token_ind = dict1_->token_trans(kstr);
                      if (token_ind == NOT_FOUND) continue;
                      size_t token_size = dict1_->size(token_ind);
++tot_term;                      
                      for (size_t j = 0; j < token_size; ++j)
                      {
++tot_loop;
                          cate_score[dict1_->get_ind(token_ind, j)] += dict1_->get_score(token_ind, j);
                          ++hit[dict1_->get_ind(token_ind, j)];
                      }
                      
                  }
                  
                  for (size_t i = 0; i < cate_size; ++i) if (cate_score[i] != 0)
                  {
                      cate_score[i] += (v_size - hit[i]) * c_dict_->get_con2(i);
                      cate_score[i] += c_dict_->get_con1(i);
//                      cate_score[i].score = (cate_score[i].score + 500 ) / 500;
                  }

                  double max_score = -1234567;
                  size_t max_ind = 0, ind = 0;;
                  size_t find = 0;
                  for (size_t i = 0; i < cate_size; ++i)
                      if (cate_score[i] != 0 && cate_score[i] > max_score && c_dict_->parent(i) == 0)
                      {
                          max_score = cate_score[i];
                          max_ind = i;
                          find = 1;
                      } 
                  if (find == 0) return res_map;


                  std::vector<cate_score_type> mm, mm1;
                  cate_score_type tmp;
                  mm1.resize(3);
                  mm1[0].score = max_score;
                  mm1[1].score = -1234567;
                  mm1[2].score = -1234567;
                  mm1[0].ind = max_ind;
                  mm.resize(3);
                  while(find == 1)
                  {
                      find = 0;
                      ind = mm1[0].ind;
                      for (size_t i = 0; i < 3; ++i)
                      {
                          mm[i].ind = 0;
                          mm[i].score = -1234567;
                      }
                      for (size_t i = 0; i < c_dict_->child_size(ind); ++i)
                      {
                          double tmp_score = cate_score[c_dict_->child(ind, i)];
                          if (tmp_score != 0 && tmp_score > mm[2].score)
                          {
                              mm[2].score = tmp_score;
                              mm[2].ind = c_dict_->child(ind, i);
                              if (mm[2].score > mm[1].score) {tmp = mm[2]; mm[2] = mm[1]; mm[1] = tmp;}
                              if (mm[1].score > mm[0].score) {tmp = mm[1]; mm[1] = mm[0]; mm[0] = tmp;}
                              find = 1;
                          }
                      }
                      if (find == 0) break;
                      for (size_t i = 0; i < 3; ++i)
                          mm1[i] = mm[i];
                  }
                  for (size_t i = 0; i < mm1.size(); ++i)
                      if (fabs(mm1[i].score + 1234567) > 1e-6) 
                          res_map[c_dict_->get_kstr(mm1[i].ind)] = mm1[i].score;

if (dolog)                  
{
//ss<<res.first<<' '<<res.second<<endl;                 
for(size_t i = 0; i < v_size; ++i)                  
ss<<v[i].first<<' '<<v[i].second<<'\t';
ss<<endl;

for(size_t i = 0; i < cate_size; ++i)
{
    if (cate_score[i] == 0)continue;
    ss<<i<<' '<<c_dict_->get_kstr(i)<<' '<<cate_score[i]<<'\t';
//    ss<<i<<' '<<cate_table_[i].kstr<<' '<<cate_score[i]<<' '<<cate_table_[i].score<<' '<<cate_table_[i].con1<<' '<<cate_table_[i].con2<<' '<<hit[i]<<endl;
    for (size_t j = 0; j < v_size; ++j)
    {
        token_ind = dict1_->token_trans(v[j].first);
        if (token_ind == NOT_FOUND) 
            ss<<v[j].first<<' '<<0<<'\t';
        else
        {
            size_t flag = 0;
            for (size_t k = 0; k < dict1_->size(token_ind); ++k)
                if (dict1_->get_ind(token_ind, k) == i)
                {
                    ss<<v[j].first<<' '<<dict1_->get_score(token_ind, k)<<'\t';
                    flag = 1;
                    break;
                }
            if (flag == 0)
                ss<<v[j].first<<' '<<0<<'\t';
        }
    }
    ss<<endl;
}
}
                      return res_map;
              }
		};
	}}
#endif

