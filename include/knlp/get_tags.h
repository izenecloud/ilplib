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


using namespace std;

namespace ilplib
{
namespace knlp
{
class Compare_tag {
public:
    bool operator() (const pair<string,float>& left, const pair<string,float>& right) {
        if (left.second > right.second)
            return true;
        else if (left.second < right.second)
            return false;
        else {
            if (left.first < right.first)
                return true;
            else
                return false;
        }
    }
};


class GetTags{
private:
    HorseTokenize *tok;
    map<string,int> tag_rank;

public:
    GetTags(const string& dict_file, const string& title_tag_freq_file, const string& query_tag_freq_file) {
        tok = new HorseTokenize(dict_file);
        read_tag_freq(title_tag_freq_file, query_tag_freq_file);
    }

    ~GetTags() {
        delete tok;
    }

    vector<string> get_tags(const string& str, unsigned int top_n = 5) {
        if (str.find("男") == string::npos && str.find("女") == string::npos)
            return vector<string>();

        vector<pair<string, float> > tok_result;
        tok->tokenize(str, tok_result);
        
        map<int, string> rank_tag;
        for (unsigned int i = 0; i < tok_result.size(); ++i) {
            map<string, int>::iterator ite = tag_rank.find(tok_result[i].first);
            if (ite != tag_rank.end()) {
                rank_tag.insert(make_pair(ite->second, ite->first));
            }
        }
        
        vector<string> tags;
        unsigned int count = 1;
        for (map<int, string>::iterator ite = rank_tag.begin(); ite != rank_tag.end(); ++ite) {
            if (count++ > top_n)
                break;
            tags.push_back(ite->second);
        }
        
        return tags;
    }
    
    
    void read_tag_freq(const string& title_tag_freq_file, const string& query_tag_freq_file) {
        map<string, pair<int,int> > tag_freq; // tag --> its freqencies in title file and query file
        ifstream fin;
        string line;
        string tag;
        int freq;
        
        // read tags and its frequencies in title file
        fin.open(title_tag_freq_file.c_str());
        if (!fin) {
            cout << "failed to open this file: " << title_tag_freq_file << endl;
            exit(1);
        }
        while (getline(fin, line)) {
            if (line.empty())
                continue;
            istringstream ss(line);
            ss >> tag;
            ss >> freq;
            tag_freq[tag] = make_pair(freq, 0);
        }
        fin.close();
                
        // read tags and its frequencies in query file
        fin.open(query_tag_freq_file.c_str());
        if (!fin) {
            cout << "failed to open this file: " << query_tag_freq_file << endl;
            exit(1);        
        }
        while (getline(fin, line)) {
            if (line.empty())
                continue;
            istringstream ss(line);
            ss >> tag;
            ss >> freq;
            map<string, pair<int,int> >::iterator ite = tag_freq.find(tag);
            if (ite != tag_freq.end())
                ite->second.second = freq;
        }
        fin.close();
        
        rank_tags(tag_freq); // rank the tags
        tag_freq.clear();
    }

    void output_tag_rank(const string& output_model_file) {
        ofstream fout(output_model_file.c_str());
        map<int,string> rank_tag;
        for (map<string, int>::iterator ite = tag_rank.begin(); ite != tag_rank.end(); ++ite) {
            rank_tag.insert(make_pair(ite->second, ite->first));
        }
        
        for (map<int,string, greater<int> >::iterator it = rank_tag.begin(); it != rank_tag.end(); ++it)
            fout << it->second << "\t" << it->first << "\n";
        fout.close();
    }

private:
    void rank_tags(map<string, pair<int,int> >& tag_freq) {
        tag_rank.clear();
        map<string, pair<int,int> >::iterator ite = tag_freq.begin();
        
        set<pair<string, float>, Compare_tag> tag_value;
        for (; ite != tag_freq.end(); ++ite) {
            float log_a = log(2 + ite->second.first);
            float log_b = log(2 + ite->second.second);
            float value = log_a * log_b + 10*(log_b/log_a);
            tag_value.insert(make_pair(ite->first, value));
        }
        
        // convert tag_value to tag_rank
        int rank = 0;
        for (set<pair<string, float> >::iterator it = tag_value.begin(); it != tag_value.end(); ++it) {
            tag_rank.insert(make_pair(it->first, rank));
            ++rank;
        }

        // output detail freq and value, just for debug
        // ofstream of("tag_freq_value.txt");
        // for (set<pair<string, float> >::iterator it = tag_value.begin(); it != tag_value.end(); ++it)
        //    of << it->first << "\t" << tag_freq[it->first].first << "\t" << tag_freq[it->first].second << "\t" << it->second << "\n";
    }
};


}
}

#endif
