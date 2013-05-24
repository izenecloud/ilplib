/*
 * =====================================================================================
 *
 *       Filename:  tokenize.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  01/12/2013 06:05:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), kevin.hu@b5m.com
 *        Company:  iZeneSoft.com
 *
 * =====================================================================================
 */
#ifndef _ILPLIB_NLP_TOKENIZE_H_
#define _ILPLIB_NLP_TOKENIZE_H_

#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <istream>
#include <ostream>
#include <algorithm>
#include <math.h>

#include "types.h"
#include "am/hashtable/khash_table.hpp"
#include "dictionary.h"
#include "util/string/kstring.hpp"
#include "trd2simp.h"
#include "am/util/line_reader.h"
#include "util/kthread/asynchronization.h"

namespace ilplib
{
namespace knlp
{

class Tokenize
{
    Dictionary freq_;
    izenelib::am::KStringHashTable<KString, bool> prefix_;
    mutable double minf_;
    std::vector<uint8_t> delimiter_;

    static void gen_prefix_(const std::string& dict_nm,
                            izenelib::am::KStringHashTable<KString, bool>& prefix)
    {
        uint32_t num = 0;
        {
            izenelib::am::util::LineReader lr(dict_nm);
            char* r = NULL;
            while((r = lr.line(r))!= NULL)
            {
                char* t = strchr(r, '\t');
                if (!t)throw runtime_error("dictionary format error.");
                IASSERT(t -r > 0);
                KString k(std::string(r, t -r));
                for ( uint32_t l=2; l<k.length() -1; ++l)
                    num++;
            }
        }
        prefix.reserve(num*3, num+3);
        {
            izenelib::am::util::LineReader lr(dict_nm);
            char* r = NULL;
            while((r = lr.line(r))!= NULL)
            {
                char* t = strchr(r, '\t');
                KString k(std::string(r, t -r));
                for ( uint32_t l=2; l<k.length() -1; ++l)
                    prefix.insert(k.substr(0, l), true);
            }
        }
        prefix.persistence(dict_nm+".prefix");
    }

    std::vector<KString> chunk_(const KString& line)
    {
        std::vector<KString> r;

        uint32_t la = 0;
        for ( uint32_t i=0; i<line.length(); ++i)
            if (delimiter_[line[i]])
            {
                //std::cout<<line.substring(la, i)<<std::endl;
                r.push_back(line.substr(la, i-la+1));
                la = i+1;
            }

        if (la < line.length()-1)
            r.push_back(line.substr(la));
        return r;
    }

    double  term_freq_(const KString& ustr)
    {
        char* f = freq_.value(ustr, false);
        if (!f)
        {
            if (minf_ == 0.0)
            {
                f = freq_.value(KString("[MIN]"));
                IASSERT(f);
                minf_ = atof(f);
            }
            return minf_;
        }
        return atof(f);
    }


    std::vector<std::pair<KString, double> >
    token_(const KString& line, uint32_t s,
           std::vector<std::pair<uint32_t, double> >& pos)
    {
        for ( uint32_t i=s; i<line.length(); ++i)
        {
            KString sub = line.substr(s, i-s+1);
            double f = term_freq_(sub);
            bool pre = (prefix_.find(sub)!=NULL);
            if (!pre && f == minf_)
            {
                //sub is not a token in dict
                token_(line, s+1, pos);
                break;
            }

            if (f == minf_ && pre)//sub is a prefix of some tokens in dictionary
                continue;

            //std::cout<<s<<"\t"<<i<<"\t"<<line.substring(s, i).get_bytes(charset)<<"--\t"<<f<<std::endl;
            double la = (s > 0? pos[s-1].second :0.0);
            if (pos[i].second < (double)(la+f))
            {
                pos[i].second = la+f;
                pos[i].first = s;
                for ( uint32_t t=s; t<=i; t++)if (pos[t].second < pos[i].second)
                        pos[t].second=pos[i].second, pos[t].first = s;
                //std::cout<<i<<" FROM "<<s<<std::endl;
            }
        }

        if (s != 0)
            return std::vector<std::pair<KString, double> >();

        /*std::cout<<line.get_bytes(charset)<<std::endl;
          for ( uint32_t i=0; i<pos.size(); ++i)
          std::cout<<i<<":::"<<pos[i].from_<<"\t"<<pos[i].sc_<<std::endl;*/
        std::list<std::pair<KString, double> > r;
        for ( int32_t i=pos.size()-1; i>=0;)
        {
            if (!r.empty())
                r.front().second -=  pos[i].second;
            r.push_front(std::pair<KString, double>(line.substr(pos[i].first, i-pos[i].first+1), pos[i].second));
            i = pos[i].first -1;
        }
        return merge_(std::vector<std::pair<KString, double> >(r.begin(), r.end()));
    }

    bool is_digit_(char c)const
    {
        return (::isdigit(c)||c=='.'||c=='%'||c=='$'||c==',');
    }
    std::vector<std::pair<KString, double> >
    merge_(std::vector<std::pair<KString, double> > v)
    {
        bool la_eng = (v.size()&&v[0].first.length() == 1? isalpha(v[0].first[0]):false);
        bool la_dig = (v.size()&&v[0].first.length() == 1? is_digit_(v[0].first[0]):false);

        for ( uint32_t i=1; i<v.size(); ++i)
            if (v[i].first.length() == 1)
            {
                if (isalpha(v[i].first[0]) || is_digit_(v[i].first[0]))
                {
                    if ((la_eng && isalpha(v[i].first[0]))
                            || (la_dig && is_digit_(v[i].first[0])))
                    {
                        la_eng = isalpha(v[i].first[0]);
                        la_dig = is_digit_(v[i].first[0]);

                        v[i-1].first += v[i].first;
                        v[i-1].second = std::max(v[i-1].second, v[i].second);
                        v.erase(v.begin()+i);
                        --i;
                        continue;
                    }
                    la_eng = isalpha(v[i].first[0]);
                    la_dig = is_digit_(v[i].first[0]);
                }
                else la_eng=la_dig=false;
            }
            else la_eng=la_dig=false;
        return v;
    }
public:

    Tokenize(const std::string& dict_nm)
        :freq_(dict_nm)
    {
        KString ustr("~！@#￥……&*（）—+【】{}：“”；‘’、|，。《》？ ^()-_=[]\\|;':\"<>?/");
        delimiter_.resize((int)((uint16_t)-1), 0);
        for ( uint32_t i=0; i<ustr.length(); ++i)
            delimiter_[ustr[i]] = 1;
        try
        {
            prefix_.load(dict_nm+".prefix");
        }
        catch(std::runtime_error e)
        {
            gen_prefix_(dict_nm, prefix_);
        }
    }

    ~Tokenize()
    {
    }

    void
    tokenize(const KString& line, std::vector<std::pair<KString, double> >& v)
    {
        std::vector<KString> chunks = chunk_(line);
        for ( uint32_t i=0; i<chunks.size(); ++i)
        {
            chunks[i]+=' ';
            std::vector<std::pair<uint32_t, double> > ps;/* {from:0, score:0.5} */
            ps.reserve(chunks[i].length());
            double la = 0;
            for ( uint32_t j=0; j<chunks[i].length(); ++j)
            {
                double f = term_freq_(chunks[i].substr(j,1));
                ps.push_back(std::pair<uint32_t, double>(j, la+f));
                la += f;
            }
            std::vector<std::pair<KString, double> > r = token_(chunks[i], 0, ps);
            v.insert(v.end(), r.begin(), r.begin()+((int)r.size()-1));
        }
    }

    std::vector<KString> tokenize(const KString& line)
    {
        std::vector<KString> r;
        std::vector<std::pair<KString, double> > vv;
        tokenize(line, vv);
        r.clear();
        r.reserve(vv.size());
        for ( uint32_t i=0; i<vv.size(); ++i)
            r.push_back(vv[i].first);
        return r;
    }

    uint32_t size()const
    {
        return freq_.size();
    }

    static bool train(const std::string& dictnm, const std::vector<std::string>& corpus,
                      const std::string& out)
    {
        Dictionary dict(dictnm);
        izenelib::am::KStringHashTable<KString, bool> prefix;
        gen_prefix_(dictnm, prefix);

        izenelib::am::KStringHashTable<KString, double> freq_table(dict.size()*10, dict.size()+2);
        std::vector<std::pair<KString, uint32_t> > vv;
        vv.reserve(1000000);

        struct timeval tvafter,tvpre;
        struct timezone tz;
        uint64_t C = 0;
        {
            uint32_t CC = 0;
            gettimeofday (&tvpre , &tz);
            for ( uint32_t t=0; t<corpus.size(); t++)
            {
                izenelib::am::util::LineReader lr(corpus[t]);
                char* line = NULL;
                while((line = lr.line(line))!= NULL)
                {
                    CC ++;
                    std::cout<<"\r"<<CC<<std::flush;
                    try
                    {
                        KString  u(line);
                        for ( uint32_t i=0; i<u.length(); ++i)
                            for ( uint32_t j=i; j<u.length(); ++j)
                            {
                                KString sub = u.substr(i, j-i+1);
                                if (!dict.row(sub))
                                {
                                    if (!prefix.find(sub))
                                        break;
                                    continue;
                                }
                                double* f = freq_table.find(sub);
                                if (!f)
                                    freq_table.insert(sub, 1);
                                else *f += 1;
                                C++;
                            }
                    }
                    catch(...) {}
                }
            }
            gettimeofday (&tvafter , &tz);
            std::cout<<"\nPhase A: "<<((tvafter.tv_sec-tvpre.tv_sec)*1000+(tvafter.tv_usec-tvpre.tv_usec)/1000)/60000.<<" min"<<std::endl;
        }
        {
            gettimeofday (&tvpre , &tz);
            std::ofstream of(out.c_str());
            const char* row = NULL;
            while((row = dict.next_row(row))!=NULL)
            {
                std::cout<<row<<std::endl;
                const char* t = strchr(row, '\t');
                IASSERT(t != NULL);
                KString kstr(std::string(row, t-row));
                double* f = freq_table.find(kstr);
                double s = 0;
                if (f)s = *f;
                char buf[125];
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "%9.9f", log((s+0.5)/(C+10000)));
                of << kstr << "\t" <<buf<<std::endl;
            }
            char buf[125];
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%9.9f", log(0.5/(C+10000)));
            of << "[MIN]\t" << buf << std::endl;
            gettimeofday (&tvafter , &tz);
            std::cout<<"\nPhase B: "<<((tvafter.tv_sec-tvpre.tv_sec)*1000+(tvafter.tv_usec-tvpre.tv_usec)/1000)/60000.<<" min"<<std::endl;
        }

        return true;
    }
};

}
}//namespace

#endif

