#ifndef _ILPLIB_NLP_ATTR_EXTRACTION_H_
#define _ILPLIB_NLP_ATTR_EXTRACTION_H_

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
class AttributeExtraction {
    HorseTokenize token_;
    KDictionary<const char*> value2name_dict_;


public:
    AttributeExtraction(const std::string& dict_path) 
    :token_(dict_path)
     ,value2name_dict_(dict_path+"/v2n.dict")
    {
    }

    ~AttributeExtraction() {
    }

    std::map<std::string, std::string>
      extract(const std::string& cate, 
            const std::string& title, 
        std::map<std::string, std::string> attrm)
      {
          if (cate.length() == 0 || title.length() < 2)
              return attrm;

          std::vector<std::pair<std::string, float> > tks;
          token_.tokenize(title, tks);

          for (uint32_t i=0;i<tks.size(); i++)
          {
              const char* nm = NULL;
              if(value2name_dict_.value(cate+"@"+tks[i].first, nm)<0)
                  continue;
              assert(nm!=NULL);
              if (attrm.find(nm) == attrm.end())
                  attrm[nm] = tks[i].first;
              else if (strstr(attrm[nm].c_str(), tks[i].first.c_str())==NULL)
                  attrm[nm] = attrm[nm] +"/" + tks[i].first;
          }

          return attrm;
    }
    
};


}
}

#endif
