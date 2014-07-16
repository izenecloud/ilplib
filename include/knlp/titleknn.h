#ifndef TITLEKNN_H
#define TITLEKNN_H

#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <string>
#include <map>
#include <math.h>


using std::vector;
using std::string;
using std::map;
using std::multimap;
using std::set;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::make_pair;
using std::pair;


struct Doc {
	vector<int> ids; // must be sorted
	
	Doc() {}
	Doc(const vector<int>& wordIDs) { ids = wordIDs;}
	void reset(const vector<int>& wordIDs) { ids = wordIDs;}
	void swap(vector<int>& wordIDs) {ids.swap(wordIDs);}
	int size() {return ids.size();}
	int logicaland_len(Doc& doc1)  {
		int len1 = ids.size();
		int len2 = doc1.ids.size();
		int len = 0;
		int i = 0, j = 0;
		while (i < len1 && j < len2) {
			if (ids[i] == doc1.ids[j]) {
				++len; ++i; ++j;
			}
			else if (ids[i] < doc1.ids[j])
				++i;
			else
				++j;
		}
		return len;
	}
	vector<int> logicaland(Doc& doc1)  {
		int len1 = ids.size();
		int len2 = doc1.ids.size();
		vector<int> result;
		int i = 0, j = 0;
		while (i < len1 && j < len2) {
			if (ids[i] == doc1.ids[j]) {
				result.push_back(ids[i]);
				++i; ++j;
			}
			else if (ids[i] < doc1.ids[j])
				++i;
			else
				++j;
		}
		return result;
	}
};


class TitleKNN {
public:
	TitleKNN(string model_dir_path, unsigned int k = 10);
	string predict(const vector<string>& title);
	void search(const set<int>& wordIDs, vector<int>& best_docs);
	void search(const vector<string>& title, vector<int>& best_docs);
	void title_to_wordIDs(const vector<string>& title, set<int>& wordIDs);
	void print_info();
	vector<string> split_by_ascii(const string& src, const string tok);
	int get_num_docs() {return docs.size();}
	int get_num_categories() {return cate_index.size();}
	int get_num_words() {return word_index.size();}
	
	void Map(const vector<vector<string> >& titles, vector<vector<pair<float,int> > >& neighb_dist);
	string Reduce(vector<pair<float,int> >& dist);
	string normalize(const string& str);
	
private:
	// index
	map<string,int> cate_index;
	vector<string> cates;
	boost::unordered_map<string,int> word_index;
	vector<vector<int> > inv_index;
	// vector<int> weight;
	
	// info of train data
	vector<Doc> docs;
	vector<int> cateID_of_doc;
	
	// global variable
	unsigned int neighbor_k;
	set<string> stopwords;
	vector<float> sqrts;
	unsigned int MAX_MAP_NUM;
	unsigned int MAX_SQRT_NUM;
	
private:
	bool load_bin_model(const string& bin_model_file);
	bool load_txt_model(const string& txt_model_file);
	bool read_raw_corpus(const string& raw_corpus_file);
	void read_stopwords(const string& stopwords_file);
	bool read_ranked_words(const string& ranked_words_file);
	void write_txt_model(const string& model_txt_file);
	void write_bin_model(const string& model_bin_file);
	
	void wordIDs_to_Doc(set<int> wordIDs, Doc& doc);
	float cosine_distance(Doc& doc1, Doc& doc2);
	bool is_single_char(const string& str);
	void merge_vector(const vector<int>& vect1, vector<int>& result);
};


TitleKNN::TitleKNN(string model_dir_path, unsigned int k):neighbor_k(k)
{
	MAX_SQRT_NUM = 20000;
	MAX_MAP_NUM = 2;
	
	sqrts.resize(MAX_SQRT_NUM, 0.0);
	for (unsigned int i = 0; i < MAX_SQRT_NUM; ++i)
		sqrts[i] = sqrt(i);
	
	if (model_dir_path.empty())
		model_dir_path = "./";
	else if (*(model_dir_path.rbegin()) != '/')
		model_dir_path += '/';
	
	string stopwords_file = model_dir_path + "stopwords.txt";
	read_stopwords(stopwords_file);
	
	string bin_model_file = model_dir_path + "knn_model.bin";
	string txt_model_file = model_dir_path + "knn_model.txt";
	string ranked_words_file = model_dir_path + "ranked_words.txt";
	string raw_corpus_file = model_dir_path + "tokenized_titles_with_categories.txt";
	
	if (load_bin_model(bin_model_file))
		return;
	else if (load_txt_model(txt_model_file))
		return;
	else if (read_ranked_words(ranked_words_file)) {
		if (!read_raw_corpus(raw_corpus_file)) {
			cout << "The file " << raw_corpus_file << " doesn't exist in " << model_dir_path << endl;
			exit(1);
		}
	} else {
		cout << "The file " << ranked_words_file << " doesn't exist in " << model_dir_path << endl;
		exit(1);
	}
}


void TitleKNN::title_to_wordIDs(const vector<string>& title, set<int>& wordIDs)
{
	wordIDs.clear();
	for (unsigned int i = 0; i < title.size(); ++i) {
		//if (is_single_char(title[i]))
			//continue;
		boost::unordered_map<string,int>::iterator ite = word_index.find(title[i]);
		
		if (ite != word_index.end())
			wordIDs.insert(ite->second);
	}
}


string TitleKNN::predict(const vector<string>& title)
{
	vector<vector<string> > titles(1, title);
	vector<vector<pair<float,int> > > neighb_dist;
	Map(titles, neighb_dist);
	
	// cout << "in predict: neighbours size: " << neighb_dist[0].size() << endl;
	
	return Reduce(neighb_dist[0]);
}


void TitleKNN::Map(const vector<vector<string> >& titles, vector<vector<pair<float,int> > >& neighb_dist)
{
	neighb_dist.clear();
	neighb_dist.resize(titles.size());
	
	for (unsigned int i = 0; i < titles.size(); ++i) {
		vector<string> title = titles[i];
		for (unsigned int k = 0; k < title.size(); ++k)
			title[k] = normalize(title[k]);
		
		set<int> wordIDs;
		title_to_wordIDs(title, wordIDs);
		vector<int> nearby_docs;
		search(wordIDs, nearby_docs);
		
		Doc doc;
		wordIDs_to_Doc(wordIDs, doc);
		for (unsigned int j = 0; j < nearby_docs.size(); ++j) {
			int id = nearby_docs[j];
			float dist = cosine_distance(doc, docs[id]);
			//if (dist > 0.3) {
				float value = dist * dist * dist;
				neighb_dist[i].push_back(make_pair(value,cateID_of_doc[id]));
			//}
		}
	}
}


string TitleKNN::Reduce(vector<pair<float,int> >& dist)
{
	if (dist.empty())
		return "";
	
	multimap<float,int,std::greater<float> > sorted_dist;
	for (vector<pair<float,int> >::iterator it = dist.begin(); it != dist.end(); ++it)
		sorted_dist.insert(*it);
	
	// erase some distance
	float last_dist = sorted_dist.begin()->first;
	multimap<float,int,std::greater<float> >::iterator ite = sorted_dist.begin();
	int top_k = 0;
	for (++ite; ite != sorted_dist.end(); ++ite) {
		if ( (last_dist - ite->first) / last_dist > 0.2)
			break;
		last_dist = ite->first;
		if (top_k++ > 30)
			break;
		
	}
	while (ite != sorted_dist.end()) {
		sorted_dist.erase(ite++);
	}
	
	// add up
	map<int,float> cateid_score;
	for (ite = sorted_dist.begin(); ite != sorted_dist.end(); ++ite) {
		map<int,float>::iterator iter = cateid_score.find(ite->second);
		if (iter != cateid_score.end())
			iter->second += ite->first;
		else
			cateid_score[ite->second] = ite->first;
	}
	
	// get best category
	float max_id = cateid_score.begin()->first;
	float max_score = cateid_score.begin()->second;
	for (map<int,float>::iterator iter = cateid_score.begin(); iter != cateid_score.end(); ++iter) {
		if (iter->second > max_score) {
			max_score = iter->second;
			max_id = iter->first;
		}
	}
	
	return cates[max_id];
}


void TitleKNN::search(const vector<string>& title, vector<int>& best_nearby_docs)
{
	set<int> wordIDs;
	title_to_wordIDs(title, wordIDs);
	search(wordIDs, best_nearby_docs);
}


void TitleKNN::search(const set<int>& wordIDs, vector<int>& best_nearby_docs)
{
	best_nearby_docs.clear();
	unsigned int n = 0;
	for (set<int>::iterator ite = wordIDs.begin(); ite != wordIDs.end(); ++ite) {
		if (++n > MAX_MAP_NUM)
			break;
		merge_vector(inv_index[*ite], best_nearby_docs);
	}
	
	//cout <<best_nearby_docs.size() << "\n";
}


float TitleKNN::cosine_distance(Doc& doc1, Doc& doc2)
{
	int len1 = doc1.size();
	int len2 = doc2.size();
	return doc1.logicaland_len(doc2) / sqrts[len1*len2];
}


void TitleKNN::wordIDs_to_Doc(set<int> wordIDs, Doc& doc)
{
	doc.ids.clear();
	doc.ids.resize(wordIDs.size());
	int i = 0;
	for (set<int>::iterator ite = wordIDs.begin(); ite != wordIDs.end(); ++ite)
		doc.ids[i++] = *ite;
}


vector<string> TitleKNN::split_by_ascii(const string& str, const string tok) {
	if (str.empty())
		return vector<string>();
	if (tok.empty())
		return vector<string>(1, str);
	
	vector<string> vect;
	string::size_type start_pos = 0;
	
	for(unsigned int i = 0; i < str.size(); ++i) {
		if (tok.find(str[i]) != string::npos ) {
			unsigned int len = i - start_pos;
			if (len > 0)
				vect.push_back(str.substr(start_pos,len));
			start_pos = i + 1;
		}
	}
  	
	if(start_pos < str.size())
		vect.push_back(str.substr(start_pos));
	
	return vect;
}


string TitleKNN::normalize(const string& str)
{
	boost::regex exp_rep("([0-9]+)");
	boost::regex exp_erase("[ \t,;.!?:<>@#$%_]+");
	string output = boost::regex_replace(str, exp_rep, "x");
	output = boost::regex_replace(str, exp_erase, "");
	
	return boost::to_lower_copy(output);
}


///////////////// read and write model  ///////////////////////

bool TitleKNN::load_bin_model(const string& bin_model_file)
{
	ifstream fin(bin_model_file.c_str(), std::ios::binary);
	if (!fin)
		return false;
	
	int num_cates = 0;
	int num_words = 0;
	int num_docs = 0;
	
	fin.read((char*)&num_cates, sizeof(int));
	fin.read((char*)&num_words, sizeof(int));
	fin.read((char*)&num_docs, sizeof(int));

	int size;
	int id;
	
	cates.resize(num_cates, "");
	// read categories
	for (int i = 0; i < num_cates; ++i) {
		char name[256] = {0};
		fin.read((char*)&size, sizeof(int));
		fin.read((char*)&name,size);
		fin.read((char*)&id, sizeof(int));
		name[size] = 0;
		cate_index.insert(make_pair(name,id));
		cates[id] = name;
	}
	
	// read words
	for (int i = 0; i < num_words; ++i) {
		char name[256] = {0};
		fin.read((char*)&size, sizeof(int));
		fin.read((char*)&name,size);
		fin.read((char*)&id, sizeof(int));
		
		word_index.insert(make_pair(name,id));
	}
	
	// read docs, build the inderted index
	cateID_of_doc.resize(num_docs, 0);
	inv_index.resize(num_words,vector<int>());
	docs.resize(num_docs);
	for (int i = 0; i < num_docs; ++i) {
		fin.read((char*)&size, sizeof(int));
		fin.read((char*)&id, sizeof(int));
		cateID_of_doc[i] = id; // category id
		
		set<int> wordIDs; // word ids of doc
		for (int j = 0; j < size-1; ++j) {
			fin.read((char*)&id, sizeof(int));
			wordIDs.insert(id);
		}
		
		wordIDs_to_Doc(wordIDs, docs[i]);
		
		// build inverted index
		for (set<int>::iterator ite = wordIDs.begin(); ite != wordIDs.end(); ++ite) {
			inv_index[*ite].push_back(i);
		}
	}
	
	return true;
}


bool TitleKNN::load_txt_model(const string& txt_model_file)
{
	ifstream fin(txt_model_file.c_str());
	if (!fin)
		return false;
	
	int num_cates = 0;
	int num_words = 0;
	int num_docs = 0;
	
	string line;
	getline(fin, line);
	istringstream iss(line);
	iss >> num_cates;
	iss >> num_words;
	iss >> num_docs;
	
	// read categories
	cates.resize(num_cates, "");
	for (int i = 0; i < num_cates; ++i) {
		getline(fin,line);
		string::size_type split_pos = line.find_first_of("\t");
		string cate_name = line.substr(0, split_pos);
		int id = boost::lexical_cast<int>(line.substr(split_pos+1));
		
		cate_index.insert(make_pair(cate_name,id));
		cates[id] = cate_name;
	}
	
	// read word
	for (int i = 0; i < num_words; ++i) {
		getline(fin,line);
		string::size_type split_pos = line.find_first_of("\t");
		string word = line.substr(0, split_pos);
		int id = boost::lexical_cast<int>(line.substr(split_pos+1));
		word_index.insert(make_pair(word,id));
	}
	
	// read docs, and build the inderted index
	cateID_of_doc.resize(num_docs, 0);
	inv_index.resize(num_words,vector<int>());
	docs.resize(num_docs);
	for (int i = 0; i < num_docs; ++i) {
		getline(fin,line);
		istringstream iss(line);
		int id;
		iss >> id;
		cateID_of_doc[i] = id;
		
		set<int> wordIDs;
		while(iss >> id)
			wordIDs.insert(id);
		
		wordIDs_to_Doc(wordIDs, docs[i]);
		
		// build inverted index
		for (set<int>::iterator ite = wordIDs.begin(); ite != wordIDs.end(); ++ite) {
			inv_index[*ite].push_back(i);
		}
	}
	
	// write binary model
	string model_dir;
	string::size_type pos = txt_model_file.find_last_of("/");
	if (pos != string::npos)
		model_dir = txt_model_file.substr(0, pos+1);
	write_bin_model(model_dir + "/knn_model.bin");
	return true;
}


bool TitleKNN::read_ranked_words(const string& ranked_words_file)
{
	ifstream fin(ranked_words_file.c_str());
	if (!fin)
		return false;
	
	word_index.clear();
	
	int rank = 0;
	string line;
	while (getline(fin, line)) {
		if (line.empty())
			continue;
		string::size_type pos = line.find_last_of("\t");
		string word = line.substr(pos+1);
		word_index.insert(make_pair(word,rank));
		++rank;
	}
	
	return true;
}


bool TitleKNN::read_raw_corpus(const string& raw_corpus_file)
{
	ifstream fin(raw_corpus_file.c_str());
	if (!fin)
		return false;
	
	// get cate_index, num_docs
	int num_docs = 0;
	string line;
	while (getline(fin,line)) {
		if (line.empty()) continue;
		string::size_type pos = line.find_last_of("\t");
		if (pos == string::npos) {
			cout << "in " << raw_corpus_file << ", wrong corpus in : \"" << line << "\""<< endl;
			exit(1);
		}
		string category = line.substr(pos+1);
		int cate_id = cate_index.size();
		cate_index.insert(make_pair(category,cate_id));
		++num_docs;
	}
	fin.close();
	
	// get cates
	cates.resize(cate_index.size());
	for (map<string,int>::iterator ite = cate_index.begin(); ite != cate_index.end(); ++ite)
		cates[ite->second] = ite->first;
	
	// read docs, and built the inverse index
	docs.resize(num_docs);
	cateID_of_doc.resize(num_docs);
	inv_index.resize(word_index.size(), vector<int>());
	int doc_id = 0;
	fin.open(raw_corpus_file.c_str());
	while (getline(fin, line)) {
		if (line.empty())
			continue;
		vector<string> terms = split_by_ascii(line, "\t");
		cateID_of_doc[doc_id] = cate_index[terms.back()];
		
		set<int> wordIDs;
		for (unsigned int i = 0; i < terms.size() - 1; ++i) {
			boost::unordered_map<string,int>::iterator ite = word_index.find(terms[i]);
			if (ite != word_index.end())
				wordIDs.insert(ite->second);
		}
		wordIDs_to_Doc(wordIDs, docs[doc_id]);
		
		// build inverted index
		for (set<int>::iterator ite = wordIDs.begin(); ite != wordIDs.end(); ++ite) {
			inv_index[*ite].push_back(doc_id);
		}
		++doc_id;
	}
	
	// write text model and binary model
	string model_dir;
	string::size_type pos = raw_corpus_file.find_last_of("/");
	if (pos != string::npos)
		model_dir = raw_corpus_file.substr(0, pos+1);
	write_txt_model(model_dir + "knn_model.txt");
	write_bin_model(model_dir + "knn_model.bin");
	
	return true;
}


void TitleKNN::write_txt_model(const string& model_txt_file)
{
	ofstream fout(model_txt_file.c_str());
	fout << cate_index.size() << "\t" << word_index.size() << "\t" << docs.size() << "\n";
	
	//write categoreis
	for (map<string,int>::iterator ite = cate_index.begin(); ite != cate_index.end(); ++ite) {
		fout << ite->first << "\t" << ite->second << "\n";
	}
	
	// write words
	for (boost::unordered_map<string,int>::iterator ite = word_index.begin(); ite != word_index.end(); ++ite)
		fout << ite->first << "\t" << ite->second << "\n"; 
	
	// write docs
	for (unsigned int i = 0; i < docs.size(); ++i) {
		fout << cateID_of_doc[i];
		for (vector<int>::const_iterator it = docs[i].ids.begin(); it != docs[i].ids.end(); ++it)
			fout << "\t" << *it;
		fout << "\n";
	}
	fout.close();
}


void TitleKNN::write_bin_model(const string& model_bin_file)
{
	ofstream fout(model_bin_file.c_str(), std::ios::binary);
	
	int num_cates = cates.size();
	int num_words = word_index.size();
	int num_docs = docs.size();
	
	fout.write((char*)&num_cates, sizeof(int));
	fout.write((char*)&num_words, sizeof(int));
	fout.write((char*)&num_docs, sizeof(int));
	
	// write categories
	for (map<string,int>::iterator ite = cate_index.begin(); ite != cate_index.end(); ++ite) {
		int size_str = (ite->first).size();
		fout.write((char*)&size_str, sizeof(int));
		fout.write((ite->first).c_str(), size_str);
		fout.write((char*)&(ite->second), sizeof(int));
	}
	
	// write words
	for (boost::unordered_map<string,int>::iterator ite = word_index.begin(); ite != word_index.end(); ++ite) {
		int size_str = (ite->first).size();
		fout.write((char*)&size_str, sizeof(int));
		fout.write((ite->first).c_str(), size_str);
		fout.write((char*)&(ite->second), sizeof(int));
	}
	
	// write docs
	for (int i = 0; i < num_docs; ++i) {
		int size = docs[i].size() + 1;
		fout.write((char*)&size, sizeof(int));
		fout.write((char*)&cateID_of_doc[i], sizeof(int));
		
		Doc& doc = docs[i];
		for (vector<int>::const_iterator it = doc.ids.begin(); it != doc.ids.end(); ++it) {
			int word_id = *it;
			fout.write((char*)&word_id, sizeof(int));
		}
	}
}


void TitleKNN::read_stopwords(const string& stopwords_file)
{
	ifstream fin(stopwords_file.c_str());
	if (!fin) {
		// cout << "in TitleKNN::read_stopwords(const string&), failed to open the file: " << stopwords_file << endl;
		return;
	}
	
	stopwords.clear();
	string line;
	while (getline(fin,line)) {
		if (line.empty())
			continue;
		stopwords.insert(line);
	}
	fin.close();
}


void TitleKNN::print_info()
{
	cout << "knn model info:\n";
	cout << "  categories:  " << cate_index.size() << "\n";
	cout << "  words:  " << word_index.size() << "\n";
	cout << "  docs:  " << cateID_of_doc.size() << endl;
}


bool TitleKNN::is_single_char(const string& str)
{
	int size = str.size();
	char ch = str[0];
	if (size == 1) {
		return true;
	} else if (size == 2) {
		if ((ch & 0xC0) == 0xC0)
			return true;
	} else if (size == 3) {
		if ((ch & 0xE0) == 0xE0)
			return true;
	} else if (size == 4) {
		if ((ch & 0xF0) == 0xF0)
			return true;
	}
	
	return false;
}


void TitleKNN::merge_vector(const vector<int>& vect1, vector<int>& result)
{
	if (vect1.empty()) {
		return;
	} else if (result.empty()) {
		result = vect1;
		return;
	}
	
	vector<int> vect2;
	vect2.swap(result);
	
	int len1 = vect1.size();
	int len2 = vect2.size();
	int i = 0, j = 0;
	
	while (i < len1 && j < len2) {
		if (vect1[i] < vect2[j]) {
			result.push_back(vect1[i]);
			++i;
		} else if (vect1[i] > vect2[j])  {
			result.push_back(vect2[j]);
			++j;
		} else {
			result.push_back(vect1[i]);
			++i; ++j;
		}
	}
	
	if (i < len1)
		for (; i < len1; ++i)
			result.push_back(vect1[i]);
	else
		for (; j < len2; ++j)
			result.push_back(vect2[j]);
}

#endif // #ifndef TITLEKNN_H
