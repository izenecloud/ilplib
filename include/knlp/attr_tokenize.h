/*
 * =====================================================================================
 *
 *       Filename:  attr_tokenize.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  01/12/2013 06:05:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), kevin.hu@b5m.com
 *        Company:  iZeneSoft.com
 *
 * =====================================================================================
 */
#ifndef _ILPLIB_NLP_ATTR_TOKENIZE_H_
#define _ILPLIB_NLP_ATTR_TOKENIZE_H_

#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <istream>
#include <ostream>
#include <cctype>
#include <math.h>
#include <set>

#include "dictionary.h"
#include "util/string/kstring.hpp"
#include "trd2simp.h"
#include "am/util/line_reader.h"
#include "datrie.h"
#include "knlp/title_pca.h"

//#define WEIGHT_SCOPE 15
namespace ilplib
{
namespace knlp
{

class AttributeTokenize
{
    TitlePCA token_dict_;
    DATrie attv_dict_;
    DATrie att_dict_;

    string unicode_to_utf8_(const KString& kstr)
    {
        string s;
        s.reserve(kstr.length() << 1);
        for(size_t i = 0; i < kstr.length(); ++i)
        {
            uint16_t unic = kstr[i];
            if ( unic <= 0x0000007F )
            {
                // * U-00000000 - U-0000007F:  0xxxxxxx
                s.append(1, unic & 0x7F);
            }
            else if ( unic >= 0x00000080 && unic <= 0x000007FF )
            {
                // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
                s.append(1, (((unic >> 6) & 0x1F) | 0xC0));
                s.append(1, ((unic & 0x3F) | 0x80));
            }
            else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )
            {
                // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
                s.append(1, (((unic >> 12) & 0x0F) | 0xE0));
                s.append(1, (((unic >>  6) & 0x3F) | 0x80));
                s.append(1, ((unic & 0x3F) | 0x80));
            }
            else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )
            {
                // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                s.append(1, (((unic >> 18) & 0x07) | 0xF0));
                s.append(1, (((unic >> 12) & 0x3F) | 0x80));
                s.append(1, (((unic >>  6) & 0x3F) | 0x80));
                s.append(1, ((unic & 0x3F) | 0x80));
            }
            else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )
            {
                // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                s.append(1, (((unic >> 24) & 0x03) | 0xF8));
                s.append(1, (((unic >> 18) & 0x3F) | 0x80));
                s.append(1, (((unic >> 12) & 0x3F) | 0x80));
                s.append(1, (((unic >>  6) & 0x3F) | 0x80));
                s.append(1, ((unic & 0x3F) | 0x80));
            }
            else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )
            {
                // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                s.append(1, (((unic >> 30) & 0x01) | 0xFC));
                s.append(1, (((unic >> 24) & 0x3F) | 0x80));
                s.append(1, (((unic >> 18) & 0x3F) | 0x80));
                s.append(1, (((unic >> 12) & 0x3F) | 0x80));
                s.append(1, (((unic >>  6) & 0x3F) | 0x80));
                s.append(1, ((unic & 0x3F) | 0x80));
            }
        }
        return s;
    }

    KString sub_cate_(const std::string& cate, bool g=true)
    {
        if (cate.length() == 0)return KString();
        KString r(cate);
        int32_t i=r.length()-1;
        if (r[i] == '>')
        {
            r = r.substr(0, r.length()-1);
            i--;
        }
        if (g)
            for ( ; i>=0 && r[i]!='/' && r[i]!='>'; --i);
        else
            for ( ; i>=0 && r[i]!='>'; --i);

        if (i+1 < (int32_t)r.length())
            return r.substr(i+1);
        return r;
    }

    KString normallize_(const std::string& str)
    {
        try
        {
            return normallize_(KString(str));
        }
        catch(...)
        {
        }
        return KString();
    }

    KString normallize_(KString r)
    {
        Normalize::normalize(r);
        return r;
    }
    
    //index
    void token_fields_(const std::string& line, double score,
                       std::vector<std::pair<std::string, double> >& r,
                       bool do_subtoken = true)
    {
        std::map<std::string, double> m, subm;
        std::vector<std::pair<std::string,float> > tks, subtks;
        std::string brand, model;
        token_dict_.pca(line, tks, brand, model, subtks, do_subtoken);
        float sum = 0;
        for (uint32_t j=0; j<tks.size();j++)
            sum += tks[j].second,
                m[tks[j].first] = tks[j].second;
        for (std::map<std::string, double>::iterator it=m.begin(); it!=m.end();++it)
            it->second /= sum;

        for (std::map<std::string, double>::iterator it=m.begin(); it!=m.end();++it)
            r.push_back(std::make_pair(it->first, it->second*score));
        if (!do_subtoken)return;

        //subtokens
        sum = 0;
        for (uint32_t j=0; j<subtks.size();j++)
            if(m.find(subtks[j].first) == m.end())
                sum += subtks[j].second,
                    subm[subtks[j].first] = subtks[j].second;
        for (std::map<std::string, double>::iterator it=subm.begin(); it!=subm.end();++it)
            it->second /= sum;

        for (std::map<std::string, double>::iterator it=subm.begin(); it!=subm.end();++it)
            r.push_back(std::make_pair(it->first, it->second*score/10.));
    }

    uint32_t chn_num_(const KString& kstr)
    {
        uint32_t r = 0;
        for (uint32_t i=0; i<kstr.length(); i++)
            if (KString::is_chinese(kstr[i]))
                r++;
        return r;
    }

    bool bad_char_(const KString& kstr)
    {
        if (kstr.length() == 1 &&
                (KString::is_english(kstr[0])||KString::is_numeric(kstr[0]))
           )
            return true;
        static const KString oth1("其他");
        static const KString oth2("其它");
        static const KString oth3("other");
        static const KString oth4("价格");
        static const KString oth5("货号");
        static const KString oth6("款号");
        static const KString oth7("编号");
        static const KString oth8("isbn");
        static const KString oth9("更多");
        static const KString oth10("商品名称");
        for (uint32_t i=0; i<kstr.length(); i++)
            if (kstr[i] == ':' || kstr[i] == '/')
                return true;
        if (kstr.find(oth1) != (uint32_t)-1
                ||kstr.find(oth2) != (uint32_t)-1
                ||kstr.find(oth3) != (uint32_t)-1
                ||kstr.find(oth4) != (uint32_t)-1
                ||kstr.find(oth5) != (uint32_t)-1
                ||kstr.find(oth6) != (uint32_t)-1
                ||kstr.find(oth7) != (uint32_t)-1
                ||kstr.find(oth8) != (uint32_t)-1
                ||kstr.find(oth9) != (uint32_t)-1
                ||kstr.find(oth10) != (uint32_t)-1
           )
            return true;
        return false;
    }

public:

    AttributeTokenize(const std::string& dir)
        :token_dict_(dir+"/../title_pca")
        ,attv_dict_(dir+"/att.nv.score")
        ,att_dict_(dir+"/att.n.score")
    {
    }

    ~AttributeTokenize()
    {
    }

    void tokenize(const std::string& title, const std::string& attr,
                  const std::string& cate, const std::string& ocate,
                  const std::string& source,
                  std::vector<std::pair<std::string, double> >& r)
    {
        std::vector<KString> kattrs = KString(attr).split(',');

        double max_avs = attv_dict_.score(KString("[title]"));
        KString subcate = sub_cate_(cate);

        token_fields_(title, max_avs*30, r);
        token_fields_(source, max_avs*10, r);
        token_fields_(unicode_to_utf8_(subcate), max_avs, r);
        token_fields_(unicode_to_utf8_(sub_cate_(ocate)), max_avs, r);

        for ( uint32_t i=0; i<kattrs.size(); ++i)
        {
            std::vector<KString> p = kattrs[i].split(':');
            if (p.size() != 2 || p[0].length()==0 || p[1].length() == 0)
                continue;
            KString av = p[0];
            av+='@',av+=subcate,av+=':',av+=p[1];
            double avs = attv_dict_.score(av);

            p[0] += ' ';p[0] += p[1];
            token_fields_(unicode_to_utf8_(p[0]), avs, r);
        }

        //sum up duplications
        std::map<std::string, double> m;
        double sum = 0;
        for (uint32_t i=0;i<r.size();i++){
            if(m.find(r[i].first) == m.end())
                m[r[i].first] = r[i].second;
            else m[r[i].first] += r[i].second;
            sum += r[i].second;
        }
        r = std::vector<std::pair<std::string, double> >();
        for (std::map<std::string, double>::iterator it=m.begin();it!=m.end();++it)
            r.push_back(std::make_pair(it->first, it->second/sum));
    }

    //query
    void tokenize(const std::string& Q,
                  std::vector<std::pair<std::string, int> >& r,
                  bool do_subtoken=false)
    {
        std::vector<std::pair<std::string, double> > rr;
        token_fields_(Q, 100., rr, do_subtoken);
        std::map<std::string, double> m;
        double sum = 0;
        for (uint32_t i=0;i<rr.size();i++)if(m.find(rr[i].first)==m.end()){
            m[rr[i].first] = rr[i].second;
            sum += rr[i].second;
        }

        for (std::map<std::string, double>::iterator it=m.begin();it!=m.end();++it)
            r.push_back(std::make_pair(it->first, int(it->second/sum+0.5)));
    }

    double att_weight(const std::string& nm, const std::string& cate)
    {
        KString att(nm);
        if (chn_num_(att) > 5 || att.length() > 6 || bad_char_(att))return 0.;
        att += '@';
        att += sub_cate_(cate);
        return att_dict_.score(att);
    }

    double att_weight(const std::string& nm, const std::string val,
                      const std::string& cate)
    {
        KString att(nm);
        KString attv(val);
        if (chn_num_(att) > 5 || chn_num_(attv) > 6
                ||att.length() > 6 || attv.length() > 15
                ||bad_char_(attv) || bad_char_(att)
           )return 0.;
        att += '@';
        att += sub_cate_(cate);
        att += attv;

        return attv_dict_.score(att);
    }

};

}
}//namespace

#endif

