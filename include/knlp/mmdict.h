#ifndef _ILPLIB_KNLP_MMDICT_H_
#define _ILPLIB_KNLP_MMDICT_H_

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

#include <types.h>
#include "knlp/normalize.h"
#include "knlp/darts.h"
#include "am/util/line_reader.h"


namespace ilplib
{
namespace knlp
{

template<class T>class MMDict;
template<class T> void store_values__(const std::vector<std::string>& v, MMDict<T>& dict);
template<class T> void save_values__(const std::string& nm, MMDict<T>& dict);
template<class T> void load_values__(const std::string& nm, MMDict<T>& dict);

template<class T = int32_t>
class MMDict
{
	friend void store_values__<T>(const std::vector<std::string>& v, MMDict<T>& dict);
	friend void save_values__<T>(const std::string& nm, MMDict<T>& dict);
	friend void load_values__<T>(const std::string& nm, MMDict<T>& dict);
	
private:
	Darts::DoubleArray trie_;
	std::vector<T> values_;

	void normalize_(std::string& str)const
	{
		Normalize::normalize(str);
	}

	void load_(const std::string& nm)
	{
		int32_t f = 1;
		try{
			if((f = trie_.open((nm+".bin").c_str()))==0)
				load_values__<T>(nm+".v", *this);
		}
		catch(...)
		{}

		if (0 == f)
			return;

		std::set<std::pair<std::string,std::string> > k_v;
		std::set<std::string> keySet;
		char* li = NULL;
		izenelib::am::util::LineReader lr(nm);
		while((li = lr.line(li)) != NULL)
		{
			char* t = strchr(li, '\t');
			std::string k,v;
			if(t)
			{
				k = std::string(li, t);
				v = std::string(t+1);
			}else
				k = std::string(li);
			normalize_(k);
			if (k.length() == 0 || keySet.find(k)!=keySet.end())
				continue;

			k_v.insert(std::make_pair(k, v));
			keySet.insert(k);
		}
		
		std::vector<const char *> keys(k_v.size());
		std::vector<std::string> v(k_v.size());
		std::vector<std::size_t> lengths(k_v.size());
		std::vector<typename Darts::DoubleArray::value_type> values(k_v.size());

		uint32_t i = 0, ff = 0;
		for (std::set<std::pair<std::string,std::string> >::const_iterator it=k_v.begin();it!=k_v.end();++it,i++)
		{
			keys[i] = it->first.c_str()
			  ,lengths[i]=it->first.length()
			  ,values[i] = i;
			if (it->second.length())
				v[i]=it->second,ff=1;
		}
		assert(keys.size() == v.size());
		trie_.build(keys.size(), &keys[0], &lengths[0], &values[0]);
		trie_.save((nm+".bin").c_str());

		if (ff == 1)
		{
			store_values__<T>(v, *this);
			save_values__(nm+".v", *this);
		}
	}

public:
	MMDict(const std::string& dict_nm)
	{
		load_(dict_nm);
	}

	~MMDict()
	{
	}

	/**
	 * Return value:
	 *  0:  sucess
	 *  1:  sucess, but no value for this key.
	 *  -1: not found
	 * */
	int32_t value(std::string key, T& v, bool nor = true)const
	{
		if (nor)
			normalize_(key);
		Darts::DoubleArray::result_pair_type res;
		trie_.exactMatchSearch(key.c_str(), res, key.length());

		if (res.length == 0 && res.value == -1)
			return -1;

		if ((std::size_t)res.value < values_.size())
		{
			v = values_[res.value];
			return 0;
		}

		return 1;
	}
	

	bool has_key(std::string key, bool nor = true)const
	{
		T v;
		int32_t r = value(key, v, nor);
		if(r >= 0)
			return true;
		return false;
	}
	
	void get_matched_prefix(const std::string& str, const int & index, std::vector<std::string>& words) const
	{
		words.clear();
		Darts::DoubleArray::result_pair_type temp;
		temp.length = 0;
		temp.value = 0;
		
		Darts::DoubleArray::result_pair_type res[12] = {temp};
		
		trie_.commonPrefixSearch(str.c_str() + index, res, 12, 0, 0);
		
		for (unsigned int i = 0; i < 12; ++i) {
			if (res[i].length == 0)
				break;
			words.push_back(str.substr(index, res[i].length));
		}
	}
	
};


template<class T>
inline void store_values__(const std::vector<std::string>& v, MMDict<T>& dict)
{
	dict.values_.resize(v.size(), 0);
	for (uint32_t i=0;i<v.size();++i)
		dict.values_[i] = atoi(v[i].c_str());
}


template<class T>
inline void save_values__(const std::string& nm, MMDict<T>& dict)
{
	FILE* f = fopen(nm.c_str(), "w+");
	if (!f)
		return;
	uint32_t s = dict.values_.size();
	IASSERT(fwrite(&s, sizeof(s), 1, f) == 1);
	IASSERT(fwrite(&dict.values_[0], dict.values_.size()*sizeof(T), 1, f) == 1);
	fclose(f);
}


template<class T>
inline void load_values__(const std::string& nm, MMDict<T>& dict)
{
	FILE* f = fopen(nm.c_str(), "r");
	if (!f)
		return;
	uint32_t s = 0;
	IASSERT(fread(&s, sizeof(s), 1, f) == 1);
	dict.values_.resize(s);
	IASSERT(fread(&dict.values_[0], dict.values_.size()*sizeof(T), 1, f) == 1);
	fclose(f);
}


template<>
inline void store_values__<const char*>(const std::vector<std::string>& v, MMDict<const char*>& dict)
{
	dict.values_.resize(v.size(), NULL);
	for (uint32_t i=0;i<v.size();++i)
	{
		char* b = new char[v[i].length()+1];
		memset(b, 0, v[i].length()+1);
		strcpy(b, v[i].c_str());
		dict.values_[i] = b;
	}
}

template<>
inline void store_values__<float>(const std::vector<std::string>& v, MMDict<float>& dict)
{
	dict.values_.resize(v.size(), 0);
	for (uint32_t i=0;i<v.size();++i)
		dict.values_[i] = atof(v[i].c_str());
}


template<>
inline void store_values__<uint32_t>(const std::vector<std::string>& v, MMDict<uint32_t>& dict)
{
	dict.values_.resize(v.size(), 0);
	for (uint32_t i=0;i<v.size();++i)
		dict.values_[i] = atoi(v[i].c_str());
}


template<>
inline void save_values__<const char*>(const std::string& nm, MMDict<const char*>& dict)
{
	FILE* f = fopen(nm.c_str(), "w+");
	if (!f)
		return;
	uint32_t s = dict.values_.size();
	fwrite(&s, 1, sizeof(s), f);
	for (uint32_t i = 0; i<dict.values_.size(); ++i) {
		s = strlen(dict.values_[i]);
		fwrite(&s, 1, sizeof(s), f);
		fwrite(dict.values_[i], 1, s, f);
	}
	fclose(f);
}


template<>
inline void load_values__<const char*>(const std::string& nm, MMDict<const char*>& dict)
{
	FILE* f = fopen(nm.c_str(), "r");
	if (!f)
		return;
	uint32_t s = 0;
	IASSERT(fread(&s, sizeof(s), 1, f) == 1);
	dict.values_.resize(s);
	for (uint32_t i = 0; i < dict.values_.size(); ++i) {
		IASSERT(fread(&s, sizeof(s), 1, f) == 1);
		char* b = new char[s+1];memset(b, 0, s+1);
		IASSERT(fread(b, s, 1, f) == 1);
		dict.values_[i] = b;
	}
	fclose(f);
}


}
}//namespace

#endif

