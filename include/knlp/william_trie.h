#ifndef _ILPLIB_NLP_WILLIAM_TRIE_H_
#define _ILPLIB_NLP_WILLIAM_TRIE_H_

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

namespace ilplib
{
namespace knlp
{
class WilliamTrie
{
private:
    std::vector<std::pair<KString, double> > dict_;
    double MINVALUE_;
    size_t tot_bi_;
    size_t tot_len_;
    size_t ch1_[100001];

public:
    WilliamTrie(string file_name)
    {
        std::vector<std::pair<KString, double> > tmpdict;
        for(size_t i = 0; i < 100000; ++i)
        {
            ch1_[i]=0;
        }
//freopen("testdict","w",stdout);
        tot_bi_ = 0;
        tot_len_ = 0;
        tmpdict.reserve(200000);
//				freopen(file_name.c_str(),"r",stdin);
        char* st = NULL;
        double value;
        string s0;
        int p;
        izenelib::am::util::LineReader lr(file_name);
        while((st = lr.line(st))!=NULL)
        {
            s0 = string(st);
            p = s0.find("\t",0);
            if (p < 0 || p >= (int)s0.length()) continue;
            KString kstr(s0.substr(0,p));
            ilplib::knlp::Normalize::normalize(kstr);
            value = atof(s0.substr(p+1, s0.length()-p-1).c_str());
            tmpdict.push_back(std::make_pair(kstr, value));
            if (kstr.length() > 0) ch1_[kstr[0]] = 1;
            if (kstr == "[min]")
                MINVALUE_ = value;
        }
        sort(tmpdict.begin(), tmpdict.end(), WilliamTrie::cmp);

        dict_.reserve(tmpdict.size());
        for(size_t i = 0; i < tmpdict.size() - 1; ++i)
            if (!(tmpdict[i].first == tmpdict[i+1].first))
                dict_.push_back(tmpdict[i]);
        dict_.push_back(tmpdict[tmpdict.size() - 1]);

//for(size_t i = 0; i < dict_.size(); ++i)
//    cout<<dict_[i].first<<'\t'<<dict_[i].second<<endl;

    }

    ~WilliamTrie() {}

    static bool cmp(const std::pair<KString, double>& x, const std::pair<KString, double>& y)
    {
        return x.first < y.first;
    }

    size_t bisearch(size_t ind, uint16_t ch, size_t &p, size_t &q, double &value, const KString& k=KString(""))
    {
        if (p > q) return 0;
        int head = p, tail = q, mid = 0;
        uint16_t tmp;
        while(head <= tail)
        {
            ++tot_bi_;
            mid = (head + tail) / 2;
            tmp = dict_[mid].first[ind];
            if (tmp < ch)
            {
                head = mid + 1;
            }
            else if (tmp == ch && (mid == (int)p || dict_[mid-1].first[ind] != ch))
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
        if (dict_[mid].first[ind] != ch)
            return 0;


        head = p;
        tail = q;
        while(head <= tail)
        {
            ++tot_bi_;
            mid = (head + tail) / 2;
            tmp = dict_[mid].first[ind];
            if (tmp > ch)
            {
                tail = mid - 1;
            }
            else if (tmp == ch && (mid == (int)q || dict_[mid+1].first[ind] != ch))
            {
                q = mid;
                break;
            }
            else
            {
                head = mid + 1;
            }
        }
        if (dict_[p].first.length() == ind+1)
        {
            value = dict_[p].second;
            ++p;
            return 2;
        }
        else
        {
            return 1;
        }
    }



    std::vector<std::pair<KString, double> > token(const KString st)
    {
        size_t i = 0;
        size_t len = st.length();
        std::vector<std::pair<KString, double> > term;
        term.reserve(len);
        if (!len)
            return term;
        while(i < len)
        {
            size_t maxlen = (size_t)-1, p = 0, q = dict_.size() - 1, flag = 0;
            double value = MINVALUE_;
            /*
            					while(i < len && st[i] == ' ')
            					    ++i;
            					if (i >= len)
            					    break;
            */
            for (size_t j = 0; j < len - i; ++j)
            {
//						if (st[i+j] == ' ') break;
                if (j == 0 && !ch1_[st[i+j]])
                    break;
                else
                    flag = bisearch(j, st[i+j], p, q, value, st);
//cout<<st[i+j]<<' '<<i<<' '<<j<<' '<<p<<' '<<q<<endl;
                ++tot_len_;
                if (flag == 0)
                {
                    break;
                }
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
            if (maxlen == (size_t)-1)
            {
                term.push_back(make_pair(st.substr(i, 1), MINVALUE_));
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

    bool check_term(const KString& st)
    {
        size_t len = st.length();
        if (!len)
            return MINVALUE_;
        size_t maxlen = -1, p = 0, q = dict_.size() - 1, flag = 0;
        double value = MINVALUE_;

        for (size_t j = 0; j < len; ++j)
        {
            flag = bisearch(j, st[j], p, q, value);
            if (flag == 0)
                return 0;
            else if (flag == 2)
                maxlen = j;
            else if (flag == 3)
            {
                maxlen = j;
                break;
            }
        }
        if (maxlen == len - 1)
            return 1;
        else
            return 0;
    }

    double score(const KString& st)
    {

        size_t len = st.length();
        if (!len)
            return MINVALUE_;
        size_t maxlen = -1, p = 0, q = dict_.size() - 1, flag = 0;
        double value = MINVALUE_;

        for (size_t j = 0; j < len; ++j)
        {
            flag = bisearch(j, st[j], p, q, value, st);
            if (flag == 0)
            {
                return MINVALUE_;
            }
            else if (flag == 2)
            {
                maxlen = j;
            }
            else if (flag == 3)
            {
                maxlen = j;
                break;
            }
        }
        if (maxlen == len - 1)
            return value;
        else
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

    size_t tot() const
    {
        return tot_bi_;
    }

    size_t totlen() const
    {
        return tot_len_;
    }

};
}
}
#endif

