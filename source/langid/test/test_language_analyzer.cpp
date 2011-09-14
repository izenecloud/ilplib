/** \file test_language_analyzer.cpp
 * Test class LanguageAnalyzer.
 * Below is the usage examples:
 * \code
 * read line from standard input, print its primary language type.
 * $./test_language_analyzer
 *
 * read from input file, print its primary language type.
 * $./test_language_analyzer -f file
 *
 * read from input file, print each language types it contains in reverse order of sentence count.
 * $./test_language_analyzer -f file -m
 *
 * read from input file, segment it into single-language regions, and print each region content and its language type.
 * $./test_language_analyzer -f file -s
 *
 * If "-l" is specifid:
 * $./test_language_analyzer -f file -l
 *
 * It would test each line like below:
 * 1. print the primary language of the first line.
 * 2. print those rest lines which primary language is not the same as the first line.
 *
 * If "-r" is specifid:
 * $./test_language_analyzer -f file -r
 *
 * It would test each language region like below:
 * 1. segment the file into regions of different language
 * 2. print the language of the first region.
 * 3. print those rest regions which language is not the same as the first region.
 *
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 24, 2009
 */

#include "language_analyzer.h"
#include "script_table.h"
#include <ilplib.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cassert>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;
using namespace ilplib::langid;

/**
 * Print the language regions in string.
 * \param str the pointer to the start of string
 * \param regionVec the language regions
 */
void printStringRegion(const char* str, const vector<LanguageRegion>& regionVec)
{
    cout << "language region index: start\tlength\tlanguage" << endl;
    cout << "language region content" << endl;
    cout << endl;
    unsigned int totalSize = 0;

    for(unsigned int i=0; i<regionVec.size(); ++i)
    {
        assert(totalSize == regionVec[i].start_);

        cout << "region " << i << ": " << regionVec[i].start_ << "\t" << regionVec[i].length_ << "\t" << Knowledge::getLanguageNameFromID(regionVec[i].languageID_) << endl;
        cout << string(str + regionVec[i].start_, regionVec[i].length_) << endl;
        cout << endl;

        totalSize = regionVec[i].start_ + regionVec[i].length_;
    }
    cout << "total size: " << totalSize << endl;
}

/**
 * Print the language regions in file.
 * \param fileName the file name
 * \param regionVec the language regions
 */
void printFileRegion(const char* fileName, const vector<LanguageRegion>& regionVec)
{
    cout << "language region index: start\tlength\tlanguage" << endl;
    cout << "language region content" << endl;
    cout << endl;
    unsigned int totalSize = 0;

    ifstream ifs(fileName);
    if(! ifs)
    {
        cerr << "error in opening file " << fileName << endl;
        return;
    }

    LanguageID prevID = LANGUAGE_ID_NUM;
        
    for(unsigned int i=0; i<regionVec.size(); ++i)
    {
        assert(totalSize == regionVec[i].start_);

        LanguageID id = regionVec[i].languageID_;
        assert(id != prevID && "error: language id of adjacent regions should not be equal!");
        prevID = id;

        cout << "region " << i << ": " << regionVec[i].start_ << "\t" << regionVec[i].length_ << "\t" << Knowledge::getLanguageNameFromID(regionVec[i].languageID_) << endl;

        //ifs.seekg(regionVec[i].start_);

        unsigned int len = regionVec[i].length_;
        const unsigned int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];

        while(len)
        {
            unsigned int t = min(len, BUFFER_SIZE);

            ifs.read(buffer, t);
            cout.write(buffer, t);

            len -= t;
        }

        cout << endl << endl;

        totalSize = regionVec[i].start_ + regionVec[i].length_;
    }
    cout << "total size: " << totalSize << endl;
}

/**
 * Test each line in file.
 * \param analyzer language analyzer
 * \param fileName file name
 *
 * It would test each line like below:
 * 1. print the primary language of the first line.
 * 2. print those rest lines which primary language is not the same as the first line.
 */
void testFileLine(LanguageAnalyzer& analyzer, const char* fileName)
{
    ifstream ifs(fileName);
    if(! ifs)
    {
        cerr << "error in opening file " << fileName << endl;
        exit(1);
    }

    cout << "testing each line, output format: 1. the primary language of the first line \t2.those lines whose primary language is different\t3.statistics result" << endl;

    string line;
    LanguageID firstID, nextID;
    if(getline(ifs, line))
    {
        if(! analyzer.primaryIDFromString(line.c_str(), firstID))
        {
            cerr << "error to get primary ID from string " << line << endl;
            exit(1);
        }
        cout << "language of 1st line: " << Knowledge::getLanguageNameFromID(firstID) << endl << endl;

        int totalLine = 1;
        int diffLine = 0;
        while(getline(ifs, line))
        {
            if(line.empty())
            {
                continue;
            }

            ++totalLine;

            if(! analyzer.primaryIDFromString(line.c_str(), nextID))
            {
                cerr << "error to get primary ID from string " << line << endl;
                exit(1);
            }
            if(nextID != firstID)
            {
                cout << Knowledge::getLanguageNameFromID(nextID) << "\t" << line << endl;
                ++diffLine;
            }
        }

        cout << endl;
        cout << "diff lines: " << diffLine << endl;
        cout << "total lines: " << totalLine << endl;
        cout << "line correct rate: ";
        if(totalLine)
            cout << (double)(totalLine - diffLine) / totalLine * 100 << "%" << endl;
        else
            cout << "0%" << endl;
    }
}

/**
 * Test each region in file.
 * \param analyzer language analyzer
 * \param fileName file name
 *
 * It would test each language region like below:
 * 1. segment the file into regions of different language
 * 2. print the language of the first region.
 * 3. print those rest regions which language is not the same as the first region.
 */
void testFileRegion(LanguageAnalyzer& analyzer, const char* fileName)
{
    vector<LanguageRegion> regionVec;

    if(! analyzer.segmentFile(fileName, regionVec))
    {
        cerr << "error to segment file " << fileName << endl;
        exit(1);
    }

    if(regionVec.empty())
        return;

    ifstream ifs(fileName);
    if(! ifs)
    {
        cerr << "error in opening file " << fileName << endl;
        return;
    }

    cout << "testing each region, output format:" << endl;
    cout << "1.the language of the first region" << endl;
    cout << "2.those regions whose language is different" << endl;
    cout << "3.statistics result" << endl << endl;

    LanguageID firstID, nextID;
    firstID = regionVec[0].languageID_;
    cout << "language of 1st region: " << Knowledge::getLanguageNameFromID(firstID) << endl << endl;

    int totalRegion = 1;
    int diffRegion = 0;
    unsigned int maxLen = 0; // maximum region size which language is different with the 1st region
    for(unsigned int i=1; i<regionVec.size(); ++i)
    {
        ++totalRegion;

        nextID = regionVec[i].languageID_;
        if(nextID != firstID)
        {
            ++diffRegion;
            if(regionVec[i].length_ > maxLen)
                maxLen = regionVec[i].length_;

            cout << "region " << i << ": " << regionVec[i].start_ << "\t" << regionVec[i].length_ << "\t" << Knowledge::getLanguageNameFromID(nextID) << endl;

            ifs.seekg(regionVec[i].start_);

            unsigned int len = regionVec[i].length_;
            const unsigned int BUFFER_SIZE = 1024;
            char buffer[BUFFER_SIZE];

            while(len)
            {
                unsigned int t = min(len, BUFFER_SIZE);

                ifs.read(buffer, t);
                cout.write(buffer, t);

                len -= t;
            }

            cout << endl << endl;
        }
    }

    cout << endl;
    cout << "max length of diff region:" << maxLen << endl;
    cout << "diff regions: " << diffRegion << endl;
    cout << "total regions: " << totalRegion << endl;
    cout << "region correct rate: ";

    if(totalRegion)
        cout << (double)(totalRegion - diffRegion) / totalRegion * 100 << "%" << endl;
    else
        cout << "0%" << endl;
}

/**
 * Test LanguageAnalyzer::multipleIDFromFile().
 * \param analyzer language analyzer
 * \param fileName file name
 */
void testMultipleIDFromFile(LanguageAnalyzer& analyzer, const char* fileName)
{
    vector<LanguageID> idVec;
    if(! analyzer.multipleIDFromFile(fileName, idVec))
    {
        cerr << "error to get multiple ID from file " << fileName << endl;
        exit(1);
    }

    cout << "multiple language types in reverse order of sentence count:" << endl;
    for(unsigned int i=0; i<idVec.size(); ++i)
    {
        cout << i << ": " << Knowledge::getLanguageNameFromID(idVec[i]) << endl;
    }
    cout << endl;
}

/**
 * Test LanguageAnalyzer::segmentFile().
 * \param analyzer language analyzer
 * \param fileName file name
 */
void testSegmentFile(LanguageAnalyzer& analyzer, const char* fileName)
{
    vector<LanguageRegion> regionVec;

    if(! analyzer.segmentFile(fileName, regionVec))
    {
        cerr << "error to segment file " << fileName << endl;
        exit(1);
    }

    cout << endl;
    printFileRegion(fileName, regionVec);
    cout << endl;
}

/**
 * Test LanguageAnalyzer::primaryIDFromFile().
 * \param analyzer language analyzer
 * \param fileName file name
 */
void testPrimaryIDFromFile(LanguageAnalyzer& analyzer, const char* fileName)
{
    LanguageID id;
    if(! analyzer.primaryIDFromFile(fileName, id))
    {
        cerr << "error to get primary ID from file " << fileName << endl;
        exit(1);
    }

    cout << "primary language: " << Knowledge::getLanguageNameFromID(id) << endl << endl;
}

/**
 * Test LanguageAnalyzer::multipleIDFromString().
 * \param analyzer language analyzer
 */
void testMultipleIDFromString(LanguageAnalyzer& analyzer)
{
    vector<LanguageID> idVec;
    string line;
    while(getline(cin, line))
    {
        if(! analyzer.multipleIDFromString(line.c_str(), idVec))
        {
            cerr << "error to get multiple ID from string " << line << endl;
            exit(1);
        }

        cout << "multiple language types in reverse order of sentence count:" << endl;
        for(unsigned int i=0; i<idVec.size(); ++i)
        {
            cout << i << ": " << Knowledge::getLanguageNameFromID(idVec[i]) << endl;
        }
        cout << endl;
    }
}

/**
 * Test LanguageAnalyzer::segmentString().
 * \param analyzer language analyzer
 */
void testSegmentString(LanguageAnalyzer& analyzer)
{
    vector<LanguageRegion> regionVec;

    string line;
    while(getline(cin, line))
    {
        if(! analyzer.segmentString(line.c_str(), regionVec))
        {
            cerr << "error to segment string " << line << endl;
            exit(1);
        }

        cout << endl;
        printStringRegion(line.c_str(), regionVec);
        cout << endl;
    }
}

/**
 * Test LanguageAnalyzer::primaryIDFromString().
 * \param analyzer language analyzer
 */
void testPrimaryIDFromString(LanguageAnalyzer& analyzer)
{
    string line;
    while(getline(cin, line))
    {
        LanguageID id;
        if(! analyzer.primaryIDFromString(line.c_str(), id))
        {
            cerr << "error to get primary ID from string " << line << endl;
            exit(1);
        }
        cout << "primary language: " << Knowledge::getLanguageNameFromID(id) << endl << endl;
    }
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    string inputFile;
    bool isAnalyzeFile = false;
    bool isMultipleID = false;
    bool isSegment = false;
    bool isTestLine = false;
    bool isTestRegion = false;

    try
    {
        po::options_description config("Allowed options");
        config.add_options()
            ("help,h", "print help message")
            ("file,f", po::value<string>(&inputFile), "get the primary language type in input file")
            ("line,l", "test each line in input file")
            ("region,r", "test each region in input file")
            ("multiple,m", "get multiple language types if exist")
            ("segment,s", "segment into single-language regions")
            ;

        po::options_description must("Must arguments");
        must.add_options()
            //("config,c", po::value<string>(&configFile), "config file")
            ;

        po::options_description cmdline_options;
        cmdline_options.add(config).add(must);

        po::variables_map vm;
        store(po::command_line_parser(argc, argv).options(cmdline_options).run(), vm);
        po::notify(vm);    

        if(vm.count("help"))
        {
            cout << cmdline_options << endl;
            exit(1);
        }

        //if(! vm.count("config"))
        //{
            //cout << "error: no config file is set." << endl;
            //cout << cmdline_options << endl;
            //exit(1);
        //}

        if(vm.count("file"))
        {
            isAnalyzeFile = true;
            cout << "input file: " << inputFile << endl;
        }

        if(vm.count("multiple"))
        {
            isMultipleID = true;
            cout << "getting multiple language types" << endl;
        }

        if(vm.count("segment"))
        {
            isSegment = true;
            cout << "segmenting into language regions" << endl;
        }

        if(vm.count("line"))
        {
            isTestLine = true;
            cout << "testing each line" << endl;

            if(! isAnalyzeFile)
            {
                cerr << "error: no file name is given by -f." << endl;
                exit(1);
            }
        }

        if(vm.count("region"))
        {
            isTestRegion = true;
            cout << "testing each region" << endl;

            if(! isAnalyzeFile)
            {
                cerr << "error: no file name is given by -f." << endl;
                exit(1);
            }
        }

        cout << endl;
    }
    catch(std::exception& e)
    {
        cerr << "error: " << e.what() << "\n";
        exit(1);
    }

    // create instances
    ScriptTable scriptTable;
    SentenceBreakTable sentenceTable;
    LanguageAnalyzer analyzer(scriptTable, sentenceTable);

    // load configuration and table
    const char* scriptFile = "../../db/langid/config/script.def";
    const char* tableFile = "../../db/langid/config/ct.table";
    const char* sentenceFile = "../../db/langid/config/sentence_break.def";

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

    if(isTestLine)
    {
        testFileLine(analyzer, inputFile.c_str());
    }
    else if(isTestRegion)
    {
        testFileRegion(analyzer, inputFile.c_str());
    }
    else if(isAnalyzeFile)
    {
        if(isMultipleID)
        {
            testMultipleIDFromFile(analyzer, inputFile.c_str());
        }
        else if(isSegment)
        {
            testSegmentFile(analyzer, inputFile.c_str());
        }
        else
        {
            testPrimaryIDFromFile(analyzer, inputFile.c_str());
        }
    }
    else
    {
        if(isMultipleID)
        {
            testMultipleIDFromString(analyzer);
        }
        else if(isSegment)
        {
            testSegmentString(analyzer);
        }
        else
        {
            testPrimaryIDFromString(analyzer);
        }
    }

    return 0;
}
