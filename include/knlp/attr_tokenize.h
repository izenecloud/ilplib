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
	Dictionary syn_dict_;

	KString sub_cate_(const std::string& cate, bool g=true)
	{
		if (cate.length() == 0)return KString();
		KString r(cate);
		int32_t i=r.length()-1;
		if (r[i] == '>')i--;
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

	std::vector<KString> token_(const KString& kstr)
	{
		std::vector<std::pair<uint32_t, uint32_t> > pos;
		bool f = false;
		for ( uint32_t i=0;i<kstr.length(); ++i)
		  if (is_model_type_(kstr[i]))
		  {
			  if (f)continue;
			  pos.push_back(make_pair(i, -1));
			  f = true;
  		  }else{
			  if (f){
				  assert(pos.size());
				  assert(pos.back().second == (uint32_t)-1);
				  assert(pos.back().first < i);
				  pos.back().second = i;
			  }
			  f = false;
		  }
		if (f)pos.back().second = kstr.length();

		std::vector<KString> chunks;
		uint32_t last = 0;
		for ( uint32_t i=0; i<pos.size(); ++i)
		{
			if (last < pos[i].first)
			  chunks.push_back(kstr.substr(last, pos[i].first - last));
			last = pos[i].second;
		}
		if (last < kstr.length())
		  chunks.push_back(kstr.substr(last));

		std::vector<KString> r;
		for ( uint32_t i=0; i<chunks.size(); ++i)
		{
			std::vector<std::pair<KString, double> > v = token_dict_.token(chunks[i]);
			for ( uint32_t j=0; j<v.size(); ++j)
			{
				const char* syn = syn_dict_.value(v[j].first);
				if (syn)v[j].first = KString(syn);
				r.push_back(v[j].first);
			}
		}
		for ( uint32_t i=0; i<pos.size(); ++i)
		{
			assert(pos[i].second >= pos[i].first);
			KString sub = kstr.substr(pos[i].first, pos[i].second-pos[i].first);
			const char* syn = syn_dict_.value(sub);
			if (syn)sub = KString(syn);
			std::vector<std::pair<KString, double> > v = token_dict_.token(sub);
			if (v.size() ==0 || v[0].first.length() < 3)
  			  r.push_back(sub);
			else for ( uint32_t j=0; j<v.size(); ++j)
			  r.push_back(v[j].first);
		}
		for ( uint32_t i=0; i<r.size(); ++i)
		  if (r[i].length() == 0 
				|| (r.size() > 1 && r[i].length() == 1 
			&& ((!KString::is_english(r[i][0]) && !KString::is_numeric(r[i][0]) && !KString::is_chinese(r[i][0]))))
			)
			r.erase(r.begin()+i),--i;
		return r;
	}

	std::vector<std::pair<std::string, double> >
		token_(const std::vector<std::pair<KString, double> >& av)
		{
			std::map<std::string, double> m;
			for ( uint32_t i=0; i<av.size(); ++i)
			{
				std::vector<KString> tks = token_(av[i].first);
				std::set<KString> set(tks.begin(), tks.end());
				for ( std::set<KString>::iterator it=set.begin(); it!=set.end(); ++it)
				  m[it->get_bytes("utf-8")] += av[i].second/sqrt(set.size());
			}

			std::vector<std::pair<std::string, double> > r;
			for (std::map<std::string, double>::iterator it=m.begin(); it!=m.end(); ++it)
  				r.push_back(make_pair(it->first, it->second*100.));
			return r;
		}
public:

    AttributeTokenize(const std::string& dir)
        :token_dict_(dir+"/term.dict")
		 ,attv_dict_(dir+"/att.nv.score")
		 ,att_dict_(dir+"/att.n.score")
		,syn_dict_(dir + "/syn.dict")
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
		const double hyper_p = sqrt(kattrs.size()/40.);

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
		rr.push_back(make_pair(normallize_(title), max_avs));
		rr.push_back(make_pair(normallize_(cate), max_avs));
		rr.push_back(make_pair(normallize_(sub_cate_(ocate,false)), max_avs));
		rr.push_back(make_pair(normallize_(source), max_avs));

		return token_(rr);
    }

	std::vector<std::string> tokenize(const std::string& Q)
	{
		KString	q = normallize_(Q);
		std::vector<KString> v = token_(q);
		std::vector<std::string> r;
		for ( uint32_t i=0; i<v.size(); ++i)
		  r.push_back(v[i].get_bytes("utf-8"));
		return r;
	}

	std::vector<std::string> subtokenize(const std::vector<std::string>& tks)
	{
		std::vector<std::string> r;
		for ( uint32_t i=0; i<tks.size(); ++i)
		{
			 std::vector<std::pair<KString, double> > v = token_dict_.sub_token(KString(tks[i]));
			 for ( uint32_t j=0; j<v.size(); ++j)
			   r.push_back(v[j].first.get_bytes("utf-8"));
		}
		return r;
	}

	double att_weight(const std::string& nm, const std::string& cate)
    {
        std::string att = nm;
        nm += '@';
        nm += sub_cate_(cate);
        return att_dict_.score(KString(nm));
    }

};

}
}//namespace

#endif

