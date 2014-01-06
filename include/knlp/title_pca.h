#ifndef _ILPLIB_NLP_TITLE_PCA_H_
#define _ILPLIB_NLP_TITLE_PCA_H_

#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <istream>
#include <ostream>
#include <vector>
#include <cctype>
#include <math.h>
#include <boost/algorithm/string.hpp>

#include "util/string/kstring.hpp"
#include "normalize.h"
#include "kdictionary.h"
#include "am/util/line_reader.h"
#include "horse_tokenize.h"
#include"re2/re2.h"

namespace ilplib
{
namespace knlp
{

class TitlePCA{
    HorseTokenize token_;
    KDictionary<> brand_;
    std::vector<re2::RE2*> reg_;

    bool is_digit_(char c)const
    {
        if (c >= '0' && c<='9')
            return true;
        if (c == '.' || c == '-' || c == '+' || c == '/' || c == '=' || c== '*' || c== '%'
          || c == ',' || c == '$' || c == '&' || c == '_')
            return true;
        return false;
    }

    bool model_type_(const std::string& m)const
    {
        if (!m.empty() && (int)(m[0])<0)
            return false;
        if (!re2::RE2::FullMatch(m, *reg_[0])
          && re2::RE2::FullMatch(m, *reg_[1])
          &&(re2::RE2::FullMatch(m, *reg_[2])
            ||(re2::RE2::PartialMatch(m, *reg_[3]) && re2::RE2::FullMatch(m, *reg_[4]))
            ||(re2::RE2::PartialMatch(m, *reg_[5]) && re2::RE2::FullMatch(m, *reg_[6])) 
            ||(re2::RE2::PartialMatch(m, *reg_[7]) && re2::RE2::FullMatch(m, *reg_[8]))
            )
          )return true;
        return false;
    }

    bool is_brand_(const  std::string& b)const
    {
        return brand_.has_key(b, false);
    }

public:
    TitlePCA(const std::string& dir)
      :token_(dir)
       ,brand_(dir + "/brand.dict")
    {
        reg_.resize(9);
        reg_[0]=new re2::RE2("20[0-1][0-9]");
        reg_[1]=new re2::RE2("[0-9a-z-]{3,}");
        reg_[2]=new re2::RE2("[0-9]{3,}");
        reg_[3]=new re2::RE2("[0-9]{2,}");
        reg_[4]=new re2::RE2("[0-9a-z-]{4,}");
        reg_[5]=new re2::RE2("[0-9]");
        reg_[6]=new re2::RE2("[0-9a-z-]{5,}");
        reg_[7]=new re2::RE2("[a-z0-9]-[a-z0-9]");
        reg_[8]=new re2::RE2("[0-9a-z-]{4,}");
    }

    ~TitlePCA()
    {
        for (size_t i = 0; i < reg_.size(); ++i)
            delete(reg_[i]);
    }

    void pca(const std::string& line, 
      std::vector<std::pair<std::string, float> >& tks, 
      std::string& brand, std::string& model_type,
      std::vector<std::pair<std::string, float> >& sub_tks, bool do_sub = false)const
    {
        token_.tokenize(line, tks);
        if (do_sub)token_.subtokenize(tks, sub_tks);

        std::vector<std::string> models, brands;
        for(uint32_t i=0; i<tks.size();i++)
            if (is_brand_(tks[i].first))
                brands.push_back(tks[i].first);
            else if (model_type_(tks[i].first))
                models.push_back(tks[i].first);

        model_type = brand = "";
        for(uint32_t i=0; i<models.size();i++)if (model_type.length() < models[i].length())
            model_type = models[i];
        for(uint32_t i=0; i<brands.size();i++)if (brand.length() < brands[i].length())
            brand = brands[i];
    }
};
}
}

#endif
