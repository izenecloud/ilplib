/*
 * =====================================================================================
 *
 *       Filename:  string_patterns.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013年06月17日 17时46分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), hukai.kevin@b5m.com
 *        Company:  b5m.com
 *
 * =====================================================================================
 */

#ifndef _ILPLIB_NLP_STRING_PATTERNS_H_
#define _ILPLIB_NLP_STRING_PATTERNS_H_

#include <vector>
#include <string>

#include "knlp/tokenize.h"
#include "am/util/line_reader.h"

#include <boost/regex.hpp>    
using namespace izenelib::am::util;
using namespace izenelib;
namespace ilplib{
	namespace knlp{

class StringPatterns
{

	public:
	    static void erase(KString& kstr, uint16_t from_c, uint16_t to_c)
        {
            uint32_t s = kstr.index_of(from_c);
            if (s == (uint32_t)-1)return;
            uint32_t e = kstr.index_of(to_c, s);
            if (e == (uint32_t)-1)return;
            KString r = kstr.substr(0, s);
            r += kstr.substr(e+1);
            kstr = r;
        }

		static int32_t string_type(const KString& kstr)
		{
			bool eng = false, num = false, chn=false, oth=false;
			for ( uint32_t i=0; i<kstr.length(); ++i)
			  if (std::isalpha(kstr[i]))
				eng = true;
			  else if(ilplib::knlp::Tokenize::is_digit(kstr[i]))
				num = true;
			  else if (KString::is_chinese(kstr[i]))
				chn = true;
			  else if (kstr[i]!=' ')
				oth = true;
			if (eng && (!num) && (!chn) && (!oth))
			  return 1;//all english char
			if (num && (!eng) && (!chn) && (!oth))
			  return 2;//all digital char
			if (eng && num && (!chn))
			  return 3;//eng and digit mixed
			if (oth && (!chn))
			  return 4;//punctuation
			return -1;
		}

};

class GarbagePattern
{
    std::vector<boost::regex> regs_;
    std::vector<std::string>  stopwds_;

public:
    GarbagePattern(const std::string& nm)
    {
        bool stop = false;
        LineReader lr(nm);
        char* line = NULL;
        while((line=lr.line(line))!=NULL)
        {
            if (strlen(line) == 0)continue;
            if (strcmp(line, "##############################") == 0)
            {
                stop = true;
                continue;
            }
            if (stop)
            {
                stopwds_.push_back(line);
                continue;
            }
            try{
                regs_.push_back(boost::regex(line));
            }catch(...)
            {
                std::cout<<"Regex compile ERROR: "<<line<<std::endl;
            }
        }
    }

    std::string clean(const std::string& str)
    {
        std::string last = str;
        std::string r = last;
        do{
            last = r;
            for (uint32_t i=0; i<regs_.size(); ++i)
                r = boost::regex_replace(r, regs_[i], " ");
            for (uint32_t i=0;i<stopwds_.size();i++)
            {
                const char* f = strstr(r.c_str(), stopwds_[i].c_str());
                if (!f)continue;
                r = r.substr(0, f-r.c_str())+std::string(" ")+(f+stopwds_[i].length());
            }
        }while(r.length()>0 && strcmp(last.c_str(), r.c_str())!=0);

        for (uint32_t i=0;i<r.length();++i)
            if (r[i]!=' ')
                return r;
        return str;
    }

    std::string erase_stop_word(const std::string& str)
    {
        std::string last = str;
        std::string r = last;
        do{
            last = r;
            for (uint32_t i=0;i<stopwds_.size();i++)
            {
                const char* f = strstr(r.c_str(), stopwds_[i].c_str());
                if (!f)continue;
                r = r.substr(0, f-r.c_str())+std::string(" ")+(f+stopwds_[i].length());
            }
        }while(r.length()>0 && strcmp(last.c_str(), r.c_str())!=0);

        for (uint32_t i=0;i<r.length();++i)
            if (r[i]!=' ')
                return r;
        return str;

    }
};

}}
#endif

