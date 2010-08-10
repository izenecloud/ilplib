/*
 * t_UnigramAnalyzer.cpp
 *
 *  Created on: 2010.06.28
 *      Author: Wei Cao
 */
#include "test_def.h"

#include <la/analyzer/NGramAnalyzer.h>
#include <la/tokenizer/Tokenizer.h>
#include <ctime>
using namespace la;
using namespace izenelib::util;
using namespace std;

int main(){

	Tokenizer tokenizer;

	Analyzer* analyzer = new NGramAnalyzer(1, 1, 2194967296);
//	analyzer->setRetFlag_index(Analyzer::ANALYZE_SECOND_);

    UString ustr("今晚十点 荷兰 对 斯洛伐克", UString::UTF_8);
    TermList tlist;
    TermList outlist;

std::string str;
ustr.convertString(str, UString::UTF_8);
std::cout << str << std::endl;

    tokenizer.tokenize( ustr, tlist );

printTermList( tlist );
    analyzer->analyze_index( tlist, outlist);
printTermList( outlist );

    delete analyzer;
	return 1;
}
