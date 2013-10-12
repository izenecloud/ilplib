#ifndef _ILPLIB_KNLP_PRODUCT_NAME_DETECTOR_H_
#define _ILPLIB_KNLP_PRODUCT_NAME_DETECTOR_H_

#include <string>
#include <list>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include "types.h"
#include "knlp/normalize.h"
#include "knlp/fmm.h"
#include "knlp/string_patterns.h"
#include "am/util/line_reader.h"

namespace ilplib
{
namespace knlp
{
class ProductNameDetector
{
private:
    Fmm tkn_;
    ilplib::knlp::GarbagePattern* gp_;
    enum {RPODUCT_NAME_TOKEN=10000};

public:
    ProductNameDetector(const std::string& nm, GarbagePattern* gp=NULL)
        :tkn_(nm),gp_(gp)
    {
    }

    ~ProductNameDetector() {}

    KString
    product_name(const string& str)
    {
        KString kstr;
        try
        {
            if(gp_)
                kstr = KString(gp_->clean(str));
            else kstr = KString(str);
        }
        catch(...)
        {
            kstr = KString(str);
        }
        ilplib::knlp::Normalize::normalize(kstr);
        std::vector<std::pair<KString, double> > v;
        tkn_.fmm(kstr, v);
        if (v.empty()) return KString("");
        KString ans;
        for (uint32_t i=0; i<v.size(); ++i)
            if (v[i].second == RPODUCT_NAME_TOKEN)
                ans += v[i].first,ans+=' ';

        return ans;
    }

    std::vector<std::string>
    multi_product_name(const string& str)
    {
        std::set<std::string> tmpans;
        std::vector<std::string> ans;
        KString kstr(str);
        std::vector<std::pair<KString, double> > v, vv;
        tkn_.fmm(kstr, v);
        if (v.empty()) return ans;
        for (int32_t i=(int32_t)(v.size()-1); i>=0; --i)
            if (v[i].second == RPODUCT_NAME_TOKEN)
            {
                tmpans.insert(v[i].first.get_bytes("utf-8"));
            }

        if (tmpans.empty())
        {
            vv = tkn_.subtokens(v);
            for (int32_t i=(int32_t)(vv.size()-1); i>=0; --i)
                if (vv[i].second == RPODUCT_NAME_TOKEN)
                {
                    tmpans.insert(vv[i].first.get_bytes("utf-8"));
                }
        }

        for(std::set<std::string>::iterator it = tmpans.begin(); it != tmpans.end(); ++it)
            ans.push_back(*it);

        return ans;
    }
    KString
    product_name(const string& str, std::stringstream& ss, bool dolog=false)
    {
        KString kstr;
        try
        {
            if(gp_)
                kstr = KString(gp_->clean(str));
            else kstr = KString(str);
        }
        catch(...)
        {
            kstr = KString(str);
        }
        ilplib::knlp::Normalize::normalize(kstr);
        std::vector<std::pair<KString, double> > v, vv;
        tkn_.fmm(kstr, v);
        if (v.empty()) return KString("");
        KString ans;
        for (int32_t i=(int32_t)(v.size()-1); i>=0; --i)
            if (v[i].second == RPODUCT_NAME_TOKEN)
            {
                ans = v[i].first;
                break;
            }

        if(dolog && !(KString("") == ans))
        {
            ss<<"!\t";
            for(size_t i = 0; i < v.size(); ++i)
                if(v[i].second == RPODUCT_NAME_TOKEN)
                    ss<<v[i].first<<'\t';
            ss<<'\n';
        }
        if (KString("") == ans)
        {
            vv = tkn_.subtokens(v);
            for (int32_t i=(int32_t)(vv.size()-1); i>=0; --i)
                if (vv[i].second == RPODUCT_NAME_TOKEN)
                {
                    ans = vv[i].first;
                    break;
                }

            if(dolog)
            {
                ss<<"@\t";
                for(size_t i = 0; i < vv.size(); ++i)
                    if(vv[i].second == RPODUCT_NAME_TOKEN)
                        ss<<vv[i].first<<'\t';
                ss<<'\n';
            }
        }

        if (KString("") == ans)
        {
            ss<<"#\t";
            for(size_t i = 0; i < v.size(); ++i)
                ss<<v[i].first<<'\t';
            ss<<'\n';
            for(size_t i = 0; i < vv.size(); ++i)
                ss<<vv[i].first<<'\t';
            ss<<'\n';
        }

        return ans;
    }
};
}
}
#endif

