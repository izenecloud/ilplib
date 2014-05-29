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

    ConditionItem(){}

    ConditionItem(std::string property, 
                  std::string op,
                  std::vector<PropertyValue> values)
      : property_(property)
      , op_(op)
      , values_(values)
    {
    }

    ConditionItem(const std::string& property, 
                  const std::string& op,
                  const float& v)
      : property_(property)
      , op_(op)
    {
        values_.push_back(PropertyValue(v));
    }

    ConditionItem(const std::string& property, 
                  const std::string& op,
                  const int32_t& v)
      : property_(property)
      , op_(op)
    {
        values_.push_back(PropertyValue(v));
    }

    ConditionItem(const std::string& property, 
                  const std::string& op,
                  const std::string& v)
      : property_(property)
      , op_(op)
    {
        values_.push_back(PropertyValue(v));
    }

    ConditionItem& operator = (const ConditionItem& it)
    {
        property_ = it.property_;
        op_ = it.op_;
        values_ = it.values_;
        return *this;
    }

    ConditionItem(const ConditionItem& it)
      :property_(it.property_)
       ,op_(it.op_)
       ,values_(it.values_)
    {
    }

    friend std::ostream& operator << (std::ostream& os, ConditionItem cnd)
    {
        os << "{ \"property\":\""<<cnd.property_<<"\", \"operator\":\""<<cnd.op_<<"\", \"value\": [";
        for(uint32_t i = 0;i<cnd.values_.size();i++){
            if (i>0)os <<",";
            if (cnd.values_[i].type() == typeid(std::string("")))os <<"\"";
            os <<cnd.values_[i];
            if (cnd.values_[i].type() == typeid(std::string("")))os <<"\"";
        }
        os <<"]}\n";
        return os;
    }
};

class KeywordCondition{
    KDictionary<const char*> price_dict_;
    KDictionary<const char*> cmt_dict_;
    KDictionary<const char*> bigram_dict_;
    KDictionary<const char*> unigram_dict_;
    KDictionary<const char*> cate_dict_;
    KDictionary<const char*> merchant_dict_;
    KDictionary<const char*> compare_dict_;

    std::vector<ConditionItem> source_in_()
    {
        std::vector<PropertyValue> values_DATA;
        values_DATA.push_back(PropertyValue("京东商城"));
        values_DATA.push_back(PropertyValue("卓越亚马逊"));
        values_DATA.push_back(PropertyValue("苏宁易购"));
        values_DATA.push_back(PropertyValue("当当网"));
        values_DATA.push_back(PropertyValue("天猫"));
        values_DATA.push_back(PropertyValue("1号店官网"));
        values_DATA.push_back(PropertyValue("国美电器官网"));
        values_DATA.push_back(PropertyValue("易迅网"));

        std::vector<ConditionItem> r;r.push_back(ConditionItem("Source", "in", values_DATA));
        return r;
    }

    std::vector<ConditionItem> itemcount_()
    {
        std::vector<ConditionItem> r;
        r.push_back(ConditionItem("itemcount", "=", 1));
        return r;
    }

    std::vector<std::string> lookup_(const std::string& kw, KDictionary<const char*>* dict, uint32_t max=-1)
    {
        std::vector<std::string> r;
        const char* p = NULL;
        if (dict->value(kw, p, false)==0){
            assert(p);
            std::string strp(p);
            const char* t = strchr(strp.c_str(), '\t');
            while(t && r.size() < max)
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

    // cond[a]: a1, a2, c3, c4
    // cond[b]: b1, b2, b3, b4
    // cond[c]: c1, c2, c3, c4
    // return: <a1, b1, c1> <a1, b1, c2> <a1, b1, c3>...<a2, b1, c1> <a2, b2, c2> ....
    void combination_(const std::vector<std::vector<ConditionItem> >& conds, std::vector<ConditionItem> comb, 
      std::vector<std::vector<ConditionItem> >& r, uint32_t level=0)
    {
        if (level >= conds.size())
        {
            r.push_back(comb);return;
        }

        for (uint32_t i=0;i<conds[level].size();i++){
            std::vector<ConditionItem> c = comb;
            if(conds[level][i].property_.length()>0)
                c.push_back(conds[level][i]);
            combination_(conds, c, r, level+1);
        }
    }

    std::string normalize_(const std::string& q)
    {
        std::string kw;
        for (uint32_t i=0;i<q.length();i++)
            if (q[i]>='A' && q[i]<='Z')kw += char(int(q[i])-(int('A')-int('a')));
            else if(q[i]!=' ')kw += q[i];
        return kw;
    }

    std::vector<std::string> normalize_(const std::vector<std::string>& strs)
    {
        std::vector<std::string> r;
        for (uint32_t i=0;i<strs.size();i++)
        {
            uint32_t en = 0, nu = 0;
            std::string str;if (strs[i].length()>0)str += strs[i][0];
            for (uint32_t j=1;j<strs[i].length();j++)
            {
                if (isalpha(strs[i][j]))en++;
                if(isdigit(strs[i][j]))nu++;
                //9300galaxy ==> 9300 galaxy
                if (isalpha(strs[i][j]) && isdigit(strs[i][j-1]))
                {
                    if (nu > 2)
                        str += ' ';
                    nu = 0;
                }
                //iphone5s ==> iphone 5s
                if (isalpha(strs[i][j-1]) && isdigit(strs[i][j]))
                {
                    if (en > 2)
                        str += ' ';
                    en = 0;
                }
                str += strs[i][j];
            }
            r.push_back(str);
        }
        return r;
    }

public:
    KeywordCondition(const std::string& dir)
      :price_dict_(dir+"/price.dict")
       ,cmt_dict_(dir+"/comment.dict")
       ,bigram_dict_(dir + "/bigram.dict")
       ,unigram_dict_(dir + "/unigram.dict")
       ,cate_dict_(dir + "/cate.dict")
       ,merchant_dict_(dir + "/merchant.dict")
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

    std::vector<std::pair<std::string, std::vector<ConditionItem> > >
      conditions(std::string kw,
                  bool hasPriceFilter = false, 
                  bool hasCategoryFilter = false, 
                  bool hasSourceFilter = false)
      {
          std::string query = kw;
          kw = normalize_(kw);

          std::vector<std::string> v = lookup_(kw, &bigram_dict_, 3);
          for (uint32_t i=0;i<v.size();i++)v[i]=query+" "+v[i];
          std::vector<std::string> exp(v.begin(), v.end());
          v = lookup_(kw, &unigram_dict_, 1);
          for (uint32_t i=0;i<v.size();i++)v[i]=query+" "+v[i];
          exp.insert(exp.end(), v.begin(), v.end());
          //exp = normalize_(exp);
          //exp.push_back(kw);

          std::vector<std::vector<ConditionItem> > conds;
          conds.push_back(itemcount_());

          std::vector<ConditionItem> cond_items;
          v = lookup_(kw, &merchant_dict_);cond_items.clear();
          if (!hasSourceFilter)
          {
            for (uint32_t i=0;i<v.size();i++)
                cond_items.push_back(ConditionItem("Source", "starts_with", v[i]));
            if (cond_items.size()){
                conds.push_back(cond_items);
                goto __COMBINE__;
            }
            else conds.push_back(source_in_());
          }

          v = lookup_(kw, &price_dict_);cond_items.clear();
          if (!hasPriceFilter)
          {
            for (uint32_t i=0;i<v.size();i++)
              cond_items.push_back(ConditionItem("Price", ">=", int32_t(atof(v[i].c_str())+0.5)));
            if (cond_items.size()) conds.push_back(cond_items);
          }
          
          v = lookup_(kw, &cmt_dict_);cond_items.clear();
          for (uint32_t i=0;i<v.size();i++)
              cond_items.push_back(ConditionItem("CommentCount", ">=", int32_t(atoi(v[i].c_str()))));
          if (cond_items.size()){
              cond_items.push_back(ConditionItem());
              conds.push_back(cond_items);
          }

          v = lookup_(kw, &cate_dict_);cond_items.clear();
          if (!hasCategoryFilter)
          {
            for (uint32_t i=0;i<v.size();i++)
                cond_items.push_back(ConditionItem("Category", "starts_with", v[i]));
            if (cond_items.size()) conds.push_back(cond_items);
          }

__COMBINE__:
          std::vector<std::pair<std::string, std::vector<ConditionItem> > > r;
          std::vector<std::vector<ConditionItem> > comb;
          combination_(conds, std::vector<ConditionItem>(), comb);
          /*{
              std::vector<ConditionItem> s = source_in_();
              std::vector<ConditionItem> it = itemcount_();
              s.insert(s.end(), it.begin(), it.end());
              comb.push_back(s);
          }

          v = lookup_(kw, &compare_dict_);cond_items.clear();
          if (v.size() > 0)
          {
              cond_items.push_back(ConditionItem("itemcount", ">", 1));
              r.push_back(std::make_pair(kw, cond_items));
          }*/

          for (uint32_t i=0;i<exp.size();i++)
              for(uint32_t j=0;j<comb.size();j++)
              {
                  r.push_back(std::make_pair(exp[i], comb[j]));
                  std::cout<<exp[i]<<"=============\n";
                  for (uint32_t t=0;t<comb[j].size();t++)
                      std::cout<<comb[j][t];
              }

          return r;
      }
};
}
}

#endif
