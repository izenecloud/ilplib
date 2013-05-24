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

#include "types.h"
#include "am/hashtable/khash_table.hpp"
#include "util/string/kstring.hpp"
#include "normalize.h"

using namespace izenelib::util;

namespace ilplib{
	namespace knlp{

class Dictionary
{
	izenelib::am::KStringHashTable<izenelib::util::KString, uint32_t>* dict_;
	char* buf_;
	uint32_t bytes_;
	char delimitor_;

	public:
	Dictionary(const std::string& nm, char delimitor='\t')
	{
		delimitor_ = delimitor;
		FILE* f = fopen(nm.c_str(), "r");
		if (!f)throw std::runtime_error("can't open file.");
		fseek(f, 0, SEEK_END);
		bytes_ = ftell(f);
		fseek(f, 0, SEEK_SET);
		buf_ = new char[bytes_+1];
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
		delete buf_;
		delete dict_;
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

}}//namespace

#endif

