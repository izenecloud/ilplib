/*
 * Preprocesser.cpp
 *
 *  Created on: 2009-6-17
 *      Author: zhjay
 */
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/serialization/map.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <LA.h>
#include <wrapper/TermVector.h>

using namespace std;
using namespace boost::filesystem;
using namespace la;

LAConfiguration laconfig;
LA la;

TermVector gTermFreq;
TermVector gDocFreq;


/*
int gcount = 0;

bool process_file(string file_path, ostream& out){

	if(gcount < 1100000){
		gcount ++;
		return false;
	}

	path full_path( file_path );
	if ( !exists( full_path ) )
	{
		cout << "Not-exist error: " << file_path << endl;
		return false;
	}
	if (  !is_regular_file( full_path ) )
	{
		cout << "Non-file error: " << file_path << endl;
		return false;
	}

	ifstream ifs(file_path.c_str());

	TermVector termVector;


	const int buff_size = 200000;
	char buff[buff_size];
	string text;
	while(!ifs.eof()){
		ifs.getline(buff, buff_size);
		string line(buff);
		text.append(line + "\n");
	}
	ifs.close();

	UString ustring(text,UString::UTF_8);
	ustring.toLowerString();
	TermList LATerms;
	la.process(ustring, LATerms);

	TermList::iterator itList = LATerms.begin();
	for(; itList != LATerms.end() ; itList ++)
		termVector.add(itList->textString());

	map<string, int>::const_iterator it = termVector.termFreqMap.begin();
	out << "====doc:"<< gcount <<"===="<< endl;
	for(; it != termVector.termFreqMap.end(); ++ it){
//		gTermFreq.add(it->first, it->second);
//		gDocFreq.add(it->first);
		out << it->first << "\t" << it->second << endl;
	}

	gcount ++;

	return true;
}
bool process_file_stat(string file_path){

	path full_path( file_path );
	if ( !exists( full_path ) )
	{
		cout << "Not-exist error: " << file_path << endl;
		return false;
	}
	if (  !is_regular_file( full_path ) )
	{
		cout << "Non-file error: " << file_path << endl;
		return false;
	}
	ifstream ifs(file_path.c_str());
	const int buff_size = 2000;
	char buff[buff_size];

	int cline = 0;
	string ll;
	while(!ifs.eof()){
		ifs >> ll;
		cline ++;
	}
	cout << "#line=" << cline;
	boost::progress_display p_display(cline);
	ifs.close();
	ifs.open(file_path.c_str());
	while(!ifs.eof()){
		ifs.getline(buff, buff_size);
		string line(buff);
		if(line.length() == 0 || line.substr(0,8).compare("====doc:") == 0)
			continue;
		int pos = line.find("\t");
		if(pos <= 0)
			continue;
		string term = line.substr(0,pos);
		int freq = atoi(line.substr(pos + 1).c_str());
		gTermFreq.add(term, freq);
		gDocFreq.add(term);
		++ p_display;
	}
	ifs.close();
	return true;
}

int process_fold_stat( string fold ,string tar_fold)
{

	path full_path( fold ); // absolute directory

	if ( !exists( full_path ) )
	{
		cout << "\nNot found: " << full_path.native_file_string() << endl;
		return 1;
	}

	if ( is_directory( full_path ) )
	{
//		boost::progress_display p_display(21);

		directory_iterator end_iter;
		directory_iterator dir_itr( full_path );

//		int count = 0;

		for ( ; dir_itr != end_iter; ++dir_itr )
		{
			try
			{
				if ( is_regular_file( dir_itr->status() ) )
				{
					cout << fold + "/" + dir_itr->filename() << endl;
					if( process_file_stat(fold + "/" + dir_itr->filename()) ){
//						++count;
//						++p_display;
//						if(count % 500 == 0)
//							cout << gcount << endl;
					}


				}
			}
			catch ( const std::exception & ex )
			{
				cout << dir_itr->filename() << " " << ex.what() << std::endl;
			}
		}

		string target = tar_fold + "/tf.txt";
		ofstream ofs(target.c_str());
		ofs << gTermFreq ;
		ofs.close();

		target = tar_fold + "/df.txt";
		ofs.open(target.c_str());
		ofs << gDocFreq ;
		ofs.close();


	}
	else // must be a file
	{
		cout << "\nFound: " << full_path.native_file_string() << "\n";
	}
	return 0;
}

int process_fold( string fold ,string tar_fold)
{
//	path full_path( "aaa" , native); // relative directory
	path full_path( fold ); // absolute directory



	if ( !exists( full_path ) )
	{
		cout << "\nNot found: " << full_path.native_file_string() << endl;
		return 1;
	}



	if ( is_directory( full_path ) )
	{
		boost::progress_display p_display(1553841);

		directory_iterator end_iter;
		directory_iterator dir_itr( full_path );

		int count = 0;
		ofstream ofs;
		int tar_count = 0;
		string target = tar_fold + "/" + boost::lexical_cast<std::string>(tar_count) ;
		ofs.open(target.c_str());

		for ( ; dir_itr != end_iter; ++dir_itr )
		{
			try
			{
				if ( is_regular_file( dir_itr->status() ) )
				{
					if( process_file(fold + "/" + dir_itr->filename(), ofs) ){
						++p_display;
						if(gcount % 500 == 0)
							cout << gcount << endl;
						if(gcount % 50000 == 0){
							ofs.close();
							tar_count ++;
							target = tar_fold + "/" + boost::lexical_cast<std::string>(tar_count) ;
							ofs.open(target.c_str());
						}
					}
				}
			}
			catch ( const std::exception & ex )
			{
				cout << dir_itr->filename() << " " << ex.what() << std::endl;
			}
		}
		ofs.close();
		cout<< "count = "<< count << endl;
	}
	else // must be a file
	{
		cout << "\nFound: " << full_path.native_file_string() << "\n";
	}
	return 0;
}

void feature_stat(){
	string file = "/disk/data/wiki_txt/df.txt";
	string output = "/disk/data/wiki_txt/df_distribution.txt";
	string output2 = "/disk/data/wiki_txt/df_log_distribution.txt";
	map<int, int> count_map;
	map<int, int>::iterator it;

	ifstream ifs(file.c_str());
	string line;
	int freq;
	while(!ifs.eof()){
		ifs >> line;
		line = line.substr(1, line.length() - 2);
		int p1 = line.find(",");
		line = line.substr(p1+1);
		freq = atoi(line.c_str());
		if(freq == 0)
			continue;
		it = count_map.find(freq);

		if(it == count_map.end())
			count_map[freq] = 1;
		else
			it->second ++;
	}

	ifs.close();

	ofstream ofs(output.c_str());
	ofstream ofs2(output2.c_str());

	it = count_map.begin();
	for(;it != count_map.end(); it ++){
		ofs << it->first << "\t" << it->second << endl ;
		ofs2 << log((double)it->first) << "\t" << log((double)it->second) << endl ;
	}

	ofs.close();
	ofs2.close();
}
void term_selection(){
	string tf_file = "/disk/data/wiki_txt/tf.txt";
	string df_file = "/disk/data/wiki_txt/df.txt";
	string output = "/disk/data/wiki_txt/voc_15_15.txt";

	ifstream ifs1(tf_file.c_str());
	ifstream ifs2(df_file.c_str());
	ofstream ofs(output.c_str());

	int  min_tf = 15;
	int  min_df = 15;
	int tf, df;

	string line1,line2,term;

	double logN = log(1550000.0), TFIDF;
	double TFIDF_MIN = 0;

	int count = 0;

	while(!ifs1.eof()){
		ifs1 >> line1;
		ifs2 >> line2;

		line1 = line1.substr(1, line1.length() - 2);
		line2 = line2.substr(1, line2.length() - 2);

		int p1 = line1.find(",");
		term = line1.substr(0,p1);
		line1 = line1.substr(p1+1);
		line2 = line2.substr(p1+1);
		tf = atoi(line1.c_str());
		df = atoi(line2.c_str());
		if(!(tf >= min_tf && df >= min_df ))
			continue;

//		UString ustr(term, UString::CP949);

		bool is_english = true;

		for(unsigned int i = 0 ; i< term.length() ; i ++){
			unsigned char c = term[i] & 0x80;
			if(c != 0x00){
				is_english = false;
				break;
			}
		}

		if(!is_english)
			continue;

		char c = term[0];

		if(c >= '0' && c <= '9')
			continue;

		TFIDF = tf * (logN - log((double)df) );

		if(TFIDF < TFIDF_MIN)
			continue;

//		ofs << term << "\t" <<tf <<"\t"<<df <<"\t" <<TFIDF << endl;
		ofs << term << endl;
		count ++ ;
	}

	cout << "size = " << count << endl;

	ifs1.close();
	ifs2.close();
	ofs.close();
}

int process_fold_merge( string fold ,string target)
{
	path full_path( fold ); // absolute directory
	if ( !exists( full_path ) )
	{
		cout << "\nNot found: " << full_path.native_file_string() << endl;
		return 1;
	}
	if ( is_directory( full_path ) )
	{
		directory_iterator end_iter;
		directory_iterator dir_itr( full_path );

		ofstream ofs;

		ofs.open(target.c_str());

		const int buff_size = 200000;
		char buff[buff_size];
		string line;
		for (int i = 0 ; i <= 9 ; i ++ )
		{
			try
			{
				string source = fold + "/" + boost::lexical_cast<std::string>(i);
				cout << source << endl;
				ifstream ifs(source.c_str());
				while(!ifs.eof()){
					ifs.getline(buff, buff_size);
					string line(buff);
					ofs << line << endl;
				}
			}
			catch ( const std::exception & ex )
			{
				cout << dir_itr->filename() << " " << ex.what() << std::endl;
			}
		}
		ofs.close();
	}
	else // must be a file
	{
		cout << "\nFound: " << full_path.native_file_string() << "\n";
	}
	return 0;
}


void generate_corpus(string voc_path, string wiki_file, string target){
	//load dictionary
	map<string,int> str2id;
	map<string,int>::iterator it;
	ifstream ifs(voc_path.c_str());
	int id = 0;
	const int buff_size = 2000;
	char buff[buff_size];
	while(!ifs.eof()){
		ifs.getline(buff, buff_size);
		string term(buff);
		if(term.length() == 0)
			continue;
		str2id[term] = id;
		id ++;
	}
	ifs.close();

	ofstream ofs(target.c_str());

	cout << "size = " << id  << "," << str2id.size() << endl;

	map<int,int> tfmap;
	map<int,int>::iterator i2i_it;

	int doc_count = 0;

	ifs.open(wiki_file.c_str());
	int line_count = 0;

	int max_id = 0;

	while(!ifs.eof()){
		ifs.getline(buff, buff_size);
		string term(buff);
		if(term.length() == 0)
			continue;
		//a new document
		if(term.substr(0,8).compare("====doc:") == 0 && line_count != 0){
			int size = tfmap.size();
			if(size > 0){
				ofs << "<" << endl;
				for(i2i_it = tfmap.begin() ;i2i_it != tfmap.end() ; i2i_it++)
					ofs << i2i_it->first << "\t" << i2i_it->second << endl;
				ofs << ">" << endl;
				tfmap.clear();
			}
			doc_count ++;
			if(doc_count % 10000 == 0)
				cout << doc_count << endl;
			continue;
		}

		int p = term.find("\t");
		int freq = atoi( term.substr(p+1).c_str() );
		term = term.substr(0,p);
		it = str2id.find(term);
		if(it != str2id.end()){
			id = it->second;
			if(id > max_id)
				max_id = id;
			tfmap[id] = freq;
		}
		line_count ++;
	}
	int size = tfmap.size();
	if(size > 0){
		ofs << "<" << endl;
		for(i2i_it = tfmap.begin() ;i2i_it != tfmap.end() ; i2i_it++)
			ofs << i2i_it->first << "\t" << i2i_it->second << endl;
		ofs << ">" << endl;
		tfmap.clear();
	}
	cout << "max_id=" << max_id << endl;

	ofs.close();
	ifs.close();
}

void generate_serialized_corpus(string voc_path, string wiki_file, string target){

	//load dictionary
	map<string,int> str2id;
	map<string,int>::iterator it;
	ifstream ifs(voc_path.c_str());
	int id = 0;
	const int buff_size = 2000;
	char buff[buff_size];
	while(!ifs.eof()){
		ifs.getline(buff, buff_size);
		string term(buff);
		if(term.length() == 0)
			continue;
		str2id[term] = id;
		id ++;
	}
	ifs.close();

	ofstream ofs(target.c_str());
	boost::archive::binary_oarchive oa(ofs);

	cout << "size = " << id  << "," << str2id.size() << endl;

	map<int,int>* tfmap = new map<int,int>();

	int doc_count = 0;

	ifs.open(wiki_file.c_str());
	int line_count = 0;

	int max_id = 0;

	while(!ifs.eof()){
		ifs.getline(buff, buff_size);
		string term(buff);
		if(term.length() == 0)
			continue;
		//a new document
		if(term.substr(0,8).compare("====doc:") == 0 && line_count != 0){
			int size = tfmap->size();
			if(size > 0)
				oa << *tfmap;
			delete tfmap;
			tfmap = new map<int,int>();
			doc_count ++;
			if(doc_count % 100000 == 0)
				cout << doc_count << endl;
			continue;
		}

		int p = term.find("\t");
		int freq = atoi( term.substr(p+1).c_str() );
		term = term.substr(0,p);
		it = str2id.find(term);
		if(it != str2id.end()){
			id = it->second;
			if(id > max_id)
				max_id = id;
			(*tfmap)[id] = freq;
		}
		line_count ++;
	}
	int size = tfmap->size();
	if(size > 0)
		oa << *tfmap;
	delete tfmap;
	tfmap = new map<int,int>();
	oa << *tfmap;
	delete tfmap;
	cout << "max_id=" << max_id << endl;

	ofs.close();
	ifs.close();
}

int main(){
//	laconfig.useStopAnalyzer = true;
//	laconfig.stopDicPath = "../resource/stopword_en.txt";
//
//	laconfig.useLengthFilter = true;
//	laconfig.minChar = 2;
//
//	laconfig.useStemAnalyzer = true;
//	la.setConfig(laconfig);
//
//	process_fold("/media/新加卷/wiki/txt", "/disk/data/wiki_txt");

//	process_fold_stat("/disk/data/wiki_txt/process1", "/disk/data/wiki_txt");

//	feature_stat();

//	term_selection();

//	process_fold_merge("/disk/data/wiki_txt/process1", "/disk/data/wiki_txt/wiki_org.txt");

	generate_corpus("/disk/data/wiki_txt/voc_10_10.txt","/disk/data/wiki_txt/wiki_org.txt", "/disk/data/wiki_corpus_v2");
//	generate_serialized_corpus("/disk/data/wiki_txt/voc_10_10.txt","/disk/data/wiki_txt/wiki_org.txt", "/disk/data/wiki_txt/wiki_corpus");
//	process_fold_merge("/disk/data/wiki_txt/process2", "/disk/data/wiki_txt/wiki.mat");

}
*/
