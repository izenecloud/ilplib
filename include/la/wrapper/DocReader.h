/*
 * InputReader.h
 *
 *  Created on: 2009-7-3
 *      Author: zhjay
 */

#ifndef _LA_DOCREADER_H_
#define _LA_DOCREADER_H_

#include <wrapper/FileIO.h>
#include <iostream>


class DocReader {
	FileReader reader_;
public:
	DocReader(const std::string& file):reader_(file){

	}
	~DocReader(){

	}

	bool nextDoc(std::string& text){

		text.clear();
		std::string line;
		if(reader_.offset() == 0)
			reader_.readLine(line);

		while(reader_.readLine(line)){
			if(line.substr(0,5).compare("*TEXT") == 0 || line.substr(0,5).compare("*STOP") == 0)
				break;
			text.append(line + "\n");
		}

		return text.length() > 0;
	}

	void close(){
		reader_.close();
	}
	void open(const std::string file){
		reader_.open(file);
	}
};





#endif /* DOCREADER_H_ */
