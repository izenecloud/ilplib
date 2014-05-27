#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "knlp/get_tags.h"

using namespace ilplib::knlp;
using namespace izenelib;


void printHelp()
{
    std::cout << "./get_tags_util\n" << "        \t-d\t dictionary path\n \
        \t-n\ttop_n tags\n\
        \t-c\tcorpus path\n\
        \t-o\toutput file" << std::endl;
}


int main(int argc,char * argv[])
{
    if (argc == 1)
    {
        printHelp();
        return 0;
    }

    char c = '?';
    std::string dictnm;
    std::string title_tag_freq;
    std::string query_tag_freq;
    unsigned int top_n = 10;
    std::string corpus;
    std::string output;
    while ((c = getopt (argc, argv, "d:n:c:o:")) != -1) {
        switch (c) {
            case 'd':
                dictnm = optarg;
                break;
            case 'n':
                top_n = atoi(optarg);
                break;
            case 'c':
                corpus = optarg;
                break;
            case 'o':
                output = optarg;
                break;
            case '?':
                printHelp();
        }
    }
    
    GetTags gt(dictnm);
    std::ifstream fin(corpus.c_str());
    if (!fin) {
        std::cout << "in main(), failed to open the file: " << corpus << std::endl;
        return 0;
    }
    
    std::ofstream fout(output.c_str());
    if (!fout) {
        std::cout << "in main(), failed to open the file: " << output << std::endl;
        return 0;
    }
    
    std::string line;
    while (getline(fin, line)) {
        if (line.empty())
            continue;
        
        std::vector<std::string> tags = gt.get_tags(line, top_n);
        
        // output
        fout << line << "\ntags:\t";
        for (unsigned int i = 0; i < tags.size(); ++i)
            fout << tags[i] << "\t";
        fout << "\n\n";
    }
    fin.close();
    fout.close();
}


