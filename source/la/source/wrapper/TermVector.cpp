/*
 * TermVector.cpp
 *
 *  Created on: 2009-6-17
 *      Author: zhjay
 */

#include <la/wrapper/TermVector.h>

using namespace std;

void TermVector::add(string t){
	map<string, int>::iterator it = termFreqMap.find(t);
	if(it != termFreqMap.end())
		it->second ++ ;
	else
		termFreqMap[t] = 1;
}
void TermVector::add(string t, int freq){
	map<string, int>::iterator it = termFreqMap.find(t);
	if(it != termFreqMap.end())
		it->second += freq ;
	else
		termFreqMap[t] = freq;
}
//pair<int, string> TermVector::getPair(string){
//	return pair<int, string>(0,"");
//}
//
//int TermVector::freq(string){
//	return 0;
//}
//
//void TermVector::remove(string){
//
//}
//ostream & TermVector::operator<<( ostream & out, const TermVector& vec )
//{
//	map<string, int>::iterator it = vec.termFreqMap_.begin();
////		map<string, int>::iterator it = vec.termFreqMap_.find("");
//	for(; it != vec.termFreqMap_.end() ; it++)
//		out<< "<" << it->first << "," << it->second<< ">" << endl;
//	return out;
//}
