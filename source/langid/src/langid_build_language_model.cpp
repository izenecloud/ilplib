/** \file langid_build_language_model.cpp
 * Build language model from configuration to binary format.
 * Below is the usage examples:
 * \code
 * To create "language.bin":
 * $./langid_build_language_model language.bin
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 29, 2009
 */

#include "script_table.h"
#include "sentence_break_table.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace ilplib::langid;

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        cerr << "Usage: " << argv[0] << " language.bin" << endl;
        exit(1);
    }

    const char* outputFile = argv[1];
    cout << "output: " << outputFile << endl;
    cout << endl;

    // create instances
    ScriptTable scriptTable;
    SentenceBreakTable sentenceTable;

    // configuration files
    const char* scriptFile = "../db/langid/config/script.def";
    const char* tableFile = "../db/langid/config/ct.table";
    const char* sentenceFile = "../db/langid/config/sentence_break.def";

    if(! scriptTable.loadConfig(scriptFile))
    {
        cerr << "error: fail to load file " << scriptFile << endl;
        exit(1);
    }

    if(! scriptTable.loadTable(tableFile))
    {
        cerr << "error: fail to load file " << tableFile << endl;
        exit(1);
    }

    if(! sentenceTable.loadConfig(sentenceFile))
    {
        cerr << "error: fail to load file " << sentenceFile << endl;
        exit(1);
    }

    ofstream ofs(outputFile, ios::binary | ios_base::out);
    if(! ofs)
    {
        cerr << "error: could not create file " << outputFile << endl;
        exit(1);
    }

    if(! scriptTable.saveBinary(ofs))
    {
        cerr << "error: fail to save binary script content into file " << outputFile << endl;
        exit(1);
    }

    if(! sentenceTable.saveBinary(ofs))
    {
        cerr << "error: fail to save binary sentence break content into file " << outputFile << endl;
        exit(1);
    }

    return 0;
}
