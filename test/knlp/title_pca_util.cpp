#include <string>
#include<iostream>
#include<fstream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "knlp/title_pca.h"

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib;
void printHelp()
{
    cout<< "./title_pca_util\n"<<"\t-d\t dictionary path\n \
		\t-o\toutput file\n\
		\t-c\tcorpus path\n";
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

    TitlePCA tok(dictnm);
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
            std::vector<std::pair<std::string, float> > tks;
            std::vector<std::pair<std::string, float> > subtks;
            std::string brand,mdt;

            of << line << "\t";
            tok.pca(line, tks, brand, mdt, subtks, true);
            of << brand <<"\t" << mdt << "\t";
            for ( uint32_t i=0; i<tks.size(); ++i)
                of << tks[i].first << "卐" << tks[i].second << "卐";
            of << "\t";
            for ( uint32_t i=0; i<subtks.size(); ++i)
                of << subtks[i].first << "卐" << subtks[i].second << "卐";
            of<<std::endl;
        }
    }

    of.close();

    return 0;
}

