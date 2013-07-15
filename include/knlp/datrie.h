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
using namespace ilplib::knlp;
using namespace std;

namespace ilplib
{
    namespace knlp
    {
        class DATrie
        {
//        pravite:
            size_t max_length_;
            size_t tot_length_;
            double MINVALUE_; 
            typedef struct {KString kstr; double value; size_t len;} word_type;
            std::vector<word_type> dict_;
            std::vector<int> base_;
            std::vector<int> check_;
            std::vector<int> cnt_;
            std::vector<double> value_;
            uint16_t ch1_[123456];

        public:
            static bool word_cmp(const word_type& x, const word_type& y)
            {
                return x.kstr < y.kstr;
                /*
                size_t min_len = std::min(x.len, y.len);
                for (size_t i = 0; i < min_len; ++i)
                    if (x.kstr[i] != y.kstr[i])
                        return x.kstr[i] < y.kstr[i];
                return x.len < y.len;
                */
            }
            DATrie(string file_name)
            {
                MINVALUE_ = 0.123;
                dict_.reserve(1234567);
                memset(ch1_, 0, sizeof(ch1_));
//                char st[65536];
                double value;
                max_length_ = 0;
                tot_length_ = 0;

                char* st = NULL;
                izenelib::am::util::LineReader lr(file_name);
				while((st = lr.line(st)) != NULL)
				{
				    word_type tmpword;
				    string s0 = string(st);
				    int p = s0.find("\t",0);
				    KString kstr(s0.substr(0,p));
					ilplib::knlp::Normalize::normalize(kstr);
				    value = atof(s0.substr(p+1, s0.length()-p-1).c_str());
				    tmpword.kstr = kstr;
				    tmpword.value = value;
				    tmpword.len = tmpword.kstr.length();
				    dict_.push_back(tmpword);
					ch1_[kstr[0]] = 1;
//					if (kstr == "[min]") MINVALUE_ = value;
					tot_length_ += tmpword.len;
					max_length_ = std::max(max_length_, tmpword.len);
				}

                sort(dict_.begin(), dict_.end(), word_cmp);
/*
                freopen("testdict", "w", stdout);
                for (size_t i = 0; i < dict.size(); ++i)
                    cout<<dict[i].kstr<<'\t'<<dict[i].len<<'\t'<<dict[i].value<<endl;
*/                    
                printf("word num = %zu, tot len = %zu, max len = %zu\n", dict_.size(), tot_length_, max_length_);


                build(dict_);
            }

            void build(std::vector<word_type> word)
            {
                int lastad, tmpad, temp;
                int dataNum;
                int tryBase, tryBaseCount, tryBaseMax = 50;
                int start;
                size_t word_size = word.size();
                size_t max_num = word_size * 7;
                int ad[word_size], next[word_size], pre[word_size];

                memset(ad, 0, sizeof(ad));
                memset(next, 0, sizeof(next));
                memset(pre, 0, sizeof(pre));

                base_.clear();
                base_.resize(max_num);
                check_.clear();
                check_.resize(max_num);
                cnt_.clear();
                cnt_.resize(max_num);
                value_.clear();
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
//                    printf("finish : %d/%d  dataNUm=%d  tryBase=%d\n", i+1, maxLength, dataNum, tryBase);
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
                        value_[tmpad] = dict_[tmpad].value;
                    }
                }
            }

            vector<pair<KString, double> > token(const KString st)
            {
                size_t i = 0;
                size_t len = st.length();
                std::vector<std::pair<KString, double> > term;
                size_t term_size = 0;
                term.resize(len);
                if (!len)
                    return term;
                while(i < len)
                {
                    int maxlen = -1, ad, nextad, flag = 0;
                    double value = MINVALUE_;
                    ad = st[i];
                    if (base_[ad] < 0) maxlen = 1;
                    for (size_t j = 1; j < len - i; ++j)
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
                        term[term_size++].second = value;
                        i += maxlen;
                    }
                }
                term.resize(term_size);
                return term;
            }

        };
    }
}
#endif


