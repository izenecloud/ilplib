#ifndef _ILPLIB_NLP_CLUSTER_DETECTOR_H_
#define _ILPLIB_NLP_CLUSTER_DETECTOR_H_
#include "knlp/model_detector.h"
#include "knlp/product_name_detector.h"
#include "knlp/attr_normalize.h"
#include<iostream>
#include<string>
#include "knlp/normalize.h"
#include "knlp/fmm.h"
#include <boost/algorithm/string.hpp>
using namespace boost;
using namespace ilplib::knlp;
using namespace izenelib::util;
namespace ilplib{
    namespace knlp{
        class ClusterDetector{
        public:
            ModelDetector md;
            ProductNameDetector* pnd;
            Fmm tkn_;
            ilplib::knlp::GarbagePattern* gp_;
            AttributeNormalize an;


            ClusterDetector(const std::string& nm, GarbagePattern* gp=NULL)
                : tkn_(nm), gp_(gp)
            {
                pnd = new ProductNameDetector(nm, gp);
            }

            ~ClusterDetector()
            {
            }

            std::string title_normal(std::string& s)
            {
                return md.model_detect(s, 0);
            }

            std::string title_trim(std::string& s)
            {
                KString kst(s);
                kst.trim();
                std::string st(kst.get_bytes("utf-8"));
                return md.model_detect(st, 0);
            }

            std::string title_full(std::string& s)
            {
                return s;
            }

            std::string title_loose(std::string& s)
            {
                return md.model_detect(s, 1);
            }

            std::string att_normal(std::string& s)
            {
                return md.model_detect(s, 2);
            }

            std::string cluster_detect(std::string& title, std::string& cate, std::string& att, bool attr_normalize = 0)
            {
                ilplib::knlp::Normalize::normalize(title);
                ilplib::knlp::Normalize::normalize(att);
                ilplib::knlp::Normalize::normalize(cate);
                gp_->clean(title);
                gp_->clean(att);
                gp_->clean(cate);

                if (attr_normalize)
                    att = an.attr_normalize(att);


                std::string model;
                std::vector<std::string> product_name;
                std::string res;

                std::string att_value;
                std::vector<std::string> v_att;
                std::string header("http://www.b5m.com/");
                if (cate.empty())
                {
                    if (title.length() > 10)
                        return header + title_full(title);
                    else 
                        return res;
                }
                boost::split(v_att, att, is_any_of(","));              
//                boost::split(v_att, att, is_any_of("\t"));

                for(size_t i = 0; i < v_att.size(); ++i)
                {
                    int p=v_att[i].find(":");
                    std::string value = v_att[i].substr(p+1, v_att[i].length()-p-1);
                    if (v_att[i].substr(0,p).find("电话") == string::npos && v_att[i].substr(0,p).find("联系") == string::npos)
                        att_value = att_value + value + " ";
                }

                product_name = pnd->multi_product_name(title);

                model = title_normal(title);
                if (model.empty())
                    model = title_trim(title);
                if (model.empty() && !product_name.empty())
                    model = att_normal(att_value);
                if (model.empty())
                    model = title_loose(title);
                if (model.empty() && title.length() > 10)
                    model = title_full(title);

                if (model.empty())
                    return res;


                res = cate + "\t" + model;
                if (!product_name.empty())
                    res = res + "\t";
                for (size_t i = 0; i< product_name.size(); ++i)
                    if(i>0)
                        res = res + " " + product_name[i];
                    else
                        res = res + product_name[i];


                return header + res;
            }




        };
    }
}
#endif
