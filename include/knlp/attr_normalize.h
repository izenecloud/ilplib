#ifndef _ILPLIB_NLP_ATTR_NORMALIZE_H_
#define _ILPLIB_NLP_ATTR_NORMALIZE_H_
#include<string>
#include<map>
#include <locale>
//#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include "knlp/normalize.h"
#include <set>
#include "re2/re2.h"
#include "dictionary.h"
using namespace izenelib::util;
namespace ilplib{
    namespace knlp{
        class AttributeNormalize
        {
            public:
                Dictionary *syn_;
                std::map<std::string, std::string> unit_map;

                re2::RE2 *ureg;
                AttributeNormalize()
                {
                    setlocale(LC_ALL, "zh_CN.utf8");
                    build();
                }
                AttributeNormalize(const string& syn_path)
                {
                    syn_ = new Dictionary(syn_path);
                    setlocale(LC_ALL, "zh_CN.utf8");
                    build();
                }
            
                ~AttributeNormalize()
                {
                    for(size_t i =0; i< whole_reg.size(); ++i)
                        delete whole_reg[i].first;
                    for(size_t i =0; i< name_reg.size(); ++i)
                        delete name_reg[i].first;
                    for(size_t i =0; i< value_reg.size(); ++i)
                        delete value_reg[i].first;
                }


                std::string trans_whole(std::string& s)
                {
                    ilplib::knlp::Normalize::normalize(s);
                    return trans(s, whole_reg);
                }

                std::string trans_name(std::string& s)
                {
                    return trans(s, name_reg);
                }

                std::string trans_value(std::string& s)
                {
                    std::string res(trans(s, value_reg));

                    std::string s0, s1, s2;
                    re2::StringPiece p(res);
                    std::string tmp = res;
                    
                    while (re2::RE2::FindAndConsume(&p, *ureg, &s0, &s1, &s2) && unit_map.find(s1) != unit_map.end())
                    {
                        re2::RE2::Replace(&res, s0+s1+s2, s0+unit_map[s1]+s2);
                    }
                    
                    return res;
//                    re2::RE2::FullMatch(res, "([0-9\\.]+)([^$/]+)($|/)", &s1, &s2, &s3);
                    
//                    return trans(s, value_reg);
                }
                

                std::string trans(const std::string& s, const std::vector<std::pair<re2::RE2*, std::string> >& reg)
                {   
                    std::string res(s);
                    for (size_t i = 0; i < reg.size(); ++i)
                        re2::RE2::GlobalReplace(&res, *(reg[i].first), reg[i].second);

                    return res;
                }

                void trans_syn(string& s, const bool is_name = 0, const std::string& cate = "")
                {
                    if(is_name)
                    {
                        KString kstr(s + "@" + cate);
                        char *v = syn_->value(kstr);
                        if (!v) return ;
                        string tmp(v);
                        size_t p = tmp.find("@");
                        if (p == string::npos) return;
                        s = tmp.substr(0, p);
                    }
                    else
                    {
                        KString kstr(s);
                        char *v = syn_->value(kstr);
                        if (!v) return;
                        s = string(v);
                    }
/*                    
                    KString kstr(s);                    
                    KString ans;
                    std::vector<KString> values(kstr.split('/'));
                    for (size_t i = 0; i < values.size(); ++i)
                    {
                        char* v = syn_->value(values[i]);
                        if (!v)
                            ans += values[i];
                        else ans += KString(v);
                        if (i<values.size()-1)ans+='/';
                    }
                    s = ans.get_bytes("utf-8");
*/                    
                }

                std::string attr_normalize(std::string& s, const std::string& cate = "", const bool add_at = 0)
                {
                    string res = trans_whole(s);
                    std::vector<std::string> atts;
                    boost::split(atts, res, boost::is_any_of("\t"));
                    res = "";
                    std::set<std::string> name_set;
                    for (size_t i = 0; i < atts.size(); ++i)
                    {
                        std::string pairs0,pairs1;
                        size_t p = atts[i].find(":");
                        if (p==string::npos) continue;
                        pairs0 = atts[i].substr(0, p);
                        pairs1 = atts[i].substr(p + 1, atts[i].length() - p - 1);
                        pairs0 = trans_name(pairs0);
                        pairs1 = trans_value(pairs1);

                        if(pairs0.empty() || pairs1.empty() || pairs0.length() > 30 || pairs1.find(":") != string::npos)
                            continue;
                        {                            
                            if(syn_!=NULL)
                            {
                                trans_syn(pairs0, 1, cate);
                                trans_syn(pairs1);
                            }
                            
                            std::vector<std::string> values;
                            boost::split(values, pairs1, boost::is_any_of("/"));
                            for (size_t j = 0; j < values.size(); ++j)
                                if(add_at)
                                    res = res + pairs0 + "@" + cate + ":" + values[j] + ",";
                                else
                                    res = res + pairs0 + ":" + values[j] + ",";
//                            name_set.insert(pairs0);
                        }

                    }
                    if(!res.empty())
                    {
/*
                        if(add_att)
                        {
                            res += "[atts]:";
                            for(std::set<std::string>::iterator it = name_set.begin(); it != name_set.end(); ++it)
                                res += (*it) + "/";
                        }
*/                        
                        if ((res[res.length()-1]==','||res[res.length()-1]=='/'))
                            res.erase(res.length()-1, 1);
                    }
                    return res;
                }

            private:
                std::vector<std::pair<re2::RE2*, std::string> > whole_reg;
                std::vector<std::pair<re2::RE2*, std::string> > name_reg;
                std::vector<std::pair<re2::RE2*, std::string> > value_reg;
                void build()
                {
                    ureg = new re2::RE2("([0-9\\.]+)([^$/0-9]+)($|/)");
                    std::vector<std::pair<std::string, std::string> > reg;
                    reg.push_back(std::make_pair("[\"☆◇^┍�②“”￥_=★?!！‘㊣’～○？○□◎→■※◆・【】●◇]+", " "));
                    reg.push_back(std::make_pair("~", "-"));
                    reg.push_back(std::make_pair("。", "."));
                    reg.push_back(std::make_pair("　", " "));
                    reg.push_back(std::make_pair("•", "·"));
                    reg.push_back(std::make_pair("：", ":"));
                    reg.push_back(std::make_pair("[、\\\\]", "/"));
                    reg.push_back(std::make_pair(":是:", ":是;"));
                    reg.push_back(std::make_pair("×", "x"));
                    reg.push_back(std::make_pair("(^[^:]+)[,]+","\\1"));
                    reg.push_back(std::make_pair("([0-9])\\*([0-9])", "\\1x\\2"));
                    reg.push_back(std::make_pair("[是/、;,·|+.]+:", ":"));
                    reg.push_back(std::make_pair(",([^,:]+:)", ",\t\\1"));
                    reg.push_back(std::make_pair("([^a-z0-9]+)[ ]+", "\\1"));
                    reg.push_back(std::make_pair("[ ]+([^a-z0-9]+)", "\\1"));
                    reg.push_back(std::make_pair("\\([^\\(\\)]*\\)", ""));
                    reg.push_back(std::make_pair("\\[[^\\[\\]]*\\]", ""));
                    reg.push_back(std::make_pair("(<[^<>]*>)", ""));
                    reg.push_back(std::make_pair("(:[\t])|(\t:)", "\t"));
                    reg.push_back(std::make_pair("(^:)|(:$)", ""));

                    for (size_t i = 0; i < reg.size(); ++i)
                    {
                        whole_reg.push_back(std::make_pair(new re2::RE2(reg[i].first), reg[i].second));                
                    }
                    reg.clear();
                    

                    reg.push_back(std::make_pair("^[0-9a-z]{1}([^0-9a-z])", "\\1"));
                    reg.push_back(std::make_pair("([^0-9a-z])[0-9a-z]{1}$", "\\1"));
                    reg.push_back(std::make_pair("[.\\- /\\\\·,\"•:;]+", ""));
                    reg.push_back(std::make_pair("^[\\*]+", ""));
                    reg.push_back(std::make_pair("^[0-9]+$", ""));
                    for (size_t i = 0; i < reg.size(); ++i)
                    {
                        name_reg.push_back(std::make_pair(new re2::RE2(reg[i].first), reg[i].second));                
                    }
                    reg.clear();

                    reg.push_back(std::make_pair("[.]+", "."));
                    reg.push_back(std::make_pair("约([0-9]+)", "\\1"));
//                    reg.push_back(std::make_pair("([a-z]+)[ ]+([0-9]+)", "\\1\\2"));
//                    reg.push_back(std::make_pair("([0-9]+)[ ]+([a-z]+)", "\\1\\2"));
                    reg.push_back(std::make_pair("([0-9]+)\\.[0]{1,2}([^0-9])", "\\1\\2"));
                    reg.push_back(std::make_pair("([0-9]+)\\.[0]{1,2}$", "\\1"));
                    reg.push_back(std::make_pair("([0-9]+),([0-9]{3})", "\\1\\2"));
                    reg.push_back(std::make_pair("[/,#@|]+", "/"));
//                    reg.push_back(std::make_pair("([^0-9a-z])[ ]+([^0-9a-z])", "\\1/\\2"));
//                    reg.push_back(std::make_pair("([^a-z0-9]+)[ ]+", "\\1"));
//                    reg.push_back(std::make_pair("[ ]+([^a-z0-9]+)", "\\1"));
//                    reg.push_back(std::make_pair("^", "/"));
//                    reg.push_back(std::make_pair("$", "/"));
                    reg.push_back(std::make_pair("[^/]*(其他|其它|other)[^/]*(/|$)", "/"));
                    reg.push_back(std::make_pair("[等\\-/\\.]+$", ""));
                    reg.push_back(std::make_pair("^[/:]+", ""));
                    reg.push_back(std::make_pair("[/]+", "/"));

/*
                    reg.push_back(std::make_pair("([0-9\\.]+)英寸($|/)", "\\1inches\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)毫安时($|/)", "\\1mah\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)秒($|/)", "\\1s\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)分钟($|/)", "\\1min\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)小时($|/)", "\\1h\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)克($|/)", "\\1g\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)毫克($|/)", "\\1mg\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)千克($|/)", "\\1kg\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)公斤($|/)", "\\1kg\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)吨($|/)", "\\1ton\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)磅($|/)", "\\1lb\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)盎司($|/)", "\\1oz\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)米($|/)", "\\1m\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)公里($|/)", "\\1km\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)千米($|/)", "\\1km\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)厘米($|/)", "\\1cm\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)毫米($|/)", "\\1mm\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)纳米($|/)", "\\1nm\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)微米($|/)", "\\1μm\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)um($|/)", "\\1μm\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)英尺($|/)", "\\1ft\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)foot($|/)", "\\1ft\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)feet($|/)", "\\1ft\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)inch($|/)", "\\1inches\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)升($|/)", "\\1l\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)毫升($|/)", "\\1ml\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)加仑($|/)", "\\1gal\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)克拉($|/)", "\\1gar\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)千瓦($|/)", "\\1kw\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)赫兹($|/)", "\\1hz\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)牛顿($|/)", "\\1n\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)帕斯卡($|/)", "\\1pa\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)焦($|/)", "\\1j\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)焦耳($|/)", "\\1j\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)瓦($|/)", "\\1w\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)瓦特($|/)", "\\1w\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)库仑($|/)", "\\1c\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)伏($|/)", "\\1v\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)伏特($|/)", "\\1v\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)安培($|/)", "\\1a\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)开尔文($|/)", "\\1k\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)摩尔($|/)", "\\1mol\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)坎德拉($|/)", "\\1cd\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)弧度($|/)", "\\1rad\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)球面度($|/)", "\\1sr\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)法拉($|/)", "\\1f\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)欧($|/)", "\\1Ω\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)欧姆($|/)", "\\1Ω\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)亨利($|/)", "\\1h\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)韦伯($|/)", "\\1wb\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)℃($|/)", "\\1摄氏度\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)°c($|/)", "\\1摄氏度\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)流明($|/)", "\\1lm\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)勒克斯($|/)", "\\1lx\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)贝可($|/)", "\\1bq\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)戈瑞($|/)", "\\1gy\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)希沃特($|/)", "\\1sv\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)°($|/)", "\\1度\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)分贝($|/)", "\\1db\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)特克斯($|/)", "\\1tex\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)电子伏($|/)", "\\1ev\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)像素($|/)", "\\1pixel\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)象素($|/)", "\\1pixel\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)万像素($|/)", "\\1wpixel\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)万象素($|/)", "\\1wpixel\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)毫安($|/)", "\\1ma\\2"));
                    reg.push_back(std::make_pair("([0-9\\.]+)万($|/)", "\\1w\\2"));
*/                    
                    for (size_t i = 0; i < reg.size(); ++i)
                    {
                        value_reg.push_back(std::make_pair(new re2::RE2(reg[i].first), reg[i].second));                
                    }
                    reg.clear();


                    unit_map.insert(std::make_pair("英寸", "inches"));
                    unit_map.insert(std::make_pair("毫安时", "mah"));
                    unit_map.insert(std::make_pair("秒", "s"));
                    unit_map.insert(std::make_pair("分钟", "min"));
                    unit_map.insert(std::make_pair("小时", "h"));
                    unit_map.insert(std::make_pair("克", "g"));
                    unit_map.insert(std::make_pair("毫克", "mg"));
                    unit_map.insert(std::make_pair("千克", "kg"));
                    unit_map.insert(std::make_pair("公斤", "kg"));
                    unit_map.insert(std::make_pair("吨", "ton"));
                    unit_map.insert(std::make_pair("磅", "lb"));
                    unit_map.insert(std::make_pair("盎司", "oz"));
                    unit_map.insert(std::make_pair("米", "m"));
                    unit_map.insert(std::make_pair("公里", "km"));
                    unit_map.insert(std::make_pair("千米", "km"));
                    unit_map.insert(std::make_pair("厘米", "cm"));
                    unit_map.insert(std::make_pair("毫米", "mm"));
                    unit_map.insert(std::make_pair("纳米", "nm"));
                    unit_map.insert(std::make_pair("微米", "μm"));
                    unit_map.insert(std::make_pair("um", "μm"));
                    unit_map.insert(std::make_pair("英尺", "ft"));
                    unit_map.insert(std::make_pair("foot", "ft"));
                    unit_map.insert(std::make_pair("feet", "ft"));
                    unit_map.insert(std::make_pair("inch", "inches"));
                    unit_map.insert(std::make_pair("升", "l"));
                    unit_map.insert(std::make_pair("毫升", "ml"));
                    unit_map.insert(std::make_pair("加仑", "gal"));
                    unit_map.insert(std::make_pair("克拉", "gar"));
                    unit_map.insert(std::make_pair("千瓦", "kw"));
                    unit_map.insert(std::make_pair("赫兹", "hz"));
                    unit_map.insert(std::make_pair("牛顿", "n"));
                    unit_map.insert(std::make_pair("帕斯卡", "pa"));
                    unit_map.insert(std::make_pair("焦", "j"));
                    unit_map.insert(std::make_pair("焦耳", "j"));
                    unit_map.insert(std::make_pair("瓦", "w"));
                    unit_map.insert(std::make_pair("瓦特", "w"));
                    unit_map.insert(std::make_pair("库仑", "c"));
                    unit_map.insert(std::make_pair("伏", "v"));
                    unit_map.insert(std::make_pair("伏特", "v"));
                    unit_map.insert(std::make_pair("安培", "a"));
                    unit_map.insert(std::make_pair("开尔文", "k"));
                    unit_map.insert(std::make_pair("摩尔", "mol"));
                    unit_map.insert(std::make_pair("坎德拉", "cd"));
                    unit_map.insert(std::make_pair("弧度", "rad"));
                    unit_map.insert(std::make_pair("球面度", "sr"));
                    unit_map.insert(std::make_pair("法拉", "f"));
                    unit_map.insert(std::make_pair("欧", "Ω"));
                    unit_map.insert(std::make_pair("欧姆", "Ω"));
                    unit_map.insert(std::make_pair("亨利", "h"));
                    unit_map.insert(std::make_pair("韦伯", "wb"));
                    unit_map.insert(std::make_pair("℃", "摄氏度"));
                    unit_map.insert(std::make_pair("°c", "摄氏度"));
                    unit_map.insert(std::make_pair("流明", "lm"));
                    unit_map.insert(std::make_pair("勒克斯", "lx"));
                    unit_map.insert(std::make_pair("贝可", "bq"));
                    unit_map.insert(std::make_pair("戈瑞", "gy"));
                    unit_map.insert(std::make_pair("希沃特", "sv"));
                    unit_map.insert(std::make_pair("°", "度"));
                    unit_map.insert(std::make_pair("分贝", "db"));
                    unit_map.insert(std::make_pair("特克斯", "tex"));
                    unit_map.insert(std::make_pair("电子伏", "ev"));
                    unit_map.insert(std::make_pair("像素", "pixel"));
                    unit_map.insert(std::make_pair("象素", "pixel"));
                    unit_map.insert(std::make_pair("万像素", "wpixel"));
                    unit_map.insert(std::make_pair("万象素", "wpixel"));
                    unit_map.insert(std::make_pair("毫安", "ma"));
                    unit_map.insert(std::make_pair("万", "w"));
                }
        };
    }
}
#endif

