#ifndef _ILPLIB_NLP_KEYWORD_CONDITION_H_
#define _ILPLIB_NLP_KEYWORD_CONDITION_H_

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
#include <ctime>
#include <boost/algorithm/string.hpp>

#include "util/string/kstring.hpp"
#include "normalize.h"
#include "kdictionary.h"
#include "am/util/line_reader.h"
#include <sf1common/PropertyValue.h>

using namespace izenelib;
namespace ilplib
{
namespace knlp
{
struct ConditionItem
{
    std::string property_;
    std::string op_;
    std::vector<PropertyValue> values_;
    ConditionItem(std::string property, 
                  std::string op,
                  std::vector<PropertyValue> values)
      : property_(property)
      , op_(op)
      , values_(values)
    {
    }
};

class KeywordCondition{
    KDictionary<const char*> price_dict_;
    KDictionary<const char*> cmt_dict_;
    KDictionary<const char*> bigram_dict_;
    KDictionary<const char*> unigram_dict_;
    KDictionary<const char*> cate_dict_;
    KDictionary<const char*> compare_dict_;

    std::string static_condition_(std::vector<ConditionItem> &condItems)
    {
        /*std::vector<PropertyValue> values;
        PropertyValue pv(10);
        values.push_back(pv);
        ConditionItem item1("CommentCount", ">", values);
        condItems.push_back(item1);
        std::string cmt = "{\"property\":\"CommentCount\",\"operator\":\">\",\"value\":[10]},";
*/
        time_t t = time(NULL)-3600*24*5;   
        char buf[255];memset(buf, 0, sizeof(buf));
        strftime(buf, 255, "%Y%m%dT%H%M%S", localtime(&t)); 
        std::vector<PropertyValue> values_DATA;
        PropertyValue pv(buf);
        values_DATA.push_back(pv);
        ConditionItem item2("DATE", ">=", values_DATA);
        condItems.push_back(item2);
        std::string dt = std::string("{\"property\":\"DATE\",\"operator\":\">=\",\"value\":[")+buf+"]}";
        return dt;
    }

    std::vector<std::string> lookup_(const std::string& kw, KDictionary<const char*>* dict)
    {
        std::vector<std::string> r;
        const char* p = NULL;
        if (dict->value(kw, p, false)==0){
            assert(p);
            std::string strp(p);
            const char* t = strchr(strp.c_str(), '\t');
            while(t)
            {
                r.push_back(strp.substr(0, t-strp.c_str()));
                strp = t+1;
                t = strchr(strp.c_str(), '\t');
            }
            if (strp.length() > 0)
                r.push_back(strp);
        }
        return r;
    }

    void combination_(const std::vector<std::vector<std::string> >& conds, std::string comb, 
      std::vector<std::string>& r, uint32_t level=0)
    {
        if (level >= conds.size())
        {
            r.push_back(comb);return;
        }

        for (uint32_t i=0;i<conds[level].size();i++)if(conds[level][i].length()>0)
            combination_(conds, comb+","+conds[level][i], r, level+1);
    }

    std::string normalize_(const std::string& q)
    {
        std::string kw;
        for (uint32_t i=0;i<q.length();i++)
            if (q[i]>='A' && q[i]<='Z')kw += char(int(q[i])-(int('A')-int('a')));
            else if(q[i]!=' ')kw += q[i];
        return kw;
    }

public:
    KeywordCondition(const std::string& dir)
      :price_dict_(dir+"/price.dict")
       ,cmt_dict_(dir+"/comment.dict")
       ,bigram_dict_(dir + "/bigram.dict")
       ,unigram_dict_(dir + "/unigram.dict")
       ,cate_dict_(dir + "/cate.dict")
       ,compare_dict_(dir + "/compare.dict")
    {
    }

    std::string compare_price(std::string kw, bool& compare_first)
    {
          kw = normalize_(kw);
          compare_first = false;
          std::vector<std::string> v = lookup_(kw, &compare_dict_);
          if (v.size() > 0)compare_first = true;
          if (v.size() > 0)return "{\"property\":\"itemcount\",\"operator\":\">\",\"value\":[1]}";
          return "{\"property\":\"itemcount\",\"operator\":\"=\",\"value\":[1]}";
    }

    std::vector<std::pair<std::string, std::string> >
      conditions(std::string kw, std::vector<ConditionItem> &condItems,
                  bool hasPriceFilter = false, bool hasCategoryFilter = false)
      {
          kw = normalize_(kw);

          std::vector<std::string> v = lookup_(kw, &bigram_dict_);
          for (uint32_t i=0;i<v.size();i++)v[i]=kw+" "+v[i];
          std::vector<std::string> exp(v.begin(), v.end());
          v = lookup_(kw, &unigram_dict_);
          for (uint32_t i=0;i<v.size();i++)v[i]=kw+" "+v[i];
          exp.insert(exp.end(), v.begin(), v.end());
          exp.push_back(kw);

          // ConditionItem
          std::vector<std::vector<std::string> > conds;
          v = lookup_(kw, &price_dict_);
          if (!hasPriceFilter)
          {
            for (uint32_t i=0;i<v.size();i++)
            {
              std::vector<PropertyValue> values;
              PropertyValue pv(v[i]);
              values.push_back(pv);
              ConditionItem item("Price", "operator", values);
              condItems.push_back(item);
              
              v[i] = std::string("{\"property\":\"Price\",\"operator\":\">=\",\"value\":[")+v[i]+"]}";
            }
            if (v.size()) conds.push_back(v);
          }
          
          v = lookup_(kw, &cmt_dict_);
          for (uint32_t i=0;i<v.size();i++)
          {
              std::vector<PropertyValue> values;
              PropertyValue pv(v[i]);
              values.push_back(pv);
              ConditionItem item("CommentCount", "operator", values);
              condItems.push_back(item);

              v[i] = std::string("{\"property\":\"CommentCount\",\"operator\":\">=\",\"value\":[")+v[i]+"]}";
          }
          if (v.size()) conds.push_back(v);

          v = lookup_(kw, &cate_dict_);
          if (!hasCategoryFilter)
          {
            for (uint32_t i=0;i<v.size();i++)
            {
                std::vector<PropertyValue> values;
                PropertyValue pv(v[i]);
                values.push_back(pv);
                ConditionItem item("Category", "starts_with", values);
                condItems.push_back(item);
                v[i] = std::string("{\"property\":\"Category\",\"operator\":\"starts_with\",\"value\":[")+v[i]+"]}";
            }
            if (v.size()) conds.push_back(v);
          }

          std::vector<std::string> comb;
          combination_(conds, static_condition_(condItems), comb);
          std::vector<std::pair<std::string, std::string> > r;

          for (uint32_t i=0;i<exp.size();i++)
              for(uint32_t j=0;j<comb.size();j++)
                  r.push_back(std::make_pair(exp[i], comb[j]));
          return r;
      }
};
}
}

#endif
