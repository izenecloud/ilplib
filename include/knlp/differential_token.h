#ifndef _ILPLIB_NLP_DIFFERENTIAL_TOKEN_H_
#define _ILPLIB_NLP_DIFFERENTIAL_TOKEN_H_

#include <string>
#include<iostream>
#include<fstream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>

#include "knlp/horse_tokenize.h"


namespace ilplib
{
namespace knlp
{
class DifferentialToken {
    HorseTokenize token_;

    std::vector<std::string> top_(std::vector<std::pair<std::string, float> > tks, float th=0.5)
    {
        {//dup
            std::set<std::string> s;
            for (uint32_t i=0;i<tks.size();i++)if(s.find(tks[i].first) == s.end())
                s.insert(tks[i].first);
            else{
                tks.erase(tks.begin() + i);
                i--;
            }
        }

        float sum = 0;
        for (uint32_t i=0;i<tks.size();i++)
            sum += tks[i].second;
        for (uint32_t i=0;i<tks.size();i++)
            tks[i].second /= sum;
        std::vector<std::pair<float, uint32_t> > v;
        for (uint32_t i=0;i<tks.size();i++)
            v.push_back(std::make_pair(tks[i].second, i));

        std::sort(v.begin(), v.end(), std::greater<std::pair<float, uint32_t> >());
        sum = 0;
        std::vector<std::string> r;
        for (uint32_t i=0;i<v.size();i++)if(sum <= th)
        {
            assert(v[i].second < tks.size());
            r.push_back(tks[v[i].second].first);
            sum += v[i].first;
        }
        return r;
    }

public:
    DifferentialToken(const std::string& dict_path) 
    :token_(dict_path)
    {
    }

    ~DifferentialToken() {
    }

    std::vector<std::string> different(
      const std::vector<std::string>& titles, 
      const std::vector<float>& prices, 
      const std::vector<uint32_t>& clicked) 
    {
        assert(titles.size() == prices.size());
        if (clicked.size() == 0)return std::vector<std::string>();

        uint32_t max_clicked = *std::max_element(clicked.begin(), clicked.end());
        assert(max_clicked < titles.size());
        std::map<std::string, int32_t> cli_tk_freq, noncli_tk_freq;
        std::set<uint32_t> cl(clicked.begin(), clicked.end());
        for (uint32_t i=0; i<titles.size();i++)if(i <= max_clicked)
        {
            std::vector<std::pair<std::string, float> > tks;
            token_.tokenize(titles[i], tks);
            std::vector<std::string> tops = top_(tks, 0.5);
            std::map<std::string, int32_t>* map = NULL;
            if (cl.find(i) == cl.end())
                map = &noncli_tk_freq;
            else map = &cli_tk_freq;
            assert(map);
            for (uint32_t t = 0; t<tops.size();t++)
            {
                if (map->find(tops[t]) == map->end())
                    map->insert(std::make_pair(tops[t], 0));
                (*map)[tops[t]] += 1;
            }
        }

        std::vector<std::pair<int32_t,std::string> > v;
        for (std::map<std::string, int32_t>::iterator it=cli_tk_freq.begin();
          it != cli_tk_freq.end(); it++)
            if (noncli_tk_freq.find(it->first) != noncli_tk_freq.end())
                v.push_back(std::make_pair(it->second - noncli_tk_freq[it->first], it->first));
            else v.push_back(std::make_pair(it->second,it->first));

        std::sort(v.begin(), v.end(), std::greater<std::pair<int32_t, std::string> >());

        std::vector<std::string> r;
        for(uint32_t i=0;i<v.size();i++)
            r.push_back(v[i].second);

        return r;
    }
    
};


}
}

#endif
