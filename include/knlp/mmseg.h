#ifndef _ILPLIB_KNLP_MMSEG_H_
#define _ILPLIB_KNLP_MMSEG_H_


#include "knlp/mmdict.h"
#include <math.h>
#include <boost/lexical_cast.hpp>
#include "knlp/normalize.h"


namespace ilplib
{
namespace knlp
{

class MMseg {
private:
	ilplib::knlp::MMDict<float> dict_;
	map<std::string,int> freedoms_;
public:
	MMseg(const std::string& dict_path) :dict_(dict_path + "/token.dict")
	{
cout << "test 00 in consturer" << endl;
		load_freedegree(dict_path + "/freedegree.txt");
cout << "test 01 in consturer" << endl;
	}
	
	void seg(std::string str, std::vector<std::string>& words);
	void seg_with_attr(std::string str, std::vector<std::pair<std::string,float> >& words_attrs);
	void seg_all(std::string str, std::vector<std::string>& words);
	void seg_all_with_attr(std::string str, std::vector<std::pair<std::string,float> >& words_attrs);
private:
	bool is_legal_utf8_str(const std::string& str);
	int get_chars_size(const std::string& str);
	void disintegrate_utf8_str(const std::string& str, std::vector<int>& split_pos, std::vector<std::string>& single_chars);
	std::string get_next_word(const std::string& str, const unsigned int& ch_index, const std::vector<int>& split_pos, const std::vector<std::string>& single_chars);
	
	std::vector<std::string> recognize_letter_digit(const std::string& str);
	void merge_letter_digit(std::vector<std::string>& words);
private:
	bool load_freedegree(const std::string& freedegree_file);
	void get_matched_prefix_lens(const std::string& str, const int& index, std::vector<int>& prefix_lens);
	void get_chunks(const std::string& str, const int& ch_index, const std::vector<int>& split_pos, 
	                std::vector<std::vector<std::pair<int,int> > >& chunks);
	void filterChunks(std::vector<std::vector<std::pair<int,int> > >& chunks, const std::vector<std::string>& single_chars);
	void rule_1st(std::vector<std::vector<std::pair<int,int> > >& chunks);
	void rule_2nd(std::vector<std::vector<std::pair<int,int> > >& chunks);
	void rule_3rd(std::vector<std::vector<std::pair<int,int> > >& chunks);
	void rule_4th(std::vector<std::vector<std::pair<int,int> > >& chunks, const std::vector<std::string>& single_chars);
	
	bool is_digit(char c);
	bool is_complete_digit(const std::string& word);
	bool is_complete_alpha(const std::string& word);
	bool is_complete_alnum(const std::string& word);
};


bool MMseg::load_freedegree(const std::string& freedegree_file)
{
	ifstream fin(freedegree_file.c_str());
	if (!fin) {
		std::cout << "in void MMseg::load_freedegree(const std::string&), failed to open " << freedegree_file << std::endl;
		return false;
	}
	
	std::string line;
	while (getline(fin,line)) {
		if (line.empty())
			continue;
		std::string::size_type pos = line.find("\t");
		std::string word = line.substr(0,pos);
		float free = boost::lexical_cast<float>(line.substr(pos+1));
		freedoms_.insert(std::make_pair(word, free));
	}
	return true;
}


void MMseg::seg(std::string str, std::vector<std::string>& words)
{
	words.clear();
	
	if (is_legal_utf8_str(str) == false) {
		throw  std::runtime_error("illegal utf8 std::string: " + str);
		return;
	}
	
	std::vector<int> lens;
	std::vector<int> split_pos;
	std::vector<std::string> single_chars;
	
	try {
		Normalize::normalize(str);
	} catch (runtime_error re) {
		re.what();
		std::cout << "in normalize(string&), bad string: " << str << std::endl;
	}
	
	disintegrate_utf8_str(str, split_pos, single_chars);
	
	unsigned int ch_index = 0;
	map<int,std::vector<int> > prefix_pool;

	while (ch_index < single_chars.size()) {
		std::string word = get_next_word(str, ch_index, split_pos, single_chars);
		
		if (!word.empty()) {
			words.push_back(word);
			ch_index += get_chars_size(word);
		}
		else
			break;
	}
	
	merge_letter_digit(words);
}


void MMseg::seg_with_attr(std::string str, std::vector<std::pair<std::string,float> >& words_attrs)
{
	std::vector<std::string> words;
	seg(str, words);
	
	words_attrs.clear();
	words.resize(words.size());
	for (unsigned int i = 0; i < words.size(); ++i) {
		float v = 0;
		int res = dict_.value(words[i], v, false);
		if (res != 0)
			words_attrs.push_back(std::make_pair(words[i], 0));
		else
			words_attrs.push_back(std::make_pair(words[i], v));
	}
}


void MMseg::seg_all(std::string str, std::vector<std::string>& words)
{
	words.clear();
	
	if (is_legal_utf8_str(str) == false) {
		throw  std::runtime_error("illegal utf8 std::string: " + str);
		return;
	}
	
	std::vector<int> lens;
	std::vector<int> split_pos;
	std::vector<std::string> single_chars;
	
	Normalize::normalize(str);
	disintegrate_utf8_str(str, split_pos, single_chars);
	
	for (unsigned int i = 0; i < single_chars.size(); ++i) {
		int index = split_pos[i];
		std::vector<std::string> prefixs;
		dict_.get_matched_prefix(str, index, prefixs);
		for (unsigned int j = 0; j < prefixs.size(); ++j)
			words.push_back(prefixs[j]);
	}
	
	std::vector<std::string> digit_letter_words = recognize_letter_digit(str);
	for (unsigned int k = 0; k < digit_letter_words.size(); ++k) {
		if (!dict_.has_key(digit_letter_words[k], false) )
			words.push_back(digit_letter_words[k]);
	}
}
	
	
void MMseg::seg_all_with_attr(std::string str, std::vector<std::pair<std::string,float> >& words_attrs)
{
	words_attrs.clear();
	
	if (is_legal_utf8_str(str) == false) {
		throw  std::runtime_error("illegal utf8 std::string: " + str);
		return;
	}
	
	std::vector<int> lens;
	std::vector<int> split_pos;
	std::vector<std::string> single_chars;
	
	Normalize::normalize(str);
	disintegrate_utf8_str(str, split_pos, single_chars);
	
	for (unsigned int i = 0; i < single_chars.size(); ++i) {
		int index = split_pos[i];
		std::vector<std::string> prefixs;
		dict_.get_matched_prefix(str, index, prefixs);
		
		for (unsigned int j = 0; j < prefixs.size(); ++j) {
			float v = 0;
			int res = dict_.value(prefixs[j], v, false);
			if (res != 0)
				words_attrs.push_back(std::make_pair(prefixs[j], 0));
			else
				words_attrs.push_back(std::make_pair(prefixs[j], v));
		}
	}
	
	std::vector<std::string> digit_letter_words = recognize_letter_digit(str);
	for (unsigned int k = 0; k < digit_letter_words.size(); ++k) {
		if (!dict_.has_key(digit_letter_words[k], false) )
			words_attrs.push_back(std::make_pair(digit_letter_words[k],0));
	}
}


std::string MMseg::get_next_word(const std::string& str, const unsigned int& ch_index, const std::vector<int>& split_pos, const std::vector<std::string>& single_chars)
{
	std::vector<std::vector<std::pair<int,int> > > chunks;
	get_chunks(str, ch_index, split_pos, chunks);

	filterChunks(chunks, single_chars);
	
	std::pair<int,int> pos = chunks[0][0];
	
	std::string word = str.substr(split_pos[pos.first], split_pos[pos.first + pos.second] - split_pos[pos.first]);
	
	return word;
}


void MMseg::get_chunks(const std::string& str, const int& ch_index, const std::vector<int>& split_pos, 
                       std::vector<std::vector<std::pair<int,int> > >& chunks)
{
	chunks.clear();
	
	const unsigned int& index = split_pos[ch_index];
	
	if (str.size() == index)
		return ;
	
	std::vector<int> layer_1;
	std::vector<int> layer_2;
	std::vector<int> layer_3;
	
	get_matched_prefix_lens(str, index, layer_1);
	
	if (layer_1.empty())
		layer_1.push_back(1);
	if (layer_1.size() == 1) {
		chunks.push_back( std::vector<std::pair<int,int> >(1,std::make_pair(ch_index, layer_1.front())) );
		return;
	}
	
	for (unsigned int x = 0; x < layer_1.size(); ++x) {
		unsigned int index_2 = split_pos[ch_index + layer_1[x]];
		
		if (index_2 == str.size()) {
			std::vector<std::pair<int,int> > chunk(1, std::make_pair(ch_index, layer_1[x]));
			chunks.push_back(chunk);
			break;
		}
		
		get_matched_prefix_lens(str, index_2, layer_2);
		if (layer_2.empty())
			layer_2.push_back(1);
		
		for (unsigned int y = 0; y < layer_2.size(); ++y) {
			unsigned int index_3 = split_pos[ch_index + layer_1[x] + layer_2[y]];
			if (index_3 == str.size()) {
				std::vector<std::pair<int,int> > chunk(2);
				chunk[0] = std::make_pair(ch_index, layer_1[x]);
				chunk[1] = std::make_pair(ch_index + layer_1[x], layer_2[y]);
				chunks.push_back(chunk);
				break;
			}
			
			get_matched_prefix_lens(str, index_3, layer_3);
			if (layer_3.empty())
				layer_3.push_back(1);
			
			for (unsigned int z = 0; z < layer_3.size(); ++z) {
				std::vector<std::pair<int,int> > chunk(3);
				chunk[0] = std::make_pair(ch_index, layer_1[x]);
				chunk[1] = std::make_pair(ch_index + layer_1[x], layer_2[y]);
				chunk[2] = std::make_pair(ch_index + layer_1[x] + layer_2[y], layer_3[z]);
				chunks.push_back(chunk);
			}
		}
		
	}


}


void MMseg::filterChunks(std::vector<std::vector<std::pair<int,int> > >& chunks, const std::vector<std::string>& single_chars)
{
	rule_1st(chunks);
	if (chunks.size() == 1)
		return;
	
	rule_2nd(chunks);
	if (chunks.size() == 1)
		return;
	

	rule_3rd(chunks);
	if (chunks.size() == 1)
		return;

	rule_4th(chunks, single_chars);
}


// max total lens
void MMseg::rule_1st(std::vector<std::vector<std::pair<int,int> > >& chunks)
{
	if (chunks.size() == 1)
		return;
	
	int max_len = 0;
	std::vector<int> total_lens(chunks.size(), 0);
	for (unsigned int i = 0; i < chunks.size(); ++i) {
		total_lens[i] = chunks[i].back().first + chunks[i].back().second - chunks[i].front().first;
		max_len = (max_len < total_lens[i]) ? total_lens[i] : max_len;
	}
	
	for (int j = chunks.size() -1; j >= 0; --j) {
		if (total_lens[j] < max_len)
			chunks.erase(chunks.begin() + j);
	}
}


void MMseg::rule_2nd(std::vector<std::vector<std::pair<int,int> > >& chunks)
{
	if (chunks.size() == 1)
		return;
	
	unsigned int min_num_words = 100;
	for (unsigned int i = 0; i < chunks.size(); ++i) {
		unsigned int size = chunks[i].size();
		min_num_words = min_num_words <= size ? min_num_words : size;
	}
	
	for (int i = chunks.size() -1; i >= 0; --i) {
		if (chunks[i].size() > min_num_words)
			chunks.erase(chunks.begin() + i);
	}
	
}


void MMseg::rule_3rd(std::vector<std::vector<std::pair<int,int> > >& chunks)
{
	if (chunks.size() == 1)
		return;
	/*
	float average_lens(chunks.size(), 0);
	for (int i = 0; i < chunks.size(); ++i) {
		int total_len = 0;
		for (int j = 0; j < chunks[i].size(); ++j)
			total_len += chunks[i][j].second;
		average_lens[i] = 1.0 * total_len / chunks[i].size();
	}
	*/
	
	float min_variance = 999999;
	std::vector<float> variance(chunks.size(), 0);
	for (unsigned int i = 0; i < chunks.size(); ++i) {
		for (unsigned int j = 0; j < chunks[i].size(); ++j) {
			// variance[i] += (chunks[i][j] - average-lens[i]) * (chunks[i][j] - average-lens[i]);
			variance[i] += chunks[i][j].second * chunks[i][j].second;
		}
		min_variance = (min_variance < variance[i]) ? min_variance : variance[i];
	}
	
	for (int i = chunks.size() -1; i >= 0; --i) {
		if (variance[i] > min_variance)
			chunks.erase(chunks.begin() + i);
	}
}


void MMseg::rule_4th(std::vector<std::vector<std::pair<int,int> > >& chunks, const std::vector<std::string>& single_chars)
{
	if (chunks.size() == 1)
		return;
	
	std::vector<float> ch_freedoms(chunks.size(), 0);
	
	for (unsigned int i = 0; i < chunks.size(); ++i) {
		float sum_freedom = 0;
		for (unsigned int j = 0; j < chunks[i].size(); ++j) {
			if (chunks[i][j].second == 1) {
				std::string ch = single_chars[chunks[i][j].first];
				map<std::string,int>::iterator ite = freedoms_.find(ch);
				if (ite != freedoms_.end()) {
					sum_freedom += log(ite->second);
				}
			}
		}
		ch_freedoms[i] = sum_freedom;
	}
	
	int max_freedom = 0;
	for (unsigned int i = 0; i < ch_freedoms.size(); ++i) {
		if (max_freedom < ch_freedoms[i])
			max_freedom = ch_freedoms[i];
	}
	
	for (int i = chunks.size() -1; i >= 0; --i) {
		if (ch_freedoms[i] < max_freedom)
			chunks.erase(chunks.begin() + i);
	}
}


std::vector<std::string> MMseg::recognize_letter_digit(const std::string& str)
{
	std::vector<int> flags(str.size(), 0);
	for (unsigned i = 0; i < str.size(); ++i) {
		if (isalnum(str[i])) {
			flags[i] = 1;
		} else if (str[i] == ',' || str[i] == '.' || str[i] == '/') {
			if (i > 0 && i < str.size()-1) {
				if (isdigit(str[i-1]) && isdigit(str[i+1]))
					flags[i] = 1;
			}
		} else if (str[i] == '%') {
			if (i > 0) {
				if (isdigit(str[i-1]))
					flags[i] = 1;
			}
		} else if (str[i] == '-' || str[i] == '_' || str[i] == '*' ||
		           str[i] == '&' || str[i] == '+')
			flags[i] = 1;
	}
	
	std::vector<std::string> words;
	std::string word;
	for (unsigned int i = 0; i < str.size(); ++i) {
		if (flags[i] == 1)
			word += str[i];
		else {
			if (!word.empty()) {
				words.push_back(word);
				word.clear();
			}
		}
	}
	if (!word.empty())
		words.push_back(word);
	return words;
}


void MMseg::merge_letter_digit(std::vector<std::string>& words)
{
	std::vector<int> flags(words.size(), 0);
	for (unsigned i = 0; i < words.size(); ++i) {
		if (is_complete_alnum(words[i]) && words[i].size() < 4) {
			flags[i] = 1;
		} else if (is_complete_digit(words[i])) {
			flags[i] = 2;
		} else if (is_complete_alnum(words[i])) {
			flags[i] = 3;
		} else if (words[i].size()== 1 && (words[i][0] == ',' || words[i][0] == '.'  || words[i][0] == '/')) {
			// for recognize 12,345 3.1415  3/4 
			if (i > 0 && i < words.size() -1) {
				if (isdigit(*words[i-1].rbegin()) && isdigit(*words[i+1].begin()))
					flags[i] = 2;
			}
		} else if (words[i].size()== 1 && words[i][0] == '%') {
			// for recognize, eg. 10.9%
			if (i > 0) {
				if (isdigit(*words[i-1].rbegin()))
					flags[i] = 2;
			}
		} else if (words[i].size() == 1 && (words[i][0] == '&' || words[i][0] == '\'')) {
			if (i > 0 && i < words.size() -1) {
				if (isalpha(*words[i-1].rbegin()) && isalpha(*words[i+1].rbegin()))
					flags[i] = 1;
			}
		} else if (words[i].size() == 1 && 
		         (words[i][0] == '*' || 
		          words[i][0] == '-' || 
		          words[i][0] == '_' || 
		          words[i][0] == '+') )
		        flags[i] = 4;
	}
	
	std::vector<std::string> temp_words;
	temp_words.swap(words);
	std::string word;
	for (unsigned int i = 0; i < temp_words.size(); ++i) {
		if (flags[i] == 0) {
			if (!word.empty()) {
				words.push_back(word);
				word.clear();
			}
			words.push_back(temp_words[i]);
		} else if (flags[i] != 0) {
			word += temp_words[i];
		}
	}
	if (!word.empty())
		words.push_back(word);
}


bool MMseg::is_complete_digit(const std::string& word) {
	const char* p = word.c_str();
	while (*p != 0) {
		if (!isdigit(*p))
			return false;
		++p;
	}
	return true;
}


bool MMseg::is_complete_alpha(const std::string& word) {
	const char* p = word.c_str();
	while (*p != 0) {
		if (!isalpha(*p))
			return false;
		++p;
	}
	return true;
}


bool MMseg::is_complete_alnum(const std::string& word) {
	const char* p = word.c_str();
	while (*p != 0) {
		if (!isalnum(*p))
			return false;
		++p;
	}
	return true;
}


////////////////////////////////////////////////

void MMseg::get_matched_prefix_lens(const std::string& str, const int& index, std::vector<int>& prefix_lens)
{
	prefix_lens.clear();
	
	std::vector<std::string> prefixs;
	dict_.get_matched_prefix(str, index, prefixs);
	
	prefix_lens.resize(prefixs.size());
	for (unsigned int i = 0; i < prefixs.size(); ++i) {
		prefix_lens[i] = get_chars_size(prefixs[i]);
	}
}



bool MMseg::is_digit(char c)
{
	if (c >= '0' && c<='9')
		return true;
	if (c == '.' || c == '-' || c == '+' || c == '/' || c == '=' || c== '*' || c== '%'
          || c == ',' || c == '$' || c == '&' || c == '_')
		return true;
	return false;
}


void MMseg::disintegrate_utf8_str(const std::string& str, std::vector<int>& split_pos, std::vector<std::string>& single_chars)
{
	split_pos.clear();
	single_chars.clear();
	
	int index = 0;
	int char_len = 0;
	const unsigned char* h = (unsigned char*)str.c_str();
	while (*h != char(0)) {
		if (*h >> 7 == 0x00)
			char_len = 1;
		else if (*h >> 5 == 0x06)
			char_len = 2;
		else if (*h >> 4 == 0x0E)
			char_len = 3;
		else if (*h >> 3 == 0x1E)
			char_len = 4;
		else if (*h >> 2 == 0x3E)
			char_len = 5;
		else if (*h >> 1 == 0x7E)
			char_len = 6;
		
		h += char_len;
		single_chars.push_back(str.substr(index, char_len));
		split_pos.push_back(index);
		index += char_len;
	}
	split_pos.push_back(str.size());
}


int MMseg::get_chars_size(const std::string& str)
{
	int char_num = 0;
	unsigned int tail_len = 0;
	
	const unsigned char* h = (unsigned char*)str.c_str();
	while (*h != char(0)) {

		if (*h >> 7 == 0x00)
			tail_len = 0;
		else if (*h >> 5 == 0x06)
			tail_len = 1;
		else if (*h >> 4 == 0x0E)
			tail_len = 2;
		else if (*h >> 3 == 0x1E)
			tail_len = 3;
		else if (*h >> 2 == 0x3E)
			tail_len = 4;
		else if (*h >> 1 == 0x7E)
			tail_len = 5;
		else
			return -1;
		
		for (unsigned i = 0; i < tail_len; ++i) {
			++h;
			if (*h >> 6 != 2)
				return -1;
		}
		++h;
		++char_num;
	}
	return char_num;
}


bool MMseg::is_legal_utf8_str(const std::string& str)
{
	unsigned int tail_len = 0;
	const unsigned char* h = (unsigned char*)str.c_str();
	while (*h != char(0)) {
		if (*h >> 7 == 0x00)
			tail_len = 0;
		else if (*h >> 5 == 0x06)
			tail_len = 1;
		else if (*h >> 4 == 0x0E)
			tail_len = 2;
		else if (*h >> 3 == 0x1E)
			tail_len = 3;
		else if (*h >> 2 == 0x3E)
			tail_len = 4;
		else if (*h >> 1 == 0x7E)
			tail_len = 5;
		else
			return false;
		
		for (unsigned int i = 0; i < tail_len; ++i) {
			++h;
			if (*h >> 6 != 2)
				return false;
		}
		++h;
	}
	return true;
}



} //namespace
} //namespace

#endif

