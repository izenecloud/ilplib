#include <string>
#include<iostream>
#include<fstream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "knlp/differential_token.h"

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib;
void printHelp()
{
    cout<< "./differential_token_util\n"<<"\t-d\t dictionary path\n \
		\t-o\toutput file\n\
		\t-c\tcorpus path\nT1\\tT2\\t....@clicked_index1\\tclicked_index2....\n";
}

std::vector<std::string> split(std::string line)
{
    std::vector<std::string> titles;
    const char* t = strchr(line.c_str(), '\t');
    while(t)
    {
        titles.push_back(line.substr(0, t-line.c_str()));
        line = t+1;
        t = strchr(line.c_str(), '\t');
    }
    titles.push_back(line);
    
    return titles;
}

int main(int argc,char * argv[])
{
    if (argc == 1)
    {
        printHelp();
        return 0;
    }

    char c = '?';
    string dictnm;
    string output;
    vector<string> corpus;
    while ((c = getopt (argc, argv, "d:o:c:")) != -1)
        switch (c)
        {
        case 'd':
            dictnm = optarg;
            break;
        case 'o':
            output = optarg;
            break;
        case 'c':
            corpus.push_back(optarg);
            break;
        case '?':
            printHelp();
        }

    if (output.length() == 0)
    {
        std::cout<<"Output file please!\n";
        printHelp();
        return 1;
    }

    ofstream of;
    of.open(output.c_str());

    DifferentialToken tok(dictnm);
    uint32_t t = 0;
    while(1)
    {
        if (t >= corpus.size() && corpus.size()>0)
            break;
        if (t<corpus.size())
        {
            if(freopen (corpus[t].c_str(), "r", stdin) == NULL)
              ;
            ++t;
        }
        string line;
        while(!std::getline(std::cin, line).eof())
        {
            const char* a  = strchr(line.c_str(), '@');
            if (!a){std::cout<<"Wrong format\n";continue;}
            std::vector<std::string> titles = split(line.substr(0, a - line.c_str()));
            std::vector<float> pri(titles.size(), 0);
            std::vector<std::string> cl = split(a+1);
            std::vector<uint32_t> clicked;
            for (uint32_t i=0;i<cl.size();i++)
                clicked.push_back(atoi(cl[i].c_str()));

            std::vector<std::string> v = tok.different(titles, pri, clicked);
            for (uint32_t i=0;i<v.size();i++)
                of<<v[i]<<"\t";
            of<<endl;
        }
    }

    of.close();

    return 0;
}

