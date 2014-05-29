#ifndef _ILPLIB_NLP_GETTAGS_H_
#define _ILPLIB_NLP_GETTAGS_H_

#include <string>
#include<iostream>
#include<fstream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>

#include "knlp/horse_tokenize.h"


namespace ilplib
{
namespace knlp
{
class GetTags {
private:
    HorseTokenize *tok;
    std::map<std::string,int> tag_rank;

public:
    GetTags(const std::string& dict_path) {
        tok = new HorseTokenize(dict_path);
        read_tag_rank(dict_path);
    }

    ~GetTags() {
        delete tok;
    }

    std::vector<std::string> get_tags(const std::string& str, unsigned int top_n = 5) {
        if (str.find("男") == std::string::npos && str.find("女") == std::string::npos)
            return std::vector<std::string>();

        std::vector<std::pair<std::string, float> > tok_result;
        tok->tokenize(str, tok_result);
        
        std::map<int, std::string> rank_tag;
        for (unsigned int i = 0; i < tok_result.size(); ++i) {
            std::map<std::string, int>::iterator ite = tag_rank.find(tok_result[i].first);
            if (ite != tag_rank.end()) {
                rank_tag.insert(make_pair(ite->second, ite->first));
            }
        }
        
        std::vector<std::string> tags;
        unsigned int count = 1;
        for (std::map<int, std::string>::iterator ite = rank_tag.begin(); ite != rank_tag.end(); ++ite) {
            if (count++ > top_n)
                break;
            tags.push_back(ite->second);
        }
        
        return tags;
    }
    
private:
    void read_tag_rank(const std::string& dict_path) {
        // read tags and its rank in dict file
        std::string dict_file = dict_path + "/token.dict";
        std::ifstream fin(dict_file.c_str());
        if (!fin) {
            std::cout << "in read_tag_rank(const string&), failed to open this file: " << dict_file << std::endl;
            exit(1);
        }

        std::string line;
        std::string tag;
        int rank;
        while (getline(fin, line)) {
            if (line.empty())
                continue;
            std::istringstream ss(line);
            ss >> tag;
            ss >> rank;
            tag_rank[tag] = rank;
        }
        fin.close();
    }
};


}
}

#endif
