/*
 * =====================================================================================
 *
 *       Filename:  etao_downloader.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2013年05月24日 14时09分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), kevin.hu@b5m.com
 *        Company:  B5M.com
 *
 * =====================================================================================
 */
#include <string>
#include<iostream>
#include<fstream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "am/hashtable/khash_table.hpp"
#include "am/util/line_reader.h"
#include "net/seda/queue.hpp"

#include <boost/regex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <netinet/in.h>
#include <curl/curl.h>

using namespace std;
using namespace izenelib::am;
using namespace izenelib::am::util;
using namespace izenelib;


std::map<std::string, int32_t> proxys;
KStringHashTable<string,bool> outs;

void load_proxy(const string file)
{
	LineReader lr(file);
	char* line = NULL;
	while((line=lr.line(line))!=NULL)
	{
		if (strlen(line) == 0)continue;
		proxys[string(line)] = 5;
	}
}

void load_output(const string file)
{
	LineReader lr(file);
	char* line = NULL;
	while((line=lr.line(line))!=NULL)
	{
		if (strlen(line) == 0)continue;
		char* t = strchr(line, '\t');
		if (!t)t = strchr(line, ' ');
		if (!t)continue;
		outs.insert(string(line, t-line), true);
	}
}

std::string url(const std::string& q)
{
	string url = "http://s.etao.com/search?q=";
	url += q + "&s=0&size=36&tbpm=t&style=grid&cd=false&tab=all&usearch=yes";
	return url;
}

#define WEB_SIZE 2000000
struct GrabPage_MemoryStruct {
	char *memory;
	size_t size;
	GrabPage_MemoryStruct(char* m)
		:memory(m), size(0)
	{
		memset(memory, 0, WEB_SIZE);
	}

	~GrabPage_MemoryStruct()
	{
	}
};

static size_t
GRAB_PAGE_CALLBACK(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct GrabPage_MemoryStruct *mem = (struct GrabPage_MemoryStruct *)userp;

	if (mem->size + realsize + 1 > WEB_SIZE)
	  return 0;

	assert (mem->memory);
	memcpy(mem->memory+mem->size, contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

string download_extract(const string& query, const string& proxy, bool& succ)
{
	succ = false;
	CURL *curl = curl_easy_init();
	if (!curl){std::cout<<"[ERROR]CURL init error!";return "";}
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	const char * dafault_agent = "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10.6; en-US; "
		"rv:1.9.2.16) Gecko/20110319 Firefox/3.6.16";
	curl_easy_setopt(curl, CURLOPT_USERAGENT, dafault_agent);

	curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
	curl_easy_setopt(curl, CURLOPT_URL, url(query).c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GRAB_PAGE_CALLBACK);

	struct GrabPage_MemoryStruct mem(new char[WEB_SIZE]);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&mem);

	std::string r;
	CURLcode res = curl_easy_perform(curl);
	if(res != CURLE_OK)
	{
  		std::cout<<"[ERROR]\t"<<query<<"\t"<<curl_easy_strerror(res)<<std::endl;
	}
	else{//extract what we want
		string html(mem.memory);
		for ( uint32_t i=0; i<html.length(); ++i)
		  if (html[i] == '\n' || html[i] == '\r')
			html[i] = ' ';
        {
            boost::regex reg(KString(string("<span class=H>")+query+"</span>").get_bytes("GBK"));
            boost::smatch m;
            boost::regex_match(html, m, reg);
            if (m.size() == 0)
            {
                cout<<query<<"[Inappropriate Term]\n";
                succ = true;
                delete mem.memory;
                curl_easy_cleanup(curl);
                return "";
            }
        }
		{
			boost::regex reg(KString("<h4 class=\"title\">商家筛选</h4>").get_bytes("GBK"));
			html = boost::regex_replace(html, reg, "@@@@");
		}
		boost::regex reg(KString("<h4>所有分类</h4>[ ]+<ul>([^@]+)@@@@").get_bytes("GBK"));
		boost::smatch m;
		if (boost::regex_search(html, m, reg) && m[1].matched)
		{
			html = m[1].str();
			boost::regex ht("<[^>]+>");
			html = boost::regex_replace(html, ht, " ");
			boost::regex sp_patt("[ ]{2,}");
			html = boost::regex_replace(html, sp_patt, " ");
			boost::regex bra_patt("[ ]+\\(");
			r = boost::regex_replace(html, bra_patt, "(");
			succ = true;
		}
		else if(html.length() > 30000)
		{
		  std::cout<<query<<"[Can't find the pattern!]\n";
		  //std::cout<<html<<std::endl;
			succ = true;
		}
		else cout<<query<<"[Maybe be blocked or something]\n";
	}
	delete mem.memory;
	curl_easy_cleanup(curl);
	return r;
}


string get_proxy()
{
	std::vector<std::string> v;
	v.reserve(proxys.size());

	bool total = (rand()%17==0);
	for ( std::map<std::string,int32_t>::const_iterator it=proxys.begin(); it!=proxys.end(); ++it)
	  if (it->second > 0 || total)
  		v.push_back(it->first);
	if (v.size() == 0)
	  for ( std::map<std::string,int32_t>::const_iterator it=proxys.begin(); it!=proxys.end(); ++it)
		v.push_back(it->first);

    std::cout<<"[PROXY]:"<<v.size()<<"-"<<total<<std::endl;
	return v[rand()%v.size()];
}

void download_stage(EventQueue<string*>* in, EventQueue<string*>* out)
{
	while(1)
	{
		string* s=NULL;
		uint64_t e;
		in->pop(s, e);
		if (s == NULL)
		  break;

		bool succ = false;
		string r = "";
		while(!succ)
		{
			string pro = get_proxy();
			r = download_extract(*s, pro, succ);
			if (!succ)
				proxys[pro]--;
			else
				proxys[pro]++;
		}

		if (r.length() > 0)
        {
            try{
                r = KString(r, "GBK").get_bytes("utf-8");
            }catch(...)
            {
                std::cout<<"[ENCODING ERORRO]\n";
            }
            out->push(new string(*s + "\t" + r), -1);
        }
		delete s;
	}
}

std::ofstream of;

void output_stage(EventQueue<string*>* out)
{
	while(1)
	{
		string* s=NULL;
		uint64_t e;
		out->pop(s, e);
		if (s == NULL)
		  return;

		of << *s<<std::endl;
		of.flush();
		std::cout<<*s<<std::endl;
		delete s;
	}
}

int main(int argc,char * argv[])
{
	if (argc < 4)
	{
		std::cout<<argv[0]<<" [query] [proxy] [output]\n";
		return 0;
	}

	string qu = argv[1];

	of.open(argv[3],  std::ofstream::out | std::ofstream::app);
	if (!of.is_open())
	  of.open(argv[3]);

	load_proxy(argv[2]);
	load_output(argv[3]);

	EventQueue<string*> q, o;
	uint32_t cpu_num = 20;
	std::vector<boost::thread*> download_ths;
	for ( uint32_t i=0; i<cpu_num; ++i)
	  download_ths.push_back(new boost::thread(&download_stage, &q, &o));
	boost::thread out_th(&output_stage, &o);

	curl_global_init(CURL_GLOBAL_DEFAULT);
	{
		LineReader lr(qu);
		char* line = NULL;
		while((line=lr.line(line))!=NULL)
		{
			if (strlen(line) == 0)continue;
			char* t = strchr(line, '\t');
			if (!t)t = strchr(line, ' ');
			if (!t)t = line+strlen(line);
			bool* e = outs.find(string(line, t-line));
			if (e)continue;
			q.push(new string(line, t-line), -1);
		}
	}

	for ( uint32_t i=0; i<cpu_num; ++i)
	  q.push(NULL, -1);
	for ( uint32_t i=0; i<cpu_num; ++i)
	  download_ths[i]->join(),delete download_ths[i];
	o.push(NULL, -1);
	out_th.join();
	of.close();

	return 0;
}
/*
gawk -F' ' '{for(i=1;i<=NF;++i)print $i}' etao_downloader.out |\
grep "("|sed -e 's/(\([0-9\.万,]\+\))/ \1/g' | \
sed -e "s/\/[^\/]\+\.\.\./\//g" -e "s/\.\.\.//g" -e "s/href=[^>]\+>//g"|\
sort -t" " -k1,1 |\
gawk -F" " '
function atof(str)
{
    i = index(str, "万")
    wan = 1;
    if (i > 0)
    {
        str = substr(str, 1, i-1)
        wan = 10000;
    }
    gsub(/,/, "", str)
    return str*wan;
}
{
    total += atof($2);
    if(last!=$1 && length(last)>0)
    {
#print last" "num;
        a[last]=num
        last=$1;
        num=atof($2);
    }
    else{
        num+=atof($2);
        last = $1
    }
}
END{
    a[last]=num;
#print last" "num;
    for (i in a)if(a[i]!=0)
        print i"\t"log(a[i]+1000000)
}' > etao.cate.sort

sed "s/ 其[它他] .\+//g" etao_downloader.out|gawk -F' ' '{for(i=2;i<=NF;++i)print $1" "$i}' |\
grep "("|sed -e 's/(\([0-9\.万,]\+\))/ \1/g' | \
sed -e "s/\/[^\/]\+\.\.\./\//g" -e "s/\.\.\.//g" -e "s/href=[^>]\+>//g"|\
sort -t" " -k1,1 |\
gawk -F" " '
function atof(str)
{
    i = index(str, "万")
    wan = 1;
    if (i > 0)
    {
        str = substr(str, 1, i-1)
        wan = 10000;
    }
    gsub(/,/, "", str)
    return str*wan;
}
{
    print $1" "$2"\t"log(atof($3)+1000)
}
' > etao.term.cate


sed "s/ 其[它他] .\+//g" etao_downloader.out|gawk -F' ' '{for(i=2;i<=NF;++i)print $1" "$i}' |\
grep "("|sed -e 's/(\([0-9\.万,]\+\))/ \1/g' | \
sed -e "s/\/[^\/]\+\.\.\./\//g" -e "s/\.\.\.//g" -e "s/href=[^>]\+>//g"|\
gawk -F" " '
function atof(str)
{
    i = index(str, "万")
    wan = 1;
    if (i > 0)
    {
        str = substr(str, 1, i-1)
        wan = 10000;
    }
    gsub(/,/, "", str)
    return str*wan;
}
{
    n = atof($3)
    term[$1] += n
    tc[$1" "$2] = n
    cate[$2] = 1;
}
END{
    for (ttcc in tc)
    {
        split(ttcc, a, " ");
        if(term[a[1]] == 0)continue;
        p = 1.*(tc[ttcc]+1)/(term[a[1]]+100);
#print a[1]":"a[2]"=="p
        tw[a[1]] += p*log(p)
    }
    for (i in tw)
        print i"\t"(10**(4 + tw[i]))
}'|sort -t $'\t' -k2,2nr > etao.term;echo -e "[MIN]\t0.3" >> etao.term

grep -ve "^[-―]" etao.term.cate |sort -t' ' -k1,1 |\
gawk -F'[ \t]' '
{
    if(last != $1 && length(last)>0)
    {
        print last"\t"cates;
        last = $1;cates=$2;
    }
    else{
        last = $1;
        if(length(cates)>0)cates = cates" "$2;
        else cates=$2;
    }
}
END{
    print last"\t"cates;
}'  > etao.term.cates
gawk -F"\t" '{print $2}' etao_downloader.out |\
sed -e 's/([0-9\.,]\+[万]*)/ /g' -e "s/[\/()*（ ）]/ /g" \
-e 's/ [^ ]\+\.\.\./ /g' -e 's/\.\.\./ /g'| \
gawk -F' ' '{for (i=1;i<=NF;++i)if(length($i)<6)a[$i]++}END{for (i in a)print i"\t"a[i]}'|sort -t$'\t' -k2,2nr > cats.txt
* */
