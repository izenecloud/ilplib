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
//#include "knlp/william_trie.h"

namespace ilplib
{
namespace knlp
{

class AttributeTokenize
{
    DATrie token_dict_;
    DATrie attv_dict_;
    DATrie att_dict_;
//	Dictionary syn_dict_;
	DATrie syn_dict_;

    string unicode_to_utf8_(const KString& kstr)
    {  
        string s;
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
		if (r[i] == '>'){
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
	    try{
	        return normallize_(KString(str));
        }catch(...)
        {
        }
        return KString();
	}

	KString normallize_(KString r)
	{
		Normalize::normalize(r);
		return r;
	}

	bool is_model_type_(uint16_t c)
	{
		return (KString::is_english(c) 
					|| KString::is_numeric(c) 
					|| c == '-' || c == ',' || c == '.'
				);
	}

    std::set<KString> token_(const KString& av)
    {
        std::vector<KString> tmp = token_dict_.token(av, 0);
        KString tmpkstr;
        size_t tmp_size = tmp.size();
        std::set<KString> set;
        for (size_t j = 0; j < tmp_size; ++j)
        {
            if(is_model_type_(tmp[j][0]) && is_model_type_(tmp[j][tmp[j].length()-1]))
                tmpkstr+=tmp[j];
            else
            {
                if(tmpkstr.length()>0)
                {
                    if (tmpkstr.length()>1 || KString::is_english(tmpkstr[0]) || KString::is_numeric(tmpkstr[0])) 
                    {
                        KString syn;
                        syn_dict_.find_syn(tmpkstr, syn);
                        if (syn.length() > 0)
                            set.insert(syn);
                        else
                            set.insert(tmpkstr);
                    }
                    tmpkstr = KString("");
                }
                if (tmp[j].length()>1 || KString::is_chinese(tmp[j][0]))
                {
                    KString syn;
                    syn_dict_.find_syn(tmp[j], syn);
                    if (syn.length() > 0)
                        set.insert(syn);
                    else 
                        set.insert(tmp[j]);
                }
            }

        }
        if(tmpkstr.length()>0 && (tmpkstr.length()>1 || KString::is_english(tmpkstr[0]) || KString::is_numeric(tmpkstr[0])))
        {
            KString syn;
            syn_dict_.find_syn(tmpkstr, syn);
            if (syn.length() > 0)
                set.insert(syn);
            else
                set.insert(tmpkstr);
        }
        return set;
    }

    //index
	std::vector<std::pair<std::string, double> > token_(const std::vector<std::pair<KString, double> >& av)
    {
        std::map<KString, double> m;
        for ( uint32_t i=0; i+1<av.size(); ++i)
        {
            std::set<KString> set(token_(av[i].first));
            double score = av[i].second;
            const std::set<KString>::iterator set_end = set.end();
            if (i>0) 
                score/=sqrt(set.size());
            for (std::set<KString>::iterator it=set.begin(); it!=set_end; ++it)
                m[*it]+=score;
        }

        KString tmp = av.back().first;
        std::vector<KString> v = tmp.split('/');
        for (uint32_t i=0;i<v.size(); ++i)
        {
            KString syn;
            syn_dict_.find_syn(v[i], syn);
            if (syn.length() > 0)
                m[syn]+=av.back().second;
            else
                m[v[i]]+=av.back().second;
        }

        std::vector<std::pair<std::string, double> > r;

        for (std::map<KString, double>::iterator it=m.begin(); it!=m.end(); ++it)
            r.push_back(make_pair(unicode_to_utf8_(it->first), it->second*100.));

        return r;
    }

	uint32_t chn_num_(const KString& kstr)
    {
        uint32_t r = 0;
        for (uint32_t i=0;i<kstr.length();i++)
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
        for (uint32_t i=0;i<kstr.length();i++)
            if (kstr[i] == ':')
                return true;
        if (kstr.find(oth1) != (uint32_t)-1
          ||kstr.find(oth2) != (uint32_t)-1
          ||kstr.find(oth3) != (uint32_t)-1
          ||kstr.find(oth4) != (uint32_t)-1
          ||kstr.find(oth5) != (uint32_t)-1
          ||kstr.find(oth6) != (uint32_t)-1
          ||kstr.find(oth7) != (uint32_t)-1
          ||kstr.find(oth8) != (uint32_t)-1
          )
            return true;
        return false;
    }

public:

    AttributeTokenize(const std::string& dir)
        :token_dict_(dir+"/term.dict")
		 ,attv_dict_(dir+"/att.nv.score")
		 ,att_dict_(dir+"/att.n.score")
		,syn_dict_(dir + "/syn.dict", 2)
    {
    }

    ~AttributeTokenize()
    {
    }

	std::vector<std::pair<std::string, double> >
		tokenize(const std::string& title, const std::string& attr, 
					const std::string& cate, const std::string& ocate,
					const std::string& source)
    {
		std::vector<std::pair<KString, double> > rr;
		std::vector<KString> kattrs = KString(attr).split(',');

		double max_avs = attv_dict_.score(KString("[title]"));
		KString subcate = sub_cate_(cate);
		const double hyper_p = sqrt(std::min(size_t(15), kattrs.size())/15.);

		rr.push_back(make_pair(normallize_(title), max_avs));
		rr.push_back(make_pair(normallize_(cate), max_avs));
		rr.push_back(make_pair(normallize_(source), max_avs));

		for ( uint32_t i=0; i<kattrs.size(); ++i)
		{
			std::vector<KString> p = kattrs[i].split(':');
			if (p.size() != 2 || p[0].length()==0 || p[1].length() == 0)
			  continue;
			KString av = p[0];av+='@',av+=subcate,av+=':',av+=p[1];
			double avs = attv_dict_.score(av);

			rr.push_back(make_pair(p[1], avs));
			rr.push_back(make_pair(p[0], avs*hyper_p));
		}
		rr.push_back(make_pair(normallize_(sub_cate_(ocate,false)), max_avs));
		return token_(rr);
    }

    //query
	std::vector<std::string> tokenize(const std::string& Q)
	{
		KString	q = normallize_(Q);
        std::set<KString > set = token_(q);
        std::vector<std::string> r;
        const std::set<KString>::iterator set_end = set.end();
        for (std::set<KString>::iterator it=set.begin(); it!=set_end; ++it)
            r.push_back(unicode_to_utf8_(*it));
		return r;
	}

	std::vector<std::string> subtokenize(const std::vector<std::string>& tks)
	{
		std::vector<std::string> r;
		for ( uint32_t i=0; i<tks.size(); ++i)
		{
            std::vector<KString> v = token_dict_.sub_token(KString(tks[i]), 0);
			for ( uint32_t j=0; j<v.size(); ++j)
		        r.push_back(unicode_to_utf8_(v[j]));
//			   r.push_back(v[j].get_bytes("utf-8"));
		}
		return r;
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

