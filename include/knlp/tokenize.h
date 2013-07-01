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
#include <cctype>
#include <math.h>

#include "types.h"
#include "am/hashtable/khash_table.hpp"
#include "dictionary.h"
#include "util/string/kstring.hpp"
#include "trd2simp.h"
#include "am/util/line_reader.h"
#include "util/kthread/asynchronization.h"
#include "net/seda/queue.hpp"

namespace ilplib
{
namespace knlp
{

class Tokenize
{
    mutable double minf_;
    DigitalDictionary freq_;
    izenelib::am::KStringHashTable<KString, bool> prefix_;
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
                for ( int32_t l=2; l<=(int32_t)k.length() -1; ++l)
                    num++;
            }
        }
        prefix.reserve(num*3, num*3+3);
        {
            izenelib::am::util::LineReader lr(dict_nm);
            char* r = NULL;
            while((r = lr.line(r))!= NULL)
            {
                char* t = strchr(r, '\t');
                KString k(std::string(r, t -r));
                for ( int32_t l=2; l<=(int32_t)k.length() -1; ++l)
                    prefix.insert(k.substr(0, l), true);
            }
        }
        prefix.persistence(dict_nm+".prefix");
    }

    std::vector<KString> chunk_(const KString& line)
    {
        std::vector<KString> r;

        int32_t la = 0;
        for ( int32_t i=0; i<(int32_t)line.length(); ++i)
		  if (KString::is_english(line[i]) || is_digit_(line[i]))
		  {
			  if (la < i)
			  {
				  r.push_back(line.substr(la, i-la));
				  la = i;
			  }
			  while(i<(int32_t)line.length() && (KString::is_english(line[i]) || is_digit_(line[i])))
				i++;
			  if (i -la <= 1)
              {
                  --i;
                  continue;
              }
			  r.push_back(line.substr(la, i-la));
			  la = i;
			  i--;
		  }
		  else if (delimiter_[line[i]])
            {
                //std::cout<<line.substring(la, i)<<std::endl;
                r.push_back(line.substr(la, i-la+1));
                la = i+1;
            }

        if (la < (int32_t)line.length())
            r.push_back(line.substr(la));
		//for ( uint32_t i=0; i<r.size(); ++i)
		  //std::cout<<r[i]<<std::endl;
        return r;
    }

    double  term_freq_(const KString& ustr)
    {
		if (minf_ == 0.0)
				minf_= freq_.value(KString("[MIN]"));

        double f = freq_.value(ustr, false);
        if (f == (double)std::numeric_limits<int>::min())
            return minf_;
        return f;
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

            double la = (s > 0? pos[s-1].second :0.0);
            //std::cout<<s<<"\t"<<i<<"\t"<<sub<<"--\t"<<f<<"\t"<<la<<"\t"<<pos[i].second<<std::endl;
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

    bool is_digit_(int c)const
    {
        return is_digit(c);
    }

	bool is_alphanum_(const KString& str)
	{
		for ( uint32_t i=0; i<str.length(); ++i)
		  if (!(is_digit_(str[i])||KString::is_english(str[i])))
			return false;
		return true;
	}

    std::vector<std::pair<KString, double> >
    merge_(std::vector<std::pair<KString, double> > v)
    {
        bool la_eng = (v.size()&&v[0].first.length() == 1? KString::is_english(v[0].first[0]):false);
        bool la_dig = (v.size()&&v[0].first.length() == 1? is_digit_(v[0].first[0]):false);

        for ( uint32_t i=1; i<v.size(); ++i)
            if (v[i].first.length() == 1)
            {
                if (KString::is_english(v[i].first[0]) || is_digit_(v[i].first[0]))
                {
                    if ((la_eng && KString::is_english(v[i].first[0]))
                            || (la_dig && is_digit_(v[i].first[0])))
                    {
                        la_eng = KString::is_english(v[i].first[0]);
                        la_dig = is_digit_(v[i].first[0]);

                        v[i-1].first += v[i].first;
                        v[i-1].second = std::max(v[i-1].second, v[i].second);
                        v.erase(v.begin()+i);
                        --i;
                        continue;
                    }
                    la_eng = KString::is_english(v[i].first[0]);
                    la_dig = is_digit_(v[i].first[0]);
                }
                else la_eng=la_dig=false;
            }
            else la_eng=la_dig=false;
        return v;
    }
public:

    Tokenize(const std::string& dict_nm)
        :minf_(0), freq_(dict_nm)
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
            std::cout<<e.what()<<std::endl;
            gen_prefix_(dict_nm, prefix_);
        }
        term_freq_(KString("[MIN]"));
    }

    ~Tokenize()
    {
    }

    static bool is_digit(int c)
    {
        return (std::isdigit(c)||c=='.'||c=='%'||c=='$'||c==','||c=='-');
    }

    bool is_in(const KString& kstr)
    {
        double f = freq_.value(kstr, false);
        if (f == (double)std::numeric_limits<int>::min())
            return false;
        return true;
    }

    double score(const KString& kstr)
    {
        return term_freq_(kstr);
    }

	void fmm(const KString& line, std::vector<std::pair<KString,double> >& r)//forward maximize match
	{
		r.clear();
		if (line.length() == 0)return;
		std::vector<KString> chunks = chunk_(line);
        std::cout<<chunks[0]<<"=="<<chunks[0].is_chinese(0)<<endl;
		for ( uint32_t i=0; i<chunks.size(); ++i)
		{
			if (is_alphanum_(chunks[i]) || chunks[i].length() < 3
			  || (chunks[i].length() == 3 && KString::is_chinese(chunks[i][0])))
			{
  				r.push_back(make_pair(chunks[i], term_freq_(chunks[i])));
				continue;
			}
			uint32_t from = 0, to = 1;
			while(to < chunks[i].length())
			{
				KString sub = chunks[i].substr(from, to-from +1);
                //std::cout<<sub<<std::endl;
				double f = term_freq_(sub);
				bool pre = (prefix_.find(sub)!=NULL);
				if (f == minf_)
				{
					if (pre && to+1<chunks[i].length())
					{
						to++;
						continue;
					}
                    //std::cout<<chunks[i].substr(from, to-from)<<"xxxxxxxxxxx\n";
                    sub = chunks[i].substr(from, to-from);
                    f = term_freq_(sub);
                    while(f == minf_ && to > from+1)
                    {
                        to--;
                        sub = chunks[i].substr(from, to-from);
                        f = term_freq_(sub);
                    }
					r.push_back(make_pair(chunks[i].substr(from, to-from),f));
					from = to, to++;
					continue;
				}
				if (pre && to+1<chunks[i].length()){to++; continue;}
				r.push_back(make_pair(sub, f));
				from  = to + 1;
				to+=2;
			}
			if (from >= chunks[i].length())
			    continue;
			KString sub = chunks[i].substr(from, to-from);
            //std::cout<<sub<<"<><>><><><\n";
			double f = term_freq_(sub);
			if (f == minf_)
			  for (;from < chunks[i].length();++from)
  				r.push_back(make_pair(chunks[i].substr(from, 1), minf_));
			else r.push_back(make_pair(sub, f));
		}
		for (uint32_t i=0;i<r.size();++i)
        {
            r[i].first.trim();
            if (r[i].first.length() > 0)
                continue;
            r.erase(r.begin()+i);
            --i;
        }
	}


	std::vector<KString> fmm(const KString& line)//forward maximize match
	{
		std::vector<std::pair<KString,double> > v;
		fmm(line, v);
		std::vector<KString> r;
		for ( uint32_t i=0; i<v.size(); ++i)
		  r.push_back(v[i].first);
		return r;
	}

    void
    tokenize(const KString& line, std::vector<std::pair<KString, double> >& v)
    {
        v.clear();
        if (line.length() == 0)return;
        std::vector<KString> chunks = chunk_(line);
        for ( uint32_t i=0; i<chunks.size(); ++i)
        {
			if (is_alphanum_(chunks[i]))
			{
				v.insert(v.end(), std::pair<KString, double>(chunks[i], minf_/10.));
				continue;
			}
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

    static void insert_table(std::pair<std::pair<Dictionary*, izenelib::am::KStringHashTable<KString, bool>*>, 
                std::pair<izenelib::EventQueue<KString*>*, izenelib::am::KStringHashTable<KString, double>*> > para)
    {
        Dictionary* dict = para.first.first;
        //izenelib::am::KStringHashTable<KString, bool>* pref = para.first.second;
        izenelib::EventQueue<KString*>* q = para.second.first;
        izenelib::am::KStringHashTable<KString, double>* tb = para.second.second;
        while(1)
        {
            uint64_t e;
            KString* s=NULL;
            q->pop(s, e);
            if (s == NULL)
              return;
             if (!dict->row(*s))
             {
                 delete s;
                 continue;
             }
            double* f = tb->find(*s);
            if (!f)
              tb->insert(*s, 1);
            else *f += 1;
            delete s;
        }
    }

    static void merge_table(std::pair<izenelib::am::KStringHashTable<KString, double>*,
                                              izenelib::am::KStringHashTable<KString, double>*> para)
    {
        izenelib::am::KStringHashTable<KString, double>* a = para.first;
        izenelib::am::KStringHashTable<KString, double>* b = para.second;
        for ( izenelib::am::KStringHashTable<KString, double>::iterator it=b->begin(); it!=b->end(); ++it)
        {
            double* f = a->find(*it.key());
            if (!f)a->insert(*it.key(), *it.value());
            else (*f) += *it.value();
        }
    }

    static bool asy_train(const std::string& dictnm, const std::vector<std::string>& corpus,
                      const std::string& out, uint32_t parrallel=8)
    {
        Dictionary dict(dictnm);
        izenelib::am::KStringHashTable<KString, bool> prefix;
        try
        {
            prefix.load(dictnm+".prefix");
        }
        catch(std::runtime_error e)
        {
            std::cout<<e.what()<<std::endl;
            gen_prefix_(dictnm, prefix);
        }

        std::vector<izenelib::EventQueue<KString*>* > qs(parrallel, NULL); 
        for ( uint32_t i=0; i<parrallel; ++i)
          qs[i] = new izenelib::EventQueue<KString*>(100000);

        std::vector<izenelib::am::KStringHashTable<KString, double>*> freq_tables(parrallel, NULL);
        for ( uint32_t i=0; i<parrallel; ++i)
          freq_tables[i] = new izenelib::am::KStringHashTable<KString, double>(dict.size()*10, dict.size()+2);

        struct timeval tvafter,tvpre;
        struct timezone tz;
        uint64_t C = 0;
        {
            uint32_t CC = 0;
            gettimeofday (&tvpre , &tz);
            izenelib::Asynchronization asyn(parrallel+1);
            for ( uint32_t i=0; i<parrallel; ++i)
              asyn.new_static_thread(&Tokenize::insert_table, 
                          std::pair<std::pair<Dictionary*, izenelib::am::KStringHashTable<KString, bool>*>, 
                          std::pair<izenelib::EventQueue<KString*>*, izenelib::am::KStringHashTable<KString, double>*> >
                          (std::pair<Dictionary*, izenelib::am::KStringHashTable<KString, bool>*>(&dict, &prefix), 
                           std::pair<izenelib::EventQueue<KString*>*, izenelib::am::KStringHashTable<KString, double>*>(qs[i], freq_tables[i])));

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
                          for ( uint32_t j=i; j-i+1<10 && j<u.length(); ++j)
                          {
                              qs[C%parrallel]->push(new KString(u.substr(i, j-i+1)), C);
                              C++;
                          }
                    }
                    catch(...) {}
                }
            }
            for ( uint32_t i=0; i<parrallel; ++i)
              qs[i]->push(NULL, C);
            asyn.join();
            gettimeofday (&tvafter , &tz);
            std::cout<<"\nPhase A: "<<((tvafter.tv_sec-tvpre.tv_sec)*1000+(tvafter.tv_usec-tvpre.tv_usec)/1000)/60000.<<" min"<<std::endl;
        }
        {
            gettimeofday (&tvpre , &tz);
            uint32_t gap = 1;
            while(gap < parrallel)
            {
                izenelib::Asynchronization asyn(parrallel+1);
                for ( uint32_t i=0; i+gap<parrallel; i+=gap*2)
                  asyn.new_static_thread(&Tokenize::merge_table, 
                              std::pair<izenelib::am::KStringHashTable<KString, double>*, 
                              izenelib::am::KStringHashTable<KString, double>*>(freq_tables[i], freq_tables[i+gap]));
                asyn.join();
                gap*=2;
            }
            std::ofstream of(out.c_str());
            const char* row = NULL;
            while((row = dict.next_row(row))!=NULL)
            {
                //std::cout<<row<<std::endl;
                const char* t = strchr(row, '\t');
                IASSERT(t != NULL);
                KString kstr(std::string(row, t-row));
                double* f = freq_tables[0]->find(kstr);
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
        for ( uint32_t i=0; i<parrallel; ++i)
        {
            delete qs[i];
            delete freq_tables[i];
        }
        return true;
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

    double min()const
    {
        return minf_;
    }
};

}
}//namespace

#endif

