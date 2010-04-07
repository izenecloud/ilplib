/** \file test_sentence_tokenizer.cpp
 * Test class SentenceTokenizer.
 * Below is the usage examples:
 * \code
 * read line from standard input, tokenize and print each sentence.
 * $./test_sentence_tokenizer
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 04, 2009
 */

#include "sentence_tokenizer.h"

#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cassert>

using namespace std;
using namespace langid;

#ifndef LANGID_DEBUG_PRINT
	#define LANGID_DEBUG_PRINT 1
#endif

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    // create instance of sentence break table
    SentenceBreakTable table;
    
    // load configuration
    const char* configFile = "../db/config/sentence_break.def";
    if(! table.loadConfig(configFile))
    {
        cerr << "error: fail to load file " << configFile << endl;
        exit(1);
    }
    
    // create instance of SentenceTokenizer
    SentenceTokenizer tokenizer(table);
    
    string line;
    while(getline(cin, line))
    {
        cout << "raw: " << line << endl;

        int i = 0;
        string sentStr;
        const char* p = line.c_str();
        while(int len = tokenizer.getSentenceLength(p))
        {
            // print each sentence
            sentStr.assign(p, len);
            cout << "sentence " << i++ << ": " << sentStr << endl;

            p += len;
        }
        cout << "total: " << i << endl;

        cout << endl;
    }

    return 0;
}
