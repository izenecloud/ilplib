/*
 * =====================================================================================
 *
 *       Filename:  dictionary.h
 *
 *    Description:  A static string-to-string hash table.
 *
 *        Version:  1.0
 *        Created:  2013年05月22日 12时12分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), kevin.hu@b5m.com
 *        Company:  B5M.com
 *
 * =====================================================================================
 */

#ifndef _ILPLIB_NLP_DICTIONARY_H_
#define _ILPLIB_NLP_DICTIONARY_H_

#include <string>
#include <vector>
#include <list>
#include <cstdlib>
#include <algorithm>
#include <istream>
#include <ostream>
#include <limits>

#include "types.h"
#include "am/hashtable/khash_table.hpp"
#include "util/string/kstring.hpp"
#include "normalize.h"

using namespace izenelib::util;

namespace ilplib
{
namespace knlp
{

class Dictionary
{
    izenelib::am::KStringHashTable<izenelib::util::KString, uint32_t>* dict_;
    char* buf_;
    uint32_t bytes_;
    char delimitor_;

public:
    Dictionary(const std::string& nm, char delimitor='\t')
        : dict_(NULL)
        , buf_(NULL)
        , bytes_(0)
        , delimitor_(delimitor)
    {
        delimitor_ = delimitor;
        FILE* f = fopen(nm.c_str(), "r");
        if (!f)throw std::runtime_error("can't open file.");
        fseek(f, 0, SEEK_END);
        bytes_ = ftell(f);
        fseek(f, 0, SEEK_SET);
        buf_ = new char[bytes_+1]();
        if(fread(buf_, bytes_, 1, f)!=1)throw std::runtime_error("File Read error.");
        fclose(f);

        char* m = buf_;
        uint32_t t = 0;
        while(*m && m-buf_ < bytes_)
        {
            if (*m == '\n')
                t++;
            m++;
        }

        IASSERT(t > 0);
        dict_ = new izenelib::am::KStringHashTable<KString, uint32_t>(t*3, t+2);
        m = buf_;
        char* la = m;
        while(*m && m-buf_ < bytes_)
        {
            if (*m == '\n')
            {
                *m = 0;
                KString kstr(la);
                Normalize::normalize(kstr);
                kstr = kstr.substr(0, kstr.index_of(delimitor));
                dict_->insert(kstr, la-buf_);
                la = m + 1;
            }
            m++;
        }
        if (*la == 0 || la - buf_ >= bytes_)
            return;
        KString kstr(la);
        Normalize::normalize(kstr);
        kstr = kstr.substr(0, kstr.index_of(delimitor));
        dict_->insert(kstr, la-buf_);
    }

    ~Dictionary()
    {
        delete dict_;
        delete[] buf_;
    }

    uint32_t size()const
    {
        return dict_->size();
    }

    char* value(KString kstr, bool nor = true)
    {
        if(nor)Normalize::normalize(kstr);
        uint32_t* p = dict_->find(kstr);
        if (!p) return NULL;
        char* l = buf_ + (*p);
        char* v = strchr(l, delimitor_);
        if (!v)return l;
        return v+1;
    }

    char* row(KString kstr, bool nor = true)
    {
        if(nor)Normalize::normalize(kstr);
        uint32_t* p = dict_->find(kstr);
        if (!p) return NULL;
        return buf_ + (*p);
    }

    const char* next_row(const char* r=NULL)
    {
        if (r == NULL)
            return buf_;
        while(*r != 0 && r < buf_+bytes_)
            r ++;
        r++;
        if (*r == 0)
            return NULL;
        if (r < buf_+bytes_)
            return r;
        return NULL;
    }
};

class VectorDictionary
{
    typedef izenelib::am::KStringHashTable<izenelib::util::KString, std::vector<char*>* > DictT;
    DictT* dict_;
    char* buf_;
    char delimitor_;

public:
    VectorDictionary(const std::string& nm, char delimitor='\t')
        : dict_(NULL)
        , buf_(NULL)
        , delimitor_(delimitor)
    {
        FILE* f = fopen(nm.c_str(), "r");
        if (!f)throw std::runtime_error("can't open file.");
        fseek(f, 0, SEEK_END);
        uint32_t bytes = ftell(f);
        fseek(f, 0, SEEK_SET);
        buf_ = new char[bytes+1]();
        if(fread(buf_, bytes, 1, f)!=1)throw std::runtime_error("File Read error.");
        fclose(f);

        char* m = buf_;
        uint32_t T = 0;
        while(*m && m-buf_ < bytes)
        {
            if (*m == '\n')
                T++;
            m++;
        }

        IASSERT(T > 0);
        dict_ = new izenelib::am::KStringHashTable<KString, std::vector<char*>*>(T*3, T+2);
        m = buf_;
        char* la = m;
        while(*m && m-buf_ < bytes)
        {
            if (*m == '\n')
            {
                *m = 0;
                char* t = strchr(la, delimitor);
                if (!t)
                    throw std::runtime_error(std::string("File format is not correct: ")+la);
                *t = 0;
                t++;

                std::vector<char*>* vt = new std::vector<char*>();
                vt->push_back(t);
                t = strchr(t, delimitor);
                while(t)
                {
                    *t = 0;
                    t++;
                    vt->push_back(t);
                    t = strchr(t, delimitor);
                }
                KString kstr(la);
                Normalize::normalize(kstr);
                if(dict_->find(kstr) == NULL)
                    dict_->insert(kstr, vt);
                else delete vt;
                la = m + 1;
            }
            m++;
        }
        if (*la == 0 || la - buf_ >= bytes)
            return;
        char* t = strchr(la, delimitor);
        if (!t)
            return;
        *t = 0, t++;
        std::vector<char*>* vt = new std::vector<char*>();
        vt->push_back(t);
        t = strchr(t, delimitor);
        while(t)
        {
            *t = 0;
            t++;
            vt->push_back(t);
            t = strchr(t, delimitor);
        }

        KString kstr(la);
        Normalize::normalize(kstr);
        if(dict_->find(kstr) == NULL)
            dict_->insert(kstr, vt);
        else delete vt;
    }

    ~VectorDictionary()
    {
        for (DictT::iterator it = dict_->begin(); it != dict_->end(); ++it)
        {
            delete[] *it.value();
        }
        delete dict_;
        delete[] buf_;
    }

    uint32_t size()const
    {
        return dict_->size();
    }

    std::vector<char*>** value(KString kstr, bool nor = true)
    {
        if(nor)Normalize::normalize(kstr);
        std::vector<char*>** p = dict_->find(kstr);
        return p;
    }
};

class DigitalDictionary
{
    izenelib::am::KStringHashTable<izenelib::util::KString, double>* dict_;
    char* buf_;
    char delimitor_;

public:
    DigitalDictionary(const std::string& nm, char delimitor='\t')
        : dict_(NULL)
        , buf_(NULL)
        , delimitor_(delimitor)
    {
        FILE* f = fopen(nm.c_str(), "r");
        if (!f)throw std::runtime_error("can't open file.");
        fseek(f, 0, SEEK_END);
        uint32_t bytes = ftell(f);
        fseek(f, 0, SEEK_SET);
        buf_ = new char[bytes+1]();
        if(fread(buf_, bytes, 1, f)!=1)throw std::runtime_error("File Read error.");
        fclose(f);

        char* m = buf_;
        uint32_t T = 0;
        while(*m && m-buf_ < bytes)
        {
            if (*m == '\n')
                T++;
            m++;
        }

        IASSERT(T > 0);
        dict_ = new izenelib::am::KStringHashTable<KString, double>(T*3, T+2);
        m = buf_;
        char* la = m;
        while(*m && m-buf_ < bytes)
        {
            if (*m == '\n')
            {
                *m = 0;
                char* t = strchr(la, delimitor);
                if (!t)
                    throw std::runtime_error(std::string("File format is not correct: ")+la);
                *t = 0;
                t++;
                KString kstr(la);
                Normalize::normalize(kstr);
                dict_->insert(kstr, atof(t));
                la = m + 1;
            }
            m++;
        }
        if (*la == 0 || la - buf_ >= bytes)
        {
            return;
        }
        char* t = strchr(la, delimitor);
        if (!t)
            return;
        *t = 0, t++;
        KString kstr(la);
        Normalize::normalize(kstr);
        kstr = kstr.substr(0, kstr.index_of(delimitor));
        dict_->insert(kstr, atof(t));
    }

    ~DigitalDictionary()
    {
        delete dict_;
        delete[] buf_;
    }

    uint32_t size()const
    {
        return dict_->size();
    }

    double value(KString kstr, bool nor = true)
    {
        if(nor)Normalize::normalize(kstr);
        double* p = dict_->find(kstr);
        if (!p) return (double)std::numeric_limits<int>::min();
        return *p;
    }
};

}
}//namespace

#endif

