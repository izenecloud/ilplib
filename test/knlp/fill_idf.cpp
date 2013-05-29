/*
 * =====================================================================================
 *
 *       Filename:  fill_idf.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2013年05月24日 08时55分41秒
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

#include "knlp/tokenize.h"
#include "knlp/normalize.h"
#include "am/hashtable/khash_table.hpp"
#include "am/util/line_reader.h"
#include "net/seda/queue.hpp"

#include <boost/thread/thread.hpp>

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib::am;
using namespace izenelib::am::util;
using namespace izenelib;

void reduce(KStringHashTable<KString, double>* a, KStringHashTable<KString, double>* b)
{
    for ( izenelib::am::KStringHashTable<KString, double>::iterator it=b->begin(); it!=b->end(); ++it)
    {
        double* f = a->find(*it.key());
        if (!f)a->insert(*it.key(), *it.value());
        else (*f) += *it.value();
    }
}

void mapping(EventQueue<std::string*>* q, KStringHashTable<KString, double>* idft, ilplib::knlp::Tokenize* tkn)
{
    while(1)
    {
        uint64_t e =  -1;
        std::string* str = NULL;
        q->pop(str, e);
        if (str == NULL)
          return;
        KString kstr(*str);
        delete str;
        ilplib::knlp::Normalize::normalize(kstr);
        std::vector<KString> v = tkn->tokenize(kstr);
        std::set<KString> set;
        for ( uint32_t t=0; t<v.size(); ++t)
        {
            v[t].trim();
            if (v[t].length() > 0)
              set.insert(v[t]);
        }
        for ( std::set<KString>::iterator it=set.begin(); it!=set.end(); ++it)
        {
            double* f = idft->find(*it);
            if (f)(*f)++;
            else idft->insert(*it, 1);
        }
    }
}

int main(int argc,char * argv[])
{
    if (argc < 4)
    {
        KStringHashTable<KString, double> idft;
        idft.load("./term.idf");
        double* w = idft.find(KString("[[NONE]]"));
        std::cout<<*w<<std::endl;
        std::cout<<argv[0]<<" [tokenize dict] [output] [corpus 1] [corpus 2] ....\n";
        return 0;
    }

    string dictnm = argv[1];
    string output = argv[2];

    ilplib::knlp::Tokenize tkn(dictnm);
    uint32_t docn = 0;
    std::vector<KStringHashTable<KString, double>*> idfts;
    const uint32_t parall = 8;
    {//mapping
        std::vector<boost::thread*> threads;
        std::vector<EventQueue<std::string*>*> qs;
        for ( uint32_t i=0; i<parall; ++i)
        {
            qs.push_back(new EventQueue<std::string*>(1000));
            idfts.push_back(new KStringHashTable<KString, double>(tkn.size()*3, tkn.size()+3));
            threads.push_back(new boost::thread(&mapping, qs[i], idfts[i], &tkn));
        }

        for ( int32_t i=3; i<argc; ++i)
        {
            LineReader lr(argv[i]);
            char* line = NULL;
            while((line=lr.line(line))!=NULL)
            {
                if (strlen(line) == 0)continue;
                std::cout<<"\r"<<docn<<std::flush;
                docn++;
                qs[docn%parall]->push(new std::string(line), -1);
            }
        }
        for ( uint32_t i=0; i<parall; ++i)
          qs[i]->push(NULL, -1);
        for ( uint32_t i=0; i<parall; ++i)
        {
            threads[i]->join();
            delete threads[i];
            threads[i] = NULL;
            delete qs[i];
        }
    }

    {//merge
        uint32_t gap = 1;
        while(gap < parall)
        {
            std::vector<boost::thread*> threads;
            for ( uint32_t i=0; i+gap<parall; i+=gap*2)
              threads.push_back(new boost::thread(&reduce, idfts[i], idfts[i+gap]));
            gap*=2;
            for ( uint32_t i=0; i<threads.size(); ++i)
            {
                threads[i]->join();
                delete threads[i];
            }
        }
    }

    double ave = 0;
    for(KStringHashTable<KString, double>::iterator it = idfts[0]->begin(); it!=idfts[0]->end(); ++it)
    {
        double t = *it.value();
        *it.value() = log((docn-t+0.5)/(t+0.5));
        ave += *it.value();
    }
    ave /= idfts[0]->size();
    idfts[0]->insert(KString("[[NONE]]"), ave);

    idfts[0]->persistence(output);

    return 0;
}
