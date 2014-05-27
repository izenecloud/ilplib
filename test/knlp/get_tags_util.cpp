#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "knlp/get_tags.h"

using namespace std;
using namespace ilplib::knlp;
using namespace izenelib;


void printHelp()
{
    cout << "./get_tags_util\n" << "        \t-d\t dictionary path\n \
        \t-p\ttitle tag frequency file\n\
        \t-q\tquery tag frequency file\n\
        \t-n\ttop_n tags\n\
        \t-c\tcorpus path\n\
        \t-o\toutput file\n";
    cout << "  eg.: ./get_tags_util -d ../dict -p ../dict/title_tag_freq -q ../dict/query_tag_freq \
 -n 5 -c ../dict/test_corpus.txt  -o ../dict/test_result.txt" << endl;
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
    string title_tag_freq;
    string query_tag_freq;
    unsigned int top_n = 10;
    string corpus;
    string output;
    while ((c = getopt (argc, argv, "d:p:q:n:c:o:")) != -1) {
        switch (c) {
            case 'd':
                dictnm = optarg;
                break;
            case 'p':
                title_tag_freq = optarg;
                break;
            case 'q':
                query_tag_freq = optarg;
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
    
    GetTags gt(dictnm, title_tag_freq, query_tag_freq);
    ifstream fin(corpus.c_str());
    if (!fin) {
        cout << "in main(), failed to open the file: " << corpus << endl;
        return 0;
    }
    
    ofstream fout(output.c_str());
    if (!fout) {
        cout << "in main(), failed to open the file: " << output << endl;
        return 0;
    }

    string line;
    while (getline(fin, line)) {
        if (line.empty())
            continue;
        
        vector<string> tags = gt.get_tags(line, top_n);
        
        // output
        fout << line << "\ntags:\t";
        for (unsigned int i = 0; i < tags.size(); ++i)
            fout << tags[i] << "\t";
        fout << "\n\n";
    }
    fin.close();
    fout.close();

    // gt.output_tag_rank("../dict/model.txt");
}


