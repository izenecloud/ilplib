/*
 * TermVector.h
 *
 *  Created on: 2009-6-17
 *      Author: zhjay
 */

#ifndef TERMVECTOR_H_
#define TERMVECTOR_H_

#include <map>
#include <string>
#include <iostream>
#include <ostream>


class TermVector {
public:
    std::map<std::string, int> termFreqMap;
	TermVector(){}
	~TermVector(){}

	void add(std::string t);
	void add(std::string t, int freq);

	bool nextTerm(std::pair<std::string, int>& term){
		static std::map<std::string, int>::iterator it = termFreqMap.begin();
		if(it != termFreqMap.end()){
			term = *it;
			it ++;
			return true;
		}
		else
			return false;
	}

	friend std::ostream& operator<<( std::ostream & out, const TermVector& vec )
	{
        std::map<std::string, int>::const_iterator it = vec.termFreqMap.begin();
		for(; it != vec.termFreqMap.end() ; ++it)
			out<< "<" << it->first << "," << it->second<< ">" << std::endl;
		return out;
	}

};

#endif /* TERMVECTOR_H_ */
