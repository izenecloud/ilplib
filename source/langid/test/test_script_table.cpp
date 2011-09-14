/** \file test_script_table.cpp
 * Test class ScriptTable.
 * Below is the usage examples:
 * \code
 * read line from standard input, print each character, its UCS2 value, and its script type
 * $./test_script_table
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 20, 2009
 */

#include "script_table.h"
#include "ucs2_converter.h"


#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cassert>

#define LANGID_DEBUG_PRINT 0

using namespace std;
using namespace ilplib::langid;

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    // create instance of encoding type
    Utf8ToUcs2 converter;

    // create instance of script type table
    ScriptTable table;

    // load configuration and table
    const char* configFile = "../../db/langid/config/script.def";
    const char* tableFile = "../../db/langid/config/ct.table";

    bool result = table.loadConfig(configFile);
    if(! result)
    {
        cerr << "error: fail to load file " << configFile << endl;
        exit(1);
    }

    result = table.loadTable(tableFile);
    if(! result)
    {
        cerr << "error: fail to load file " << tableFile << endl;
        exit(1);
    }

    // count each character type
    vector<int> countVec(SCRIPT_TYPE_NUM);

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
            ScriptType type = table.getProperty(value);

            ++countVec[type];

#if LANGID_DEBUG_PRINT
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
#endif
        }
    }

    // print the statistics finally
    cout << "===== statistics of character type =====" << endl;
    cout << dec;
    int total = 0;
    for(int i=0; i<SCRIPT_TYPE_NUM; ++i)
    {
        total += countVec[i];
    }

    cout << fixed << setprecision(2);
    string propStr;
    for(int i=0; i<SCRIPT_TYPE_NUM; ++i)
    {
        if(table.propertyToStr(static_cast<ScriptType>(i), propStr))
        {
            cout << propStr << ":\t" << countVec[i] << "\t";
        }
        else
        {
            cerr << "error: unknown property: " << i << endl;
            continue;
        }
        if(total)
            cout << (double)countVec[i]/total*100 << "%" << endl;
        else
            cout << "0%" << endl;
    }

    return 0;
}
