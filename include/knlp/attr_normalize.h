#ifndef _ILPLIB_NLP_CATE_CLASSIFY_H_
#define _ILPLIB_NLP_CATE_CLASSIFY_H_
#include<string>
using std::string;
using std::wstring;
#include <locale>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include "knlp/normalize.h"
#include <set>
using namespace boost;
namespace ilplib{
    namespace knlp{
        class AttributeNormalize
        {
            public:
                AttributeNormalize()
                {
                    build();
                }
            
                ~AttributeNormalize()
                {}


                std::string trans_whole(std::string& s)
                {
                    ilplib::knlp::Normalize::normalize(s);
                    return trans(s, whole_reg);
                }

                std::string trans_name(std::string& s)
                {
                    ilplib::knlp::Normalize::normalize(s);
                    return trans(s, name_reg);
                }

                std::string trans_value(std::string& s)
                {
                    ilplib::knlp::Normalize::normalize(s);
                    return trans(s, value_reg);
                }
                

                std::string trans(const std::string& s, const std::vector<std::pair<boost::wregex, std::wstring> >& reg)
                {
                    setlocale(LC_ALL, "");
                    int wlen = mbstowcs(NULL, s.c_str(), 0);
                    wchar_t *wst = new wchar_t[wlen+1];
                    mbstowcs(wst, s.c_str(), wlen+1);
                    std::wstring wres(wst);

                    for(size_t i = 0 ; i < wres.size(); ++i)
                        if ((int)wres[i] > 65248 && (int)wres[i] < 65373)    
                            wres[i] = wres[i]-65248;

                    for (size_t i = 0; i < reg.size(); ++i)
                    {
                        wres = regex_replace(wres, reg[i].first, reg[i].second);
                    }
                    
                    int len= wcstombs(NULL, wres.c_str(), 0);
                    char *st= new char[len+1];
                    wcstombs(st, wres.c_str(), len+1);
                    st[len] = '\0';
                    std::string res(st);
                    delete []wst;
                    delete []st;
                    return res;
                }

                std::string attr_normalize(std::string& s, const std::string& cate = "")
                {
                
                    string res = trans_whole(s);
                    std::vector<std::string> atts;
                    boost::split(atts, res, boost::is_any_of("\t"));
                    res = "";
                    std::set<std::string> name_set;
                    for (int i = 0; i < atts.size(); ++i)
                    {
                        std::string pairs0,pairs1;
//        boost::split(pairs, atts[i], boost::is_any_of(":"));
                        int p = atts[i].find(":");
                        pairs0 = atts[i].substr(0, p);
                        pairs1 = atts[i].substr(p + 1, atts[i].length() - p - 1);
//        if (pairs.size() != 2) continue;

                        pairs0 = trans_name(pairs0);
                        pairs1 = trans_value(pairs1);
//        cout<<pairs0<<":"<<pairs1<<endl;
                        if(pairs0!=""&&pairs1!="")
                        {
                            res = res + pairs0 + "@" + cate + ":" + pairs1 + ",";
                            name_set.insert(pairs0);
                        }

                    }
                    if(!res.empty())
                    {
                        res += "[atts]:";
                        for(std::set<std::string>::iterator it = name_set.begin(); it != name_set.end(); ++it)
                            res += (*it) + "/";
                        if ((res[res.length()-1]==','||res[res.length()-1]=='/'))
                            res.erase(res.length()-1, 1);
                    }
                    return res;
                }

            public:
                std::vector<std::pair<boost::wregex, std::wstring> > whole_reg;
                std::vector<std::pair<boost::wregex, std::wstring> > name_reg;
                std::vector<std::pair<boost::wregex, std::wstring> > value_reg;
                void build()
                {
                    std::vector<std::pair<std::wstring, std::wstring> > reg;
                    reg.push_back(std::make_pair(L"[\"☆◇~^┍�②“”￥_=★?!！‘㊣’～○？○□◎→■※◆・【】●◇]+", L" "));
                    reg.push_back(std::make_pair(L"。", L"."));
                    reg.push_back(std::make_pair(L"　", L" "));
                    reg.push_back(std::make_pair(L"•", L"·"));
                    reg.push_back(std::make_pair(L"、", L"\\"));
                    reg.push_back(std::make_pair(L":是:", L":是;"));
                    reg.push_back(std::make_pair(L"×", L"x"));
                    reg.push_back(std::make_pair(L"(^[^:]+)[,]+",L"$1"));
                    reg.push_back(std::make_pair(L"([0-9])\\*([0-9])", L"$1x$2"));
                    reg.push_back(std::make_pair(L"[是\\/、;,·|+.\\\\]+:", L":"));
                    reg.push_back(std::make_pair(L",([^,:]+:)", L",\t$1"));
                    reg.push_back(std::make_pair(L"([^a-z0-9]+)[ ]+", L"$1"));
                    reg.push_back(std::make_pair(L"[ ]+([^a-z0-9]+)", L"$1"));
                    reg.push_back(std::make_pair(L"\\\\",L"/"));
                    reg.push_back(std::make_pair(L"\\([^\\(\\)]*\\)", L""));
                    reg.push_back(std::make_pair(L"\\[[^\\[\\]]*\\]", L""));
                    for (size_t i = 0; i < reg.size(); ++i)
                    {
                        boost::wregex wreg(reg[i].first, boost::regex::icase|boost::regex::perl);
                        whole_reg.push_back(std::make_pair(wreg, reg[i].second));                
                    }
                    reg.clear();
                    

                    reg.push_back(std::make_pair(L"^[0-9a-z]{1}([^0-9a-z])", L"$1"));
                    reg.push_back(std::make_pair(L"([^0-9a-z])[0-9a-z]{1}$", L"$1"));
                    reg.push_back(std::make_pair(L"[.\\- /\\\\·,\"•:;]+", L""));
                    reg.push_back(std::make_pair(L"^[\\*]+", L""));
                    reg.push_back(std::make_pair(L"^[0-9]+$", L""));
                    for (size_t i = 0; i < reg.size(); ++i)
                    {
                        boost::wregex wreg(reg[i].first, boost::regex::icase|boost::regex::perl);
                        name_reg.push_back(std::make_pair(wreg, reg[i].second));                
                    }
                    reg.clear();

                    reg.push_back(std::make_pair(L"[.]+", L"."));
                    reg.push_back(std::make_pair(L"约([0-9]+)", L"$1"));
//                    reg.push_back(std::make_pair(L"([a-z]+)[ ]+([0-9]+)", L"$1$2"));
//                    reg.push_back(std::make_pair(L"([0-9]+)[ ]+([a-z]+)", L"$1$2"));
                    reg.push_back(std::make_pair(L"([0-9]+)\\.[0]{1,2}([^0-9])", L"$1$2"));
                    reg.push_back(std::make_pair(L"([0-9]+)\\.[0]{1,2}$", L"$1"));
                    reg.push_back(std::make_pair(L"([0-9]+),([0-9]{3})", L"$1$2"));
                    reg.push_back(std::make_pair(L"[/,#@|]+", L"\\/"));
//                    reg.push_back(std::make_pair(L"([^0-9a-z])[ ]+([^0-9a-z])", L"$1/$2"));
                    reg.push_back(std::make_pair(L"([^a-z0-9]+)[ ]+", L"$1"));
                    reg.push_back(std::make_pair(L"[ ]+([^a-z0-9]+)", L"$1"));
                    reg.push_back(std::make_pair(L"^", L"/"));
                    reg.push_back(std::make_pair(L"$", L"/"));
                    reg.push_back(std::make_pair(L"/[^/]*(其他|其它|other)[^/]*/", L"/"));
                    reg.push_back(std::make_pair(L"[等\\-/\\.]+$", L""));
                    reg.push_back(std::make_pair(L"^[/:]+", L""));

                    reg.push_back(std::make_pair(L"([0-9\\.]+)英寸($|/)", L"$1inches$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)毫安时($|/)", L"$1mah$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)秒($|/)", L"$1s$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)分钟($|/)", L"$1min$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)小时($|/)", L"$1h$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)克($|/)", L"$1g$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)毫克($|/)", L"$1mg$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)千克($|/)", L"$1kg$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)公斤($|/)", L"$1kg$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)吨($|/)", L"$1ton$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)磅($|/)", L"$1lb$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)盎司($|/)", L"$1oz$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)米($|/)", L"$1m$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)公里($|/)", L"$1km$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)千米($|/)", L"$1km$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)厘米($|/)", L"$1cm$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)毫米($|/)", L"$1mm$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)纳米($|/)", L"$1nm$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)微米($|/)", L"$1μm$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)um($|/)", L"$1μm$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)英尺($|/)", L"$1ft$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)foot($|/)", L"$1ft$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)feet($|/)", L"$1ft$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)inch($|/)", L"$1inches$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)升($|/)", L"$1l$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)毫升($|/)", L"$1ml$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)加仑($|/)", L"$1gal$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)克拉($|/)", L"$1gar$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)千瓦($|/)", L"$1kw$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)赫兹($|/)", L"$1hz$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)牛顿($|/)", L"$1n$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)帕斯卡($|/)", L"$1pa$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)焦($|/)", L"$1j$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)焦耳($|/)", L"$1j$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)瓦($|/)", L"$1w$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)瓦特($|/)", L"$1w$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)库仑($|/)", L"$1c$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)伏($|/)", L"$1v$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)伏特($|/)", L"$1v$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)安培($|/)", L"$1a$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)开尔文($|/)", L"$1k$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)摩尔($|/)", L"$1mol$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)坎德拉($|/)", L"$1cd$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)弧度($|/)", L"$1rad$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)球面度($|/)", L"$1sr$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)法拉($|/)", L"$1f$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)欧($|/)", L"$1Ω$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)欧姆($|/)", L"$1Ω$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)亨利($|/)", L"$1h$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)韦伯($|/)", L"$1wb$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)℃($|/)", L"$1摄氏度$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)°c($|/)", L"$1摄氏度$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)流明($|/)", L"$1lm$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)勒克斯($|/)", L"$1lx$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)贝可($|/)", L"$1bq$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)戈瑞($|/)", L"$1gy$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)希沃特($|/)", L"$1sv$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)°($|/)", L"$1度$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)分贝($|/)", L"$1db$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)特克斯($|/)", L"$1tex$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)电子伏($|/)", L"$1ev$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)像素($|/)", L"$1pixel$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)象素($|/)", L"$1pixel$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)万像素($|/)", L"$1wpixel$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)万象素($|/)", L"$1wpixel$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)毫安($|/)", L"$1ma$2"));
                    reg.push_back(std::make_pair(L"([0-9\\.]+)万($|/)", L"$1w$2"));
                    for (size_t i = 0; i < reg.size(); ++i)
                    {
                        boost::wregex wreg(reg[i].first, boost::regex::icase|boost::regex::perl);
                        value_reg.push_back(std::make_pair(wreg, reg[i].second));                
                    }
                    reg.clear();


                }
        };
    }
}
#endif

