#ifndef SCD_TREATER_H
#define SCD_TREATER_H

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include "knlp/horse_tokenize.h"
#include "titleknn.h"

using std::string;
using std::map;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::pair;
using std::make_pair;
using std::cout;
using std::endl;

struct Scd_info {
	string title;
	string original_category;
	bool be_ali;
	bool be_book;
	
	Scd_info () {
		be_book = false;
		be_ali = false;
	}
	void clear() {
		title.clear();
		original_category.clear();
		be_book = false;
		be_ali = false;
	}
	bool empty() {
		return title.empty();
	}
};


class SCD_treater {
public:
	SCD_treater(const string& tok_dict_path, const string& knn_model_dir);
	~SCD_treater();
	string classify(const Scd_info& info);
	void add_KNN_to_scd(const string& input_scd, const string& output_scd);
	
	void add_knn_field_to_scd(const string& input_scd, const string& output_scd);
	
private:
	ilplib::knlp::HorseTokenize *tok;
	TitleKNN *knn;
};


SCD_treater::SCD_treater(const string& tok_dict_path, const string& knn_model_dir)
{
	tok = new ilplib::knlp::HorseTokenize(tok_dict_path);
	knn = new TitleKNN(knn_model_dir);
}


SCD_treater::~SCD_treater()
{
	delete tok;
	delete knn;
}


void SCD_treater::add_knn_field_to_scd(const string& input_scd, const string& output_scd)
{
	ifstream fin(input_scd.c_str());
	if (!fin) {
		cout << "failed to open this file: " << input_scd << endl;
		return;
	}
	
	ofstream fout(output_scd.c_str());
	if (!fout) {
		cout << "failed to open this file: " << output_scd << endl;
		return;
	}
	
	Scd_info info;
	
	string line;
	while (getline(fin, line)) {
		if (line.empty())
			continue;
		
		if (line.size() > 7 && line.substr(0,7) == "<DOCID>") {
			if (!info.empty())
				fout << "<KNN>" << classify(info) << "\n";
			info.clear();
		} else if (line.size() > 7 && line.substr(0,7) == "<Title>") {
			info.title = line.substr(7);
		} else if (line.size() > 18 && line.substr(0,18) == "<OriginalCategory>") {
			info.original_category = line.substr(18);
			if (info.original_category.find("图书") != string::npos || info.original_category.find("音像") != string::npos)
				info.be_book = true;
		} else if (line.size() > 8 && line.substr(0,8) == "<Source>") {
			string source = line.substr(8);
			if (source.find("淘宝") != string::npos || source.find("天猫") != string::npos)
				info.be_ali = true;
		}
		fout << line << "\n";
	}
	if (!info.empty())
		fout << "<KNN>" << classify(info) << "\n";
	
	fin.close();
	fout.close();
}


string SCD_treater::classify(const Scd_info& info)
{
	if (info.be_ali || info.be_book)
		return info.original_category;
	
	vector<pair<string, float> > tokenized_title;
	tok->tokenize(info.title, tokenized_title);
	vector<string> terms(tokenized_title.size());
	for (unsigned int i = 0; i < tokenized_title.size(); ++i)
		terms[i] = tokenized_title[i].first;
	
	return knn->predict(terms);
}


#endif  //  #ifndef SCD_TREATER_H
