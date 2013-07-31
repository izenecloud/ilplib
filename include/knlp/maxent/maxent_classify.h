#ifndef _ILPLIB_KNLP_MAXENT_CLASSIFY_H_
#define _ILPLIB_KNLP_MAXENT_CLASSIFY_H_

#include <string>
#include <list>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include "types.h"
#include "knlp/normalize.h"
#include "knlp/maxent/maxent.h"
#include "knlp/fmm.h"
#include "knlp/cate_classify.h"
#include "knlp/string_patterns.h"
#include "am/util/line_reader.h"

using namespace std;
using namespace izenelib;
using namespace izenelib::am::util;
using namespace izenelib::util;
using namespace ilplib::knlp;

namespace ilplib
{
    namespace knlp
    {
        class MaxentClassify
        {
        private:
            ME_Model model_[3];
            Fmm* tkn_;
            ilplib::knlp::GarbagePattern* gp_;
            Dictionary* query_cate_dict_;
            
        public:
            MaxentClassify(const std::string& model_nm, 
              Fmm* tkn=NULL, GarbagePattern* gp=NULL, const std::string& query_dict="")
              :tkn_(tkn),gp_(gp),query_cate_dict_(NULL)
            {
                if (model_nm.length() > 0)
                    for (uint32_t i=0;i<3;i++)
                    {
                        std::string m = model_nm;
                        m += '.';m += '0'+i;
                        model_[i].load_from_file(m);
                    }
                if (query_dict.length())
                    query_cate_dict_ = new Dictionary(query_dict);
            }

            static std::vector<std::string> split_class(std::string ca)
            {
                std::vector<std::string> r;
                size_t t = ca.find('>');
                while(t != (size_t)-1)
                {
                    r.push_back(ca.substr(0, t));
                    ca = ca.substr(t+1);
                    t = ca.find('>');
                }
                r.push_back(ca);
                return r;
            }

            static void train(const std::string& train_set, const std::string& mdnm, 
              Fmm* tkn, GarbagePattern* gp)
            {
                ME_Model model[3];
                LineReader lr(train_set);
                char* line = NULL;
                while((line = lr.line(line)) != NULL)
                {
                    char* t = strchr(line, '\t');
                    if (!t)continue;
                    std::string ti(line, t-line);
                    std::string ca(++t);
                    std::vector<std::string> cats = split_class(ca);

                    std::vector<std::pair<KString, double> > vv;
                    std::stringstream ss;
                    features(tkn, gp, ti, vv, ss, false);
                    if(vv.empty()) continue;

                    std::string R="";
                    for (uint32_t i=0;i<3 && i<cats.size();++i)
                    {
                        if (R.length())R += '>';
                        R += cats[i];
                        ME_Sample s(R);
                        for (size_t t = 0; t < vv.size(); ++t)
                            s.add_feature(vv[t].first.get_bytes("utf-8"));
                        model[i].add_training_sample(s);
                    }
                }

//                model.use_l1_regularizer(1.0);
//                model.use_l2_regularizer(1.0);
//                model.use_SGD();
//                model.set_heldout(100);
                for (uint32_t i=0;i<3;i++)
                {
                    model[i].train();
                    std::string m = mdnm;
                    m += '.'; m += '0'+i;
                    model[i].save_to_file(m);
                }
                cout<<"train ok\n";
            }

            ~MaxentClassify(){}

            KString query_features(const std::string& q)
            {
                KString kstr(gp_->clean(q));
                ilplib::knlp::Normalize::normalize(kstr);
                std::vector<std::pair<KString, double> > v;
                tkn_->fmm(kstr, v);
                std::sort(v.begin(), v.end(), MaxentClassify::cmp);
                KString r;
                for (uint32_t i=0;i<v.size();i++)
                    r += v[i].first;
                return r;
            }

            std::map<std::string,double>
              classify(const string& str, std::stringstream& ss, bool dolog=false)
            {  
                std::map<std::string,double>  r;
                if (query_cate_dict_ && str.length() < 20)
                {
                    KString k = query_features(str);
                    char* v = query_cate_dict_->value(k);
                    if (v)r[std::string(v)] = 1.0;
                    if (dolog && v)ss<<"[Query Dict]: "<<v;
                }
                std::vector<std::pair<KString, double> > vv;
                features(tkn_, gp_, str, vv, ss, dolog);
                if(vv.empty()) return r;

                ME_Sample t;
                for (size_t i = 0; i < vv.size(); ++i)
                    t.add_feature(vv[i].first.get_bytes("utf-8"));

                std::vector<std::vector<std::pair<double, std::string> > > scores;
                for (uint32_t i=0;i<3;i++)
                {
                    std::vector<double> sc = model_[i].classify(t);
                    std::vector<std::pair<double, std::string> >scp;
                    for (uint32_t j=0;j<sc.size();j++)
                        scp.push_back(make_pair(sc[j], model_[i].get_class_label(j)));
                    std::sort(scp.begin(), scp.end(), std::greater<std::pair<double, std::string> >());
                    scores.push_back(scp);
                }

                if (scores.size() == 0 || scores[0].size() == 0)
                    return r;
                std::string R = scores[0][0].second;
                if (dolog)ss<<" [Level "<<0<<"]: "<<scores[0][0].second;
                for (uint32_t i=1;i<3;i++)
                    for (uint32_t j=0;j<scores[i].size();++j)
                        if (scores[i][j].second.length() > R.length()
                          && strstr(scores[i][j].second.c_str(), R.c_str()))
                {
                    if (dolog)ss<<" [Level "<<i<<"]: "<<scores[i][j].second;
                    if (i == 2 && r.size() < 3)
                        r[scores[i][j].second] = scores[i][j].first;
                    else{
                        R = scores[i][j].second;
                        break;
                    }
                }
                if (r.size() == 0 && scores.size() > 1 && scores[1].size() > 0 && scores[0].size() > 0)
                    for (uint32_t i=0;i < scores[1].size();++i)
                        if (strstr(scores[1][i].second.c_str(), scores[0][0].second.c_str())
                          && r.size() < 3)
                            r[scores[1][i].second] = scores[1][i].first;
                if (r.size() == 0 && scores.size() && scores[0].size())
                    r[scores[0][0].second] = scores[0][0].first;
                return r;
            }
            
            static bool cmp(const std::pair<KString, double>& x, const std::pair<KString, double>& y)
            {
                return x.second > y.second;
            }

            static std::string fetch_price(const std::string& ti)
            {
                const char* s = strstr(ti.c_str(), "[[");
                if (!s)return "";
                const char* e = strstr(s, "]]");
                if (!e)return "";
                return ti.substr(s - ti.c_str() + 2, e-s-2);
            }

            static void features(Fmm* tkn, GarbagePattern* gp, const string& str, 
              std::vector<std::pair<KString, double> >& vv, std::stringstream& ss, bool dolog=false)
            {
                vv.clear();
                std::string price = fetch_price(str);
                KString kstr(gp->clean(str));
                ilplib::knlp::Normalize::normalize(kstr);
                std::vector<std::pair<KString, double> > v;
                tkn->fmm(kstr, v);
                if (v.empty()) return;

                std::sort(v.begin(), v.end(), MaxentClassify::cmp);
                vv.reserve(v.size());
                if (v.size())vv.push_back(v[0]);
                for (size_t i = 1; i < v.size(); ++i)
                    if (!(v[i].first == v[i-1].first))
                        vv.push_back(v[i]);
                if (price.length())vv.push_back(make_pair(KString(price), 0));

                KString key;
                for (uint32_t i=0;i<3 && i<v.size(); ++i)
                    key += v[i].first;
                vv.push_back(make_pair(key, 0));
                if (dolog)
                    for(uint32_t i=0;i<vv.size();++i)
                        ss<<vv[i].first<<":"<<vv[i].second<<" ";
            }
        };
    }
}
#endif

