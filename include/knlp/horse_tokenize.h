#ifndef _ILPLIB_NLP_HORSE_TOKENIZE_H_
#define _ILPLIB_NLP_HORSE_TOKENIZE_H_

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
#include <boost/algorithm/string.hpp>

#include "util/string/kstring.hpp"
#include "normalize.h"
#include "kdictionary.h"
#include "am/util/line_reader.h"

namespace ilplib
{
namespace knlp
{
class HorseTokenize{
    KDictionary<float> tk_dict_;
    KDictionary<const char*> rewrite_dict_;

    bool is_digit_(char c)const
    {
        if (c >= '0' && c<='9')
            return true;
        if (c == '.' || c == '-' || c == '+' || c == '/' || c == '=' || c== '*' || c== '%'
          || c == ',' || c == '$' || c == '&' || c == '_')
            return true;
        return false;
    }

    void merge_(std::vector<std::pair<std::string, float> >& tks)const
    {
        std::vector<int32_t> flags(tks.size(), 0);
        for (uint32_t i=0;i<tks.size();i++)
        {
            uint32_t j=0;
            for (;j<tks[i].first.length();j++)
                if (is_digit_(tks[i].first[j])
                  ||(tks[i].first[j]>='a' && tks[i].first[j]<='z'))
                    break;
            if (j < tks[i].first.length())flags[i] = 1;
        }

        for (uint32_t i=0;i<tks.size();i++)if(flags[i])
        {
            uint32_t t = i;
            i++;
            while(i<tks.size() && flags[i])
            {
                tks[t].first += tks[i].first;
                tks[t].second += tks[i].second;
                tks.erase(tks.begin()+i);
                flags.erase(flags.begin()+i);
            }
        }
    }

    void rewrite_(std::vector<std::pair<std::string, float> >& tks)const
    {
        for (uint32_t i=0;i<tks.size();i++)
        {
            const char* p = NULL;
            if (rewrite_dict_.value(tks[i].first, p, false)!=0)
                continue;
            assert(p != NULL);
            tks[i].first = std::string(p);
        }
        for (uint32_t i=0;i<tks.size();i++)
            if (tks[i].first == " ")
            {
                tks.erase(tks.begin()+i);
                i--;
            }
    }

public:
    HorseTokenize(const std::string& dir)
      :tk_dict_(dir+"/token.dict")
       ,rewrite_dict_(dir + "/rewrite.dict")
    {
    }

    void tokenize(const std::string& line, 
      std::vector<std::pair<std::string, float> >& tks)const
    {
        tk_dict_.fmm(line, tks);
        //for(uint32_t i=0;i<tks.size();i++)std::cout<<tks[i].first<<"oooooo"<<tks[i].second<<std::endl;
        merge_(tks);
        rewrite_(tks);
    }

    void subtokenize(const std::vector<std::pair<std::string, float> >& tks,
      std::vector<std::pair<std::string, float> >& subs)const
    {
        tk_dict_.subtokens(tks, subs);
        for (uint32_t i=0;i<subs.size();++i)
        {
            std::vector<std::string> v;
            boost::split(v, subs[i].first, boost::is_any_of("-, /"));
            std::vector<std::pair<std::string, float> > s;
            for (uint32_t j=0;j<v.size();++j)
                s.push_back(std::make_pair(v[j], subs[i].second));
            if (s.size() < 2)continue;
            subs.insert(subs.begin()+i+1, s.begin(), s.end());
            subs.erase(subs.begin()+i);
            i += s.size() -1;
        }
    }

    float max()const
    {
        return tk_dict_.max();
    }
};
}
}

#endif
