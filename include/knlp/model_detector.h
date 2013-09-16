#ifndef _ILPLIB_NLP_MODEL_DETECTOR_H_
#define _ILPLIB_NLP_MODEL_DETECTOR_H_
#include<iostream>
#include<string>
#include<boost/regex.hpp>
#include<algorithm>
#include"re2/re2.h"
namespace ilplib{
    namespace knlp{
        class ModelDetector{
        public:
            ModelDetector()
            {
                unit_init();
                build();
            }
            ~ModelDetector()
            {}

            std::set<std::string> chs_unit;
            std::set<std::string> chs_num;
            std::set<std::string> eng_unit;
            std::vector<std::pair<re2::RE2*, std::string> > regs0, regs1, regs2;
            std::vector<re2::RE2*> unit_reg;
            void unit_init()
            {
                chs_unit.insert("克");
                chs_unit.insert("像素");
                chs_unit.insert("公斤");
                chs_unit.insert("英寸");
                chs_unit.insert("分钟");
                chs_unit.insert("小时");
                chs_unit.insert("秒");
                chs_unit.insert("年");
                chs_unit.insert("月");
                chs_unit.insert("日");
                chs_unit.insert("斤");
                chs_unit.insert("寸");
                chs_unit.insert("英尺");
                chs_unit.insert("公里");
                chs_unit.insert("像素");
                chs_unit.insert("倍");
                chs_unit.insert("周");
                chs_unit.insert("吨");
                chs_unit.insert("磅");
                chs_unit.insert("盎司");
                chs_unit.insert("米");
                chs_unit.insert("厘米");
                chs_unit.insert("毫米");
                chs_unit.insert("分米");
                chs_unit.insert("微米");
                chs_unit.insert("纳米");
                chs_unit.insert("升");
                chs_unit.insert("毫升");
                chs_unit.insert("加仑");
                chs_unit.insert("度");
                chs_unit.insert("瓦");
                chs_unit.insert("焦");
                chs_unit.insert("伏");
                chs_unit.insert("安培");
                chs_unit.insert("毫安");
                chs_unit.insert("分贝");
                chs_unit.insert("象素");
                chs_unit.insert("毫秒");
                chs_unit.insert("微秒");
                chs_unit.insert("纳秒");
                chs_unit.insert("摄氏度");
                chs_unit.insert("克拉");
                chs_unit.insert("赫兹");
                chs_unit.insert("欧姆");
                chs_unit.insert("牛顿");
                chs_unit.insert("立方");
                chs_unit.insert("摩尔");
                chs_unit.insert("岁");
                chs_unit.insert("年级");
                chs_unit.insert("周岁");
                chs_unit.insert("个");
                chs_unit.insert("包邮");
                chs_unit.insert("送");
                chs_unit.insert("码");
                chs_unit.insert("尺");
                chs_unit.insert("号");
                chs_unit.insert("色");
                chs_unit.insert("支");
                chs_unit.insert("套");
                chs_unit.insert("条");
                chs_unit.insert("饼");
                chs_unit.insert("块");
                chs_unit.insert("元");
                chs_unit.insert("公升");


                chs_num.insert("万");
                chs_num.insert("百");
                chs_num.insert("千");
                chs_num.insert("兆");
                chs_num.insert("亿");


                eng_unit.insert("kg");
                eng_unit.insert("g");
                eng_unit.insert("mah");
                eng_unit.insert("mg");
                eng_unit.insert("l");
                eng_unit.insert("ml");
                eng_unit.insert("oz");
                eng_unit.insert("cm");
                eng_unit.insert("m");
                eng_unit.insert("mm");
                eng_unit.insert("km");
                eng_unit.insert("nm");
                eng_unit.insert("inch");
                eng_unit.insert("inches");
                eng_unit.insert("ton");
                eng_unit.insert("lb");
                eng_unit.insert("ft");
                eng_unit.insert("gal");
                eng_unit.insert("min");
                eng_unit.insert("sec");
                eng_unit.insert("hour");
                eng_unit.insert("kw");
                eng_unit.insert("gar");
                eng_unit.insert("hz");
                eng_unit.insert("khz");
                eng_unit.insert("gb");
                eng_unit.insert("mb");
                eng_unit.insert("mol");
                eng_unit.insert("rad");
                eng_unit.insert("ma");
                eng_unit.insert("pix");
                eng_unit.insert("pixel");
                eng_unit.insert("f");
                eng_unit.insert("a");
                eng_unit.insert("j");
                eng_unit.insert("w");
                eng_unit.insert("db");
                eng_unit.insert("kb");
                eng_unit.insert("tex");
                eng_unit.insert("mw");
                eng_unit.insert("hours");
                eng_unit.insert("years");
                eng_unit.insert("year");
                eng_unit.insert("minute");
                eng_unit.insert("mins");
                eng_unit.insert("v");
                eng_unit.insert("cc");
                eng_unit.insert("pc");
                eng_unit.insert("pcs");
                eng_unit.insert("mhz");
                eng_unit.insert("ghz");
                eng_unit.insert("tb");

            }

            void clean(std::string& s)
            {
                //cout<<"before clean "<<res<<endl;
/*                
                res = regex_replace(res, boost::regex("android"), "");
                res = regex_replace(res, boost::regex("1573"), "");
                res = regex_replace(res, boost::regex("cet"), "");
                res = regex_replace(res, boost::regex("spf[0-9\\-]+"), "");
                res = regex_replace(res, boost::regex("[0-9]*color[s]*"), "");
                res = regex_replace(res, boost::regex("led"), "");
                res = regex_replace(res, boost::regex("[-]+"), "-");
                res = regex_replace(res, boost::regex("[\\.]+"), ".");
                res = regex_replace(res, boost::regex("^-|-$"), "");
                res = regex_replace(res, boost::regex("^\\.|\\.$"), "");
                res = regex_replace(res, boost::regex("^([0-9\\.]+)x([0-9]+\\.)x([0-9]+\\.)$"), "");
                res = regex_replace(res, boost::regex("^([0-9]+\\.)x([0-9]+\\.)$"), "");
                res = regex_replace(res, boost::regex("^([0-9]{1,4})\\.([0-9]{1,2})$"), "");
                res = regex_replace(res, boost::regex("^([0-9]{1,4})\\.([0-9]{1,2})\\.([0-9]{1,2})$"), "");
                res = regex_replace(res, boost::regex("^[a-z]{2,}(20|19)[0-9]{2,2}$"), "");
*/                

                for (size_t i = 0; i < regs1.size(); ++i)
                    re2::RE2::GlobalReplace(&s, *(regs1[i].first), regs1[i].second);
                if (s.length() < 4)
                {
                    s.clear();
                    return;
                }

                std::string s0, s1, s2, s3, s4, s5, s6, tmp(s);


                if (re2::RE2::FullMatch(s, *(unit_reg[0]), &s0, &s1) && eng_unit.find(s1) != eng_unit.end())
                    s.clear();
                else if (re2::RE2::FullMatch(s, *(unit_reg[1]), &s0, &s1, &s2) && eng_unit.find(s2) != eng_unit.end())
                    s.clear();
                else if (re2::RE2::FullMatch(s, *(unit_reg[2]), &s0, &s1, &s2) && eng_unit.find(s1) != eng_unit.end())
                    s.clear();
                else if (re2::RE2::FullMatch(s, *(unit_reg[3]), &s0, &s1, &s2, &s3) && eng_unit.find(s1) != eng_unit.end() && eng_unit.find(s3) != eng_unit.end())
                    s.clear();
                else if (re2::RE2::FullMatch(s, *(unit_reg[4]), &s0, &s1, &s2, &s3, &s4, &s5) && eng_unit.find(s1) != eng_unit.end() && eng_unit.find(s3) != eng_unit.end() &&eng_unit.find(s5) != eng_unit.end())
                    s.clear();

/*
                {
                    boost::smatch what;
                    boost::regex_search(res, what, boost::regex("^([0-9\\.]*[0-9])([a-z]{1,6})$"));
                    if (what.size()==3 && eng_unit.find(what[2])!=eng_unit.end())
                        res = regex_replace(res, boost::regex("^([0-9\\.]*[0-9])([a-z]{1,6})$"), "");
                }

                {
                    boost::smatch what;
                    boost::regex_search(res, what, boost::regex("^([0-9\\.]+)[x\\-]([0-9\\.]+)([a-z]{1,6})$"));
                    if (what.size()==4 && eng_unit.find(what[3])!=eng_unit.end())
                        res = regex_replace(res, boost::regex("^([0-9\\.]+)[x\\-]([0-9\\.]+)([a-z]{1,6})$"), "");
                }

                {
                    boost::smatch what;
                    boost::regex_search(res, what, boost::regex("^([0-9\\.]+)([a-wyz]{1,6})[x\\-]([0-9\\.]+)$"));
                    if (what.size()==4 && eng_unit.find(what[2])!=eng_unit.end())
                        res = regex_replace(res, boost::regex("^([0-9\\.]+)([a-wyz]{1,6})[x\\-]([0-9\\.]+)$"), "");
                }

                {
                    boost::smatch what;
                    boost::regex_search(res, what, boost::regex("^([0-9\\.]+)([a-wyz]{1,6})[x\\-]([0-9\\.]+)([a-z]{1,6})$"));
                    if (what.size()==5 && eng_unit.find(what[2])!=eng_unit.end() && eng_unit.find(what[4])!=eng_unit.end())
                        res = regex_replace(res, boost::regex("^([0-9\\.]+)([a-wyz]{1,6})[x\\-]([0-9\\.]+)([a-z]{1,6})$"), "");
                }

                {
                    boost::smatch what;
                    boost::regex_search(res, what, boost::regex("^([0-9\\.]+)([a-wyz]{1,6})[x\\-]([0-9\\.]+)([a-wyz]{1,6})[x\\-]([0-9\\.]+)([a-z]{1,6})$"));
                    if (what.size()==7 && eng_unit.find(what[2])!=eng_unit.end() && eng_unit.find(what[4])!=eng_unit.end() && eng_unit.find(what[6])!=eng_unit.end())
                        res = regex_replace(res, boost::regex("^([0-9\\.]+)([a-wyz]{1,6})[x\\-]([0-9\\.]+)([a-wyz]{1,6})[x\\-]([0-9\\.]+)([a-z]{1,6})$"), "");
                }
*/

                /*    
                      for (size_t i = 0; i < what.size(); ++i)
                      {
                      if (what[i].matched)
                      std::cout << what[i] << std::endl;
                      }
                 */
                if (s.length() < 4)
                    s.clear();
                //cout<<"after clean "<<res<<endl;
                //    res = regex_replace(res, "[-]+", "-");
                //    res = regex_replace(res, "[-]+", "-");

            }

            std::string check(std::string& s, int begin, int len, int eng, int num, int punc, int mode = 0)
            {
                std::string res;
                if (begin == -1)
                    return res;
                if (1!=mode)
                {
                    if (0 == mode)
                    {
                        if(len<4 || 0==num || punc>2)
                            return res;
                        if(4==len && ((s[begin]=='2' && s[begin+1]=='0') || (s[begin]=='1' && s[begin+1]=='9')))
                            return res;
                    }
                    else
                    {
                        if(len < 6 || num < 3 || punc > 1)
                            return res;
                    }
                std::string tmp=s.substr(s.length()-4, 4);

                bool find=1;
                if (begin+len==(int)s.length()||eng>0)
                {
                }
                else 
                {
                    int p=begin+len;
                    std::string s0,s1,s2,s3;
                    if (p+2<(int)s.length() && s[p]&0x80 && (unsigned)s[p]>=0)
                    {
                        s0 = s.substr(p,3);
                        if (chs_num.find(s0)!=chs_num.end())
                            p+=3;
                    }
                    if (p+2<(int)s.length() && s[p]&0x80 && (unsigned)s[p]>=0)
                    {
                        s1= s.substr(p,3);
                        p+=3;
                        if (p+2<(int)s.length() && s[p]&0x80 && (unsigned)s[p]>=0)
                        {
                            s2=s1 + s.substr(p,3);
                            p+=3;
                            if (p+2<(int)s.length() && s[p]&0x80 && (unsigned)s[p]>=0)
                            {
                                s3=s2+ s.substr(p,3);
                                p+=3;
                            }
                        }
                    }
                    //cout<<p<<' '<<s0<<' '<<s1<<' '<<s2<<' '<<s3<<endl;
                    if (!s3.empty()&&chs_unit.find(s3)!=chs_unit.end())
                        find=0;
                    else if(!s2.empty()&&chs_unit.find(s2)!=chs_unit.end())
                        find=0;
                    else if(!s1.empty()&&chs_unit.find(s1)!=chs_unit.end())
                        find=0;
                }
                if(find)
                {
                    res = s.substr(begin, len);
                    clean(res);
                }
                }
                else
                {
                    if(len<4 || 0==num)
                        return res;
                    res = s.substr(begin, len);
                
                }
                return res;
            }

            std::string model_detect(std::string& s, int mode = 0)
            {
                int num=0,eng=0,punc=0;
                int len=0,begin=-1;
                std::string res;
//                transform(s.begin(), s.end(), s.begin(), (int (*)(int))tolower);
//                s = regex_replace(s, boost::regex("[0-9\\.]+\\*[0-9\\.]+"), "");
                for (size_t i = 0; i < regs0.size(); ++i)
                    re2::RE2::GlobalReplace(&s, *(regs0[i].first), regs0[i].second); 


                for(size_t i = 0; i < s.length(); ++i)
                {   
                    if(s[i]>='0'&&s[i]<='9')
                    {   
                        ++num;
                        ++len;
                        if(-1==begin)
                            begin = i;
                    }   
                    else if ((s[i]>='a'&&s[i]<='z')||(s[i]>='A'&&s[i]<='Z'))
                    {   
                        ++eng;
                        ++len;
                        if(-1==begin)
                            begin = i;
                    }   
                    else if (s[i]=='-'||s[i]=='.')
                    {
                        ++len;
                        ++punc;
                        if(-1==begin)
                            begin = i;
                    }   
                    else
                    {   
                        std::string tmp;
                        tmp = check(s, begin, len, eng, num, punc, mode);
                        if (res.empty() || tmp.length() > res.length())
                            res = tmp;
                        num=0;
                        eng=0;
                        punc=0;
                        len=0;
                        begin=-1;
                    }   
                }   

                std::string tmp = check(s, begin, len, eng, num, punc, mode);
                if (res.empty() || tmp.length() > res.length())
                    res = tmp;
                return res;
            }

            void build()
            {
            
                std::vector<std::pair<std::string, std::string> > reg;
                reg.push_back(std::make_pair("[0-9\\.]+\\*[0-9\\.]+\\*[0-9\\.]+", ""));
                reg.push_back(std::make_pair("[0-9\\.]+[\\*x\\-][0-9\\.]+", ""));
                for (size_t i = 0; i < reg.size(); ++i)
                {
                    regs0.push_back(std::make_pair(new re2::RE2(reg[i].first), reg[i].second));                
                }
                reg.clear();



                reg.push_back(std::make_pair("android|1573|cet|led|(spf[0-9\\-]+)|[0-9]*color[s]*", ""));
                reg.push_back(std::make_pair("^\\.|\\.$||^-|-$", ""));
                reg.push_back(std::make_pair("[-]+", "-"));
                reg.push_back(std::make_pair("[\\.]+", "."));
                reg.push_back(std::make_pair("^([0-9\\.]+)x([0-9]+\\.)x([0-9]+\\.)$", ""));
                reg.push_back(std::make_pair("^([0-9]+\\.)x([0-9]+\\.)$", ""));
                reg.push_back(std::make_pair("^([0-9]{1,4})\\.([0-9]{1,2})$", ""));
                reg.push_back(std::make_pair("^([0-9]{2,4})\\.([0-9]{1,2})\\.([0-9]{1,2})$", ""));
                reg.push_back(std::make_pair("^[a-z]{2,}(20|19)[0-9]{2,2}$",""));
                for (size_t i = 0; i < reg.size(); ++i)
                {
                    regs1.push_back(std::make_pair(new re2::RE2(reg[i].first), reg[i].second));                
                }
                reg.clear();

                unit_reg.push_back(new re2::RE2("^([0-9\\.]*[0-9])([a-z]{1,6})$"));
                unit_reg.push_back(new re2::RE2("^([0-9\\.]+)[x\\-]([0-9\\.]+)([a-z]{1,6})$"));
                unit_reg.push_back(new re2::RE2("^([0-9\\.]+)([a-wyz]{1,6})[x\\-]([0-9\\.]+)$"));
                unit_reg.push_back(new re2::RE2("^([0-9\\.]+)([a-wyz]{1,6})[x\\-]([0-9\\.]+)([a-z]{1,6})$"));
                unit_reg.push_back(new re2::RE2("^([0-9\\.]+)([a-wyz]{1,6})[x\\-]([0-9\\.]+)([a-wyz]{1,6})[x\\-]([0-9\\.]+)([a-z]{1,6})$"));
            
            }
        };
    }
}
#endif
