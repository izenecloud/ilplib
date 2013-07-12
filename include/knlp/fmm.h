/*
 * =====================================================================================
 *
 *       Filename:  tokenize.h
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
#ifndef _ILPLIB_NLP_FMM_H_
#define _ILPLIB_NLP_FMM_H_

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

#include "util/string/kstring.hpp"
#include "trd2simp.h"
#include "am/util/line_reader.h"
#include "william_trie.h"

namespace ilplib
{
namespace knlp
{

class Fmm
{
    WilliamTrie trie_;
    std::vector<uint8_t> delimiter_;

    std::vector<KString> chunk_(const KString& line, bool del=true)
    {
        std::vector<KString> r;

        int32_t la = 0;
        for ( int32_t i=0; i<(int32_t)line.length(); ++i)
		  if (KString::is_english(line[i]) || is_digit_(line[i]))
		  {
			  if (la < i)
			  {
				  r.push_back(line.substr(la, i-la));
				  la = i;
			  }
			  while(i<(int32_t)line.length() && (KString::is_english(line[i]) || is_digit_(line[i])))
				i++;
			  if (i -la <= 1)
              {
                  --i;
                  continue;
              }
			  r.push_back(line.substr(la, i-la));
			  la = i;
			  i--;
		  }
		  else if (del && delimiter_[line[i]])
            {
                //std::cout<<line.substr(la, i-la+1)<<"OOOOOO\n";
                if (i > la)r.push_back(line.substr(la, i-la));
                r.push_back(line.substr(i, 1));
                la = i+1;
            }

        if (la < (int32_t)line.length())
            r.push_back(line.substr(la));
		//for ( uint32_t i=0; i<r.size(); ++i)
		  //std::cout<<r[i]<<std::endl;
        return r;
    }

    bool is_digit_(int c)const
    {
        return is_digit(c);
    }

	bool is_alphanum_(const KString& str)
	{
		for ( uint32_t i=0; i<str.length(); ++i)
		  if (!(is_digit_(str[i])||KString::is_english(str[i])))
			return false;
		return true;
	}

public:

    Fmm(const std::string& dict_nm)
        :trie_(dict_nm)
    {
        KString ustr("~！@#￥……&*（）—+【】{}：“”；‘’、|，。《》？ ^()-_=[]\\|;':\"<>?/　");
        delimiter_.resize((int)((uint16_t)-1), 0);
        for ( uint32_t i=0; i<ustr.length(); ++i)
            delimiter_[ustr[i]] = 1;
    }

    ~Fmm()
    {
    }

    static bool is_digit(int c)
    {
        return (std::isdigit(c)||c=='.'||c=='%'||c=='$'||c==','||c=='-');
    }

    bool is_in(const KString& kstr)
    {
        return trie_.check_term(kstr);
    }

    double score(const KString& kstr)
    {
        return trie_.score(kstr);
    }

	void fmm(const KString& line, std::vector<std::pair<KString,double> >& r, bool smart=true, bool bigterm=true)//forward maximize match
	{
		r.clear();
		if (line.length() == 0)return;

		std::vector<KString> chunks = chunk_(line, (!bigterm));
		for ( uint32_t i=0; i<chunks.size(); ++i)
		{
			if (smart && (is_alphanum_(chunks[i]) || chunks[i].length() < 3
			  || (chunks[i].length() == 3 && KString::is_chinese(chunks[i][0]))))
			{
                //std::cout<<"::"<<chunks[i]<<"::\n";
  				r.push_back(make_pair(chunks[i], trie_.score(chunks[i])));
				continue;
			}
			std::vector<std::pair<KString,double> > v = trie_.token(chunks[i]);
			r.insert(r.end(), v.begin(), v.end());
		}
		for (uint32_t i=0;i<r.size();++i)
        {
            r[i].first.trim();
            if (r[i].first.length() > 0)
                continue;
            r.erase(r.begin()+i);
            --i;
        }
	}


	std::vector<KString> fmm(const KString& line, bool smart=true, bool bigterm=true)//forward maximize match
	{
		std::vector<std::pair<KString,double> > v;
		fmm(line, v, smart, true);
		std::vector<KString> r;
		for ( uint32_t i=0; i<v.size(); ++i)
		  r.push_back(v[i].first);
		return r;
	}

    uint32_t size()const
    {
        return trie_.size();
    }

    double min()const
    {
        return trie_.min();
    }
};

}
}//namespace

#endif

