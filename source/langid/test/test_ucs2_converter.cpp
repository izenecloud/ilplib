/** \file test_ucs2_converter.cpp
 * Test class UCS2_Converter.
 * Below is the usage examples:
 * \code
 * read line from standard input, print each character and its UCS2 value
 * $./test_ucs2_converter
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 19, 2009
 */

#include "ucs2_converter.h"

#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>

using namespace std;
using namespace ilplib::langid;

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    // create instance of encoding type
    Utf8ToUcs2 converter;
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
            cout << "0x" << setw(4) << setfill('0') << hex << uppercase << value << "\t# " << string(p, mblen) << endl;
        }
    }

    return 0;
}
