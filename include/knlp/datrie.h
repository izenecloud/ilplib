#ifndef _ILPLIB_NLP_DA_TRIE_H_
#define _ILPLIB_NLP_DA_TRIE_H_

#include <string.h>
#include <string>
#include <vector>
#include "types.h"
#include "util/string/kstring.hpp"
#include "knlp/normalize.h"
#include "am/util/line_reader.h"

using namespace izenelib;
using namespace izenelib::util;
using namespace std;
#define NOT_FOUND 0xFFFFFFFFFFFFFFFF
namespace ilplib
{
    namespace knlp
    {
        class DATrie
        {
        private:
            size_t max_length_;
            size_t tot_length_;
            size_t max_num;
            double MINVALUE_; 
            typedef struct {KString kstr; double value; size_t len;} word_type;
            std::vector<int> base_;
            std::vector<int> check_;
            std::vector<double> value_;
            std::vector<int> cnt_;
            std::vector<word_type> dict_;
            std::vector<int> ad,next,pre;
            bool ch1_[123456];
            

        public:
            static bool word_cmp(const word_type& x, const word_type& y)
            {
                return x.kstr < y.kstr;
            }

            DATrie() {}
            DATrie(const std::string& file_name, const size_t mode = 0)
            {
time_t time1, time2;
time1 = clock();
                base_.clear();
                check_.clear();
                cnt_.clear();
                value_.clear();
                dict_.clear();
                max_length_ = 0;
                tot_length_ = 0;
                MINVALUE_ = 0;
                memset(ch1_, 0, sizeof(ch1_));
                MINVALUE_ = 0.123;
                std::vector<word_type> tmpdict;
                tmpdict.clear();
                tmpdict.reserve(1234567);
                double value;

//cout<<"mode = "<<mode<<endl;
                char* st = NULL;
                izenelib::am::util::LineReader lr(file_name);
				while((st = lr.line(st)) != NULL)
				{
				  if (mode == 1)
                  {
				    if (strlen(st) == 0) continue;
				    word_type tmpword;
				    string s0 = string(st);
				    s0 += '\t';
//                    Normalize::normalize(s0);
				    int p = s0.find("\t",0);
				    if (p <= 0 || p >= (int)s0.length()) continue;
				    KString kstr(s0.substr(0,p));
                    Normalize::normalize(kstr);
					tmpword.kstr = kstr;
					tmpword.len = kstr.length();
					tmpdict.push_back(tmpword);
					if (kstr.length() > 0)
					    ch1_[kstr[0]] = 1; 
					tot_length_ += tmpword.len;
					max_length_ = std::max(max_length_, tmpword.len);
                  }
                  else if (mode == 0)
                  {
				    if (strlen(st) == 0) continue;
				    word_type tmpword;
				    string s0 = string(st);
				    s0 += '\t';
//                    Normalize::normalize(s0);
				    int p = s0.find("\t",0);
				    if (p <= 0 || p >= (int)s0.length()) continue;
				    KString kstr(s0.substr(0,p));
                    Normalize::normalize(kstr);
				    value = atof(s0.substr(p+1, s0.length()-p-1).c_str());
				    tmpword.kstr = kstr;
				    tmpword.value = value;
				    tmpword.len = kstr.length();
				    tmpdict.push_back(tmpword);
					if (kstr.length() > 0) 
					ch1_[kstr[0]] = 1;
					if (kstr == "[min]") MINVALUE_ = value;
					tot_length_ += tmpword.len;
					max_length_ = std::max(max_length_, tmpword.len);
                  }
				}
                
                if (tmpdict.empty()) return;
                sort(tmpdict.begin(), tmpdict.end(), word_cmp);

                dict_.reserve(tmpdict.size());
                for(size_t i = 0; i+1 < tmpdict.size(); ++i)
                    if (!(tmpdict[i].kstr == tmpdict[i+1].kstr))
                        dict_.push_back(tmpdict[i]);
                dict_.push_back(tmpdict[tmpdict.size() - 1]);
                    
//printf("word num = %zu, tot len = %zu, max len = %zu\n", dict_.size(), tot_length_, max_length_);

                max_num = tot_length_ * 2;
                max_num = std::max((size_t)1000000, max_num);
time2 = clock();
//printf("before build dict time = %lf\n", (double)(time2 - time1) / 1000000);
                build(dict_);
//cout<<"build finish"<<endl;                
time1 = clock();
//printf("build %zu dict time = %lf\n", dict_.size(), (double)(time1 - time2) / 1000000);
            }

            ~DATrie(){}

            void build(std::vector<word_type>& word)
            {
                if (word.empty()) return;
                int lastad, tmpad, temp;
                int dataNum;
                int tryBase, tryBaseCount, tryBaseMax = 2;
                int start;
                size_t word_size = word.size();
 //               int ad[word_size], next[word_size], pre[word_size];

//                memset(ad, 0, sizeof(ad));
//                memset(next, 0, sizeof(next));
//                memset(pre, 0, sizeof(pre));
                ad.resize(word_size);
                next.resize(word_size);
                pre.resize(word_size);

                base_.resize(max_num);
                check_.resize(max_num);
                cnt_.resize(max_num);
                value_.resize(max_num);

                for (size_t i = 0; i < word_size; ++i)
                {
                    ad[i] = word[i].kstr[0];
                    base_[ad[i]] = 1;
                    next[i] = i + 1;
                    pre[i] = i - 1;
                }
                tryBase = 1;
                tryBaseCount = 0;
                dataNum = 0;
                start = 0;

                for (size_t i = 0; i < max_length_; i++)
                {
                    lastad = start;
                    size_t j = 0;
                    for (j = next[start]; j < word_size; j = next[j])
                    {
                        size_t k = 0;
                        for (k = 0; k <= i && word[j].kstr[k] == word[pre[j]].kstr[k]; ++k);
                            if (k <= i)
                            {
                                size_t tmpBase = 0;
                                for (tmpBase = tryBase; tmpBase < max_num; ++tmpBase)
                                {
                                    for (k = lastad; k < j; k = next[k])
                                        if (i+1 < word[k].len && base_[tmpBase + word[k].kstr[i+1]] != 0)
                                            break;
                                    if (k == j)
                                        break;
                                    ++cnt_[tmpBase];
                                }

                                while (cnt_[tryBase] >= tryBaseMax)
                                    ++tryBase;

                                base_[ad[lastad]] = tmpBase;
                                for (k = lastad; k < j; k = next[k])
                                    if (i+1 < word[k].len)
                                    {
                                        temp = tmpBase + word[k].kstr[i+1];
                                        base_[temp] = 1;
                                        check_[temp] = ad[k];
                                        ad[k] = temp;
                                        dataNum = std::max(dataNum, temp);
                                    }
                                lastad = j;
                            }
                    }

                    size_t tmpBase = 0;
                    for (tmpBase = tryBase; tmpBase < max_num; ++tmpBase)
                    {
                        size_t k = 0;
                        for (k = lastad; k < j; k = next[k])
                            if (i+1 < word[k].len && base_[tmpBase + word[k].kstr[i+1]] != 0)
                                break;
                        if (k == j)
                            break;
                    }

                    base_[ad[lastad]] = tmpBase;
                    for (size_t k = lastad; k < j; k = next[k])
                        if (i+1 < word[k].len)
                        {
                            temp = tmpBase + word[k].kstr[i+1];
                            base_[temp] = 1;
                            check_[temp] = ad[k];
                            ad[k] = temp;
                            dataNum = std::max(dataNum, temp);
                        }

                    start = -1;
                    for (size_t j = 0; j < word_size; ++j)
                        if (word[j].len > i+1)
                        {
                            if (start == -1)
                            {
                                start = j;
                                lastad = j;
                                continue;
                            }
                            next[lastad] = j;
                            pre[j] = lastad;
                            lastad = j;
                        }
                    next[lastad] = word_size;
                }

                for (size_t i = 0; i < word_size; ++i)
                {
                    tmpad = word[i].kstr[0];
                    for (size_t j = 1; j < word[i].len; ++j)
                        tmpad = abs(base_[tmpad]) + word[i].kstr[j];
                    if (base_[tmpad] > 0)
                    {
                        base_[tmpad] = 0 - base_[tmpad];
                        value_[tmpad] = i;
                    }
                }
            }

            size_t find_word(const KString& st, const bool normalize = 0)
            {
//                KString st(kst);
//                if (normalize)
//                    Normalize::normalize(st);
                int ad = 0, nextad = 0;
                size_t len = st.length();
                if (len == 0) return NOT_FOUND;
                for (size_t i = 0; i < len; ++i)
                {
                    nextad = abs(base_[ad]) + st[i];
                    if (base_[nextad] == 0 || check_[nextad] != ad)
                        return NOT_FOUND;
                    ad = nextad;
                }
                if (base_[ad] < 0)
                    return value_[ad];
                return NOT_FOUND;
            }

            size_t find_word(const KString& st, const size_t be, const size_t en)
            {
                int ad = 0, nextad = 0;
                size_t len = st.length();
                if (len == 0) return NOT_FOUND;
                for (size_t i = be; i < en; ++i)
                {
                    nextad = abs(base_[ad]) + st[i];
                    if (base_[nextad] == 0 || check_[nextad] != ad)
                        return NOT_FOUND;
                    ad = nextad;
                }
                if (base_[ad] < 0)
                    return value_[ad];
                return NOT_FOUND;
            }

            bool check_term(const KString& st, const bool normalize = 0)
            {
//                KString st(kst);
//                if (normalize)
//                    Normalize::normalize(st);
                int ad = 0, nextad;
                size_t len = st.length();
                if (len == 0) return 0;
                for (size_t i = 0; i < len; ++i)
                {
                    nextad = abs(base_[ad]) + st[i];
                    if (base_[nextad] == 0 || check_[nextad] != ad)
                        return 0;
                    ad = nextad;
                }
                if (base_[ad] < 0)
                    return 1;
                return 0;
            }

            double score(const KString& st, const bool normalize = 0)
            {
                size_t ind = find_word(st, normalize);
                if (ind != NOT_FOUND)
                    return dict_[ind].value;
                return MINVALUE_;
            }

            size_t size() const
            {
                return dict_.size();
            }

            double min() const
            {
                return MINVALUE_;
            }

            std::vector<std::pair<KString, double> > token(const KString& st)
            {
                size_t i = 0;
                size_t len = st.length();
                std::vector<std::pair<KString, double> > term;
                if (len == 0)
                    return term;
                size_t term_size = 0;
                term.resize(len);
                while(i < len)
                {
                    int maxlen = -1, ad = 0, nextad, flag = 0;
                    double value = MINVALUE_;
                    for (size_t j = 0; j < len - i; ++j)
                    {
                        nextad = abs(base_[ad]) + st[i+j];                
                        if (base_[nextad] == 0 || check_[nextad] != ad)
                        {
                            flag = 0;
                            break;
                        }
                        ad = nextad;
                        if (base_[ad] < 0)
                        {
                            maxlen = j+1;
                            value = value_[ad];
                        }
                    }

                    if (maxlen == -1)
                    {
                        term[term_size].first = st.substr(i, 1);
                        term[term_size++].second = MINVALUE_;
                        i += 1;
                    }
                    else
                    {
                        term[term_size].first = st.substr(i, maxlen);
                        term[term_size++].second = dict_[value].value;
                        i += maxlen;
                    }
                }
                term.resize(term_size);
                return term;
            }

            std::vector<std::pair<KString, double> > sub_token(const KString& st)
            {
                size_t r = find_word(st);
                if (r == NOT_FOUND) return token(st);
                else
                {
                    size_t len = st.length();
                    std::vector<std::pair<KString, double> > term;
                    for (size_t i = len-1; i > 0; --i)
                    {
                        size_t p = find_word(st, 0, i);
                        size_t q = find_word(st, i, len);
                        if (p != NOT_FOUND && q != NOT_FOUND)
                        {
                            term.push_back(std::make_pair(st.substr(0, i), dict_[p].value));
                            term.push_back(std::make_pair(st.substr(i, len - i), dict_[q].value));
                            return term;
                        }
                    }
                    term.push_back(std::make_pair(st, dict_[r].value));
                    return term;
                }
            }

        };
    }
}
#endif


