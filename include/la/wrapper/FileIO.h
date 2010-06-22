/*
 * InputReader.h
 *
 *  Created on: 2009-7-3
 *      Author: zhjay
 */

#ifndef FILEIO_H_
#define FILEIO_H_

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

class FileReader {
	std::ifstream ifs;
	char* buff;
	int buffsize;
public:
	FileReader(const std::string& file,int fsize = 10000){
		ifs.open(file.c_str());
		if(ifs.fail()){
			std::cout << "File open error:" << file << std::endl;
			throw std::exception();
		}
		buffsize = fsize;
		buff = new char[buffsize];
	}
    FileReader(const char* file,int fsize = 10000){
		ifs.open(file);
		if(ifs.fail()){
			std::cout << "File open error:" << file << std::endl;
			throw std::exception();
		}
		buffsize = fsize;
		buff = new char[buffsize];
	}
	~FileReader(){
		delete buff;
	}
	bool readLine(std::string& line){
		if(!ifs.eof()){
			ifs.getline(buff, buffsize);
			line.assign(buff);
			return true;
		}
		else
			return false;
	}
	void restart(){
		ifs.clear();
		ifs.seekg(0,std::ios::beg);
	}
	void close(){
		ifs.close();
	}
	void open(const std::string& file){
		ifs.open(file.c_str());
		if(ifs.fail()){
			std::cout << "File open error:" << file << std::endl;
			throw std::exception();
		}
	}
	void open(const char* file){
		ifs.open(file);
		if(ifs.fail()){
			std::cout << "File open error:" << file << std::endl;
			throw std::exception();
		}
	}
	int offset(){
		return ifs.tellg();
	}
	bool eof(){
		return ifs.eof();
	}
};


class FileWriter {
	std::ofstream ofs;
public:
	FileWriter(const std::string& file){
		ofs.open(file.c_str());
		if(ofs.fail()){
			std::cout << "File open error:" << file << std::endl;
			throw std::exception();
		}
	}
	FileWriter(const char* file){
		ofs.open(file);
		if(ofs.fail()){
			std::cout << "File open error:" << file << std::endl;
			throw std::exception();
		}
	}
	~FileWriter(){
	}

	bool writeLine(const std::string& line){
		ofs << line << std::endl;
		return true;
	}

	bool write(const std::string& __n){
		ofs << __n;
		return true;
	}

	void close(){
		ofs.close();
	}

	void open(const char* file){
		ofs.open(file);
		if(ofs.fail()){
			std::cout << "File open error:" << file << std::endl;
			throw std::exception();
		}
	}
};



#endif /* FILEIO_H_ */
