#include "knlp/mmseg.h"
#include <iostream>


int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cout << "usage: ./mmseg_util dict_dir" << std::endl;
		return 1;
	}
	

	ilplib::knlp::MMseg mmseg(argv[1]);
	
	std::string str; 
	std::vector<string> words;
	
	
	str = "触控笔记本电脑";
	try {
		mmseg.seg(str,words);
	} catch (std::runtime_error re) {
		std::cout << re.what() << std::endl;
	}
	for (unsigned int i = 0; i < words.size(); ++i)
		std::cout << words[i] << "\t";
	std::cout << "\n------------------\n" << std::endl;
	
	
	str = "触控笔记本电脑";
	try {
		mmseg.seg_all(str,words);
	} catch (std::runtime_error re) {
		std::cout << re.what() << std::endl;
	}
	for (unsigned int i = 0; i < words.size(); ++i)
		std::cout << words[i] << "\t";
	std::cout << "\n------------------\n" << std::endl;
	
	
	std::vector<pair<string,float> > words_attrs;
	
	str = "触控笔记本电脑";
	try {
		mmseg.seg_with_attr(str,words_attrs);
	} catch (std::runtime_error re) {
		std::cout << re.what() << std::endl;
	}
	for (unsigned int i = 0; i < words_attrs.size(); ++i)
		std::cout << words_attrs[i].first << "\t" << words_attrs[i].second << "\n";
	std::cout << "------------------\n" << endl;
	
	
	str = "触控笔记本电脑";
	try {
		mmseg.seg_all_with_attr(str,words_attrs);
	} catch (std::runtime_error re) {
		std::cout << re.what() << std::endl;
	}
	for (unsigned int i = 0; i < words_attrs.size(); ++i)
		std::cout << words_attrs[i].first << "\t" << words_attrs[i].second << "\n";
	std::cout << "------------------\n" << std::endl;
	
}


// g++ mmseg_util.cpp ~/codebase/ilplib/source/knlp/normalize.cpp -o mmseg_util -lboost_system



