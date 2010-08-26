/** \file test_sentence_break_table.cpp
 * Test class SentenceBreakTable.
 * Below is the usage examples:
 * \code
 * read line from standard input, print each character, its UCS2 value, and its sentence break type
 * $./test_sentence_break_table
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 04, 2009
 */

#include "sentence_break_table.h"
#include "ucs2_converter.h"

#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cassert>

using namespace std;
using namespace ilplib::langid;

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    // create instance of encoding type
    Utf8ToUcs2 converter;

    // create instance of sentence break table
    SentenceBreakTable table;

    // load configuration and table
    const char* configFile = "../db/langid/config/sentence_break.def";

    bool result = table.loadConfig(configFile);
    if(! result)
    {
        cerr << "error: fail to load file " << configFile << endl;
        exit(1);
    }

    string line;
    while(getline(cin, line))
    {
        const char* begin = line.c_str();
        const char* end = begin + line.size();

        // print each character and its UCS2 value
        size_t mblen;
        for(const char* p=begin; p!=end; p+=mblen)
        {
            unsigned short value = converter.convertToUCS2(p, end, &mblen);
            SentenceBreakType type = table.getProperty(value);

            string propStr;
            if(table.propertyToStr(type, propStr))
            {
                cout << string(p, mblen) <<
                    "\t0x" << setw(4) << setfill('0') << hex << value <<
                    "\t" << propStr << endl;
            }
            else
            {
                cerr << "error: unknown property: " << type << endl;
            }
        }
        cout << endl;
    }

    return 0;
}
