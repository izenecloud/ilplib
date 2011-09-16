/** \file test_langid_run.cpp
 * Test the features in langid library.
 * Below is the usage examples:
 * \code
 * In below commands, instead of reading each line from standard input,
 * "-f file" could be added to specify an input file.
 *
 * identify character encoding
 * $./test_langid_run -t encoding
 *
 * identify primary language in UTF-8 encoding (the default command option)
 * $./test_langid_run -t language
 *
 * identify a list of languages in UTF-8 multi-lingual text
 * $./test_langid_run -t list
 *
 * segment the UTF-8 multi-lingual text into single-language regions
 * $./test_langid_run -t segment
 *
 * tokenize the UTF-8 text into sentences
 * $./test_langid_run -t sentence
 *
 * identify character encoding for each line of "inputFile", and print those different results.
 * $./test_langid_run -t encoding -f inputFile -l
 *
 * identify primary language for each line of "inputFile" in UTF-8 encoding, and print those different results.
 * $./test_langid_run -t language -f inputFile -l
 *
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 25, 2009
 */

#include "langid/langid.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cassert>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;
using namespace ilplib::langid;

namespace
{
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

        const unsigned int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        unsigned int t;
        for(unsigned int len = regionVec[i].length_; len; len-=t)
        {
            t = min(len, BUFFER_SIZE);

            ifs.read(buffer, t);
            cout.write(buffer, t);
        }

        cout << endl << endl;

        totalSize = regionVec[i].start_ + regionVec[i].length_;
    }
    cout << "total size: " << totalSize << endl;
}

} // namespace

/**
 * Test Analyzer::encodingFromFile().
 * \param analyzer analyzer
 * \param fileName file name
 */
void testEncodingFromFile(Analyzer& analyzer, const char* fileName)
{
    EncodingID id;
    if(! analyzer.encodingFromFile(fileName, id))
    {
        cerr << "error to get encoding ID from file " << fileName << endl;
        exit(1);
    }

    cout << "encoding: " << Knowledge::getEncodingNameFromID(id) << endl << endl;
}

/**
 * Test Analyzer::encodingFromString().
 * \param analyzer analyzer
 */
void testEncodingFromString(Analyzer& analyzer)
{
    string line;
    while(getline(cin, line))
    {
        EncodingID id;
        if(! analyzer.encodingFromString(line.c_str(), id))
        {
            cerr << "error to get encoding ID from string " << line << endl;
            exit(1);
        }
        cout << "encoding: " << Knowledge::getEncodingNameFromID(id) << endl << endl;
    }
}

/**
 * Test Analyzer::languageFromFile().
 * \param analyzer analyzer
 * \param fileName file name
 */
void testLanguageFromFile(Analyzer& analyzer, const char* fileName)
{
    LanguageID id;
    if(! analyzer.languageFromFile(fileName, id))
    {
        cerr << "error to get language ID from file " << fileName << endl;
        exit(1);
    }

    cout << "primary language: " << Knowledge::getLanguageNameFromID(id) << endl << endl;
}

/**
 * Test Analyzer::languageFromString().
 * \param analyzer analyzer
 */
void testLanguageFromString(Analyzer& analyzer)
{
    string line;
    while(getline(cin, line))
    {
        LanguageID id;
        if(! analyzer.languageFromString(line.c_str(), id))
        {
            cerr << "error to get language ID from string " << line << endl;
            exit(1);
        }
        cout << "primary language: " << Knowledge::getLanguageNameFromID(id) << endl << endl;
    }
}

/**
 * Test Analyzer::languageListFromFile().
 * \param analyzer analyzer
 * \param fileName file name
 */
void testLanguageListFromFile(Analyzer& analyzer, const char* fileName)
{
    vector<LanguageID> idVec;
    if(! analyzer.languageListFromFile(fileName, idVec))
    {
        cerr << "error to get language ID list from file " << fileName << endl;
        exit(1);
    }

    cout << "raw file name: " << fileName << endl;
    cout << "multiple language types in descending order of sentence count:" << endl;
    for(unsigned int i=0; i<idVec.size(); ++i)
    {
        cout << i << ": " << Knowledge::getLanguageNameFromID(idVec[i]) << endl;
    }
    cout << endl;
}

/**
 * Test Analyzer::languageListFromString().
 * \param analyzer analyzer
 */
void testLanguageListFromString(Analyzer& analyzer)
{
    vector<LanguageID> idVec;
    string line;
    while(getline(cin, line))
    {
        if(! analyzer.languageListFromString(line.c_str(), idVec))
        {
            cerr << "error to get language ID list from string " << line << endl;
            exit(1);
        }

        cout << "multiple language types in descending order of sentence count:" << endl;
        for(unsigned int i=0; i<idVec.size(); ++i)
        {
            cout << i << ": " << Knowledge::getLanguageNameFromID(idVec[i]) << endl;
        }
        cout << endl;
    }
}

/**
 * Test Analyzer::segmentFile().
 * \param analyzer analyzer
 * \param fileName file name
 */
void testSegmentFile(Analyzer& analyzer, const char* fileName)
{
    vector<LanguageRegion> regionVec;

    if(! analyzer.segmentFile(fileName, regionVec))
    {
        cerr << "error to segment file " << fileName << endl;
        exit(1);
    }

    printFileRegion(fileName, regionVec);
    cout << endl;
}

/**
 * Test Analyzer::segmentString().
 * \param analyzer analyzer
 */
void testSegmentString(Analyzer& analyzer)
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

        printStringRegion(line.c_str(), regionVec);
        cout << endl;
    }
}

/**
 * Test Analyzer::segmentString().
 * \param analyzer analyzer
 * \param is input stream
 */
void testSentenceLength(Analyzer& analyzer, istream& is)
{
    string line;
    while(getline(is, line))
    {
        const char* p = line.c_str();
        string sentStr;
        while(int len = analyzer.sentenceLength(p))
        {
            sentStr.assign(p, len); // get each sentence
            cout << sentStr << endl; // print each sentence
            p += len; // move to the begining of next sentence
        }
        cout << endl;
    }
}

/**
 * Run Analyzer::languageFromString() for each line in \e fileName, and print those different results with Analyzer::languageFromFile().
 * \param analyzer analyzer
 * \param fileName file name
 *
 * It would test like below:
 * 1. print the primary language of the file.
 * 2. print those lines which primary language is not the same as the file.
 */
void testLanguageFromFileLine(Analyzer& analyzer, const char* fileName)
{
    LanguageID fileID;
    if(! analyzer.languageFromFile(fileName, fileID))
    {
        cerr << "error to get language ID from file " << fileName << endl;
        exit(1);
    }

    cout << "file primary language: " << Knowledge::getLanguageNameFromID(fileID) << endl << endl;

    ifstream ifs(fileName);
    if(! ifs)
    {
        cerr << "error in opening file " << fileName << endl;
        exit(1);
    }

    cout << "========== the lines with different primary language:" << endl;

    string line;
    LanguageID lineID;
    int totalLine = 0;
    int diffLine = 0;
    while(getline(ifs, line))
    {
        if(line.empty())
            continue;

        ++totalLine;

        if(! analyzer.languageFromString(line.c_str(), lineID))
        {
            cerr << "error to get language ID from string " << line << endl;
            exit(1);
        }
        if(lineID != fileID)
        {
            cout << Knowledge::getLanguageNameFromID(lineID) << "\t" << line << endl;
            ++diffLine;
        }
    }

    cout << endl;
    cout << "========== statistics result:" << endl;
    cout << "diff lines: " << diffLine << endl;
    cout << "total lines: " << totalLine << endl;
    cout << "line correct rate: ";
    if(totalLine)
        cout << (double)(totalLine - diffLine) / totalLine * 100 << "%" << endl;
    else
        cout << "0%" << endl;
}

/**
 * Run Analyzer::encodingFromString() for each line in \e fileName, and print those different results with Analyzer::encodingFromFile().
 * \param analyzer analyzer
 * \param fileName file name
 *
 * It would test like below:
 * 1. print the encoding of the file.
 * 2. print those lines which encoding is not the same as the file.
 */
void testEncodingFromFileLine(Analyzer& analyzer, const char* fileName)
{
    EncodingID fileID;
    if(! analyzer.encodingFromFile(fileName, fileID))
    {
        cerr << "error to get encoding ID from file " << fileName << endl;
        exit(1);
    }

    cout << "file encoding: " << Knowledge::getEncodingNameFromID(fileID) << endl << endl;

    ifstream ifs(fileName);
    if(! ifs)
    {
        cerr << "error in opening file " << fileName << endl;
        exit(1);
    }

    cout << "========== the lines with different encoding:" << endl;

    string line;
    EncodingID lineID;
    int totalLine = 0;
    int diffLine = 0;
    while(getline(ifs, line))
    {
        if(line.empty())
            continue;

        ++totalLine;

        if(! analyzer.encodingFromString(line.c_str(), lineID))
        {
            cerr << "error to get encoding ID from string " << line << endl;
            exit(1);
        }
        if(lineID != fileID)
        {
            cout << Knowledge::getEncodingNameFromID(lineID) << "\t" << line << endl;
            ++diffLine;
        }
    }

    cout << endl;
    cout << "========== statistics result:" << endl;
    cout << "diff lines: " << diffLine << endl;
    cout << "total lines: " << totalLine << endl;
    cout << "line correct rate: ";
    if(totalLine)
        cout << (double)(totalLine - diffLine) / totalLine * 100 << "%" << endl;
    else
        cout << "0%" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    string inputFile, typeStr;
    bool isAnalyzeFile = false;
    bool isTestLine = false;

    try
    {
        po::options_description config("Allowed options");
        config.add_options()
            ("help,h", "print help message")
            ("file,f", po::value<string>(&inputFile), "use an input file instead of standard input")
            ("type,t", po::value<string>(&typeStr)->default_value("language"), "type of function: [encoding, language, list, segment, sentence]")
            ("line,l", "test each line in input file")
            ;

        po::options_description cmdline_options;
        cmdline_options.add(config);

        po::variables_map vm;
        store(po::command_line_parser(argc, argv).options(cmdline_options).run(), vm);
        po::notify(vm);    

        if(vm.count("help"))
        {
            cout << cmdline_options << endl;
            exit(1);
        }

        if(vm.count("file"))
        {
            isAnalyzeFile = true;
            //cout << "input file: " << inputFile << endl;
        }

        if(typeStr == "encoding" || typeStr == "language" || typeStr == "list" || typeStr == "segment" || typeStr == "sentence")
        {
            //cout << "function type: " << typeStr << endl;
        }
        else
        {
            cerr << "unknown function type: " << typeStr << endl;
            cout << cmdline_options << endl;
            exit(1);
        }

        if(vm.count("line"))
        {
            isTestLine = true;
            //cout << "testing each line" << endl;

            if(! isAnalyzeFile)
            {
                cerr << "error: no input file is given by -f." << endl;
                exit(1);
            }

            if(typeStr != "encoding" && typeStr != "language")
            {
                cerr << "error: line test mode is only valid for \"encoding\" or \"language\" type." << endl;
                exit(1);
            }
        }

        //cout << endl;
    }
    catch(std::exception& e)
    {
        cerr << "error: " << e.what() << "\n";
        exit(1);
    }

    // create instances
    Factory* factory = Factory::instance();
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // model files
    const char* encodingModel = "../../db/langid/model/encoding.bin";
    const char* languageModel = "../../db/langid/model/language.bin";

    // load encoding model for encoding identification
    if(! knowledge->loadEncodingModel(encodingModel))
    {
        cerr << "error: fail to load file " << encodingModel << endl;
        exit(1);
    }

    // load language model for language identification or sentence tokenization
    if(! knowledge->loadLanguageModel(languageModel))
    {
        cerr << "error: fail to load file " << languageModel << endl;
        exit(1);
    }

    // not to limit analyze size
    //analyzer->setOption(Analyzer::OPTION_TYPE_LIMIT_ANALYZE_SIZE, 0);

    // set minimum block size
    //analyzer->setOption(Analyzer::OPTION_TYPE_BLOCK_SIZE_THRESHOLD, 100);

    // identify Chinese Traditional text as Chinese Simplified language
    //analyzer->setOption(Analyzer::OPTION_TYPE_NO_CHINESE_TRADITIONAL, 1);
    
    // set knowledge
    analyzer->setKnowledge(knowledge);
    
    // identify character encoding
    if(typeStr == "encoding")
    {
        if(isAnalyzeFile)
        {
            if(isTestLine)
                testEncodingFromFileLine(*analyzer, inputFile.c_str());
            else
                testEncodingFromFile(*analyzer, inputFile.c_str());
        }
        else
            testEncodingFromString(*analyzer);
    }
    // identify the single primary language in UTF-8 encoding
    else if(typeStr == "language")
    {
        if(isAnalyzeFile)
        {
            if(isTestLine)
                testLanguageFromFileLine(*analyzer, inputFile.c_str());
            else
                testLanguageFromFile(*analyzer, inputFile.c_str());
        }
        else
            testLanguageFromString(*analyzer);
    }
    // identify the list of multiple languages in UTF-8 encoding
    else if(typeStr == "list")
    {
        if(isAnalyzeFile)
            testLanguageListFromFile(*analyzer, inputFile.c_str());
        else
            testLanguageListFromString(*analyzer);
    }
    // segment the UTF-8 multi-lingual text into single-language regions
    else if(typeStr == "segment")
    {
        if(isAnalyzeFile)
            testSegmentFile(*analyzer, inputFile.c_str());
        else
            testSegmentString(*analyzer);
    }
    // tokenize the UTF-8 text into sentences
    else if(typeStr == "sentence")
    {
        if(isAnalyzeFile)
        {
            ifstream ifs(inputFile.c_str());
            if(! ifs)
            {
                cerr << "error in opening file " << inputFile << endl;
                exit(1);
            }
            testSentenceLength(*analyzer, ifs);
        }
        else
            testSentenceLength(*analyzer, cin);
    }
    
    delete knowledge;
    delete analyzer;

    return 0;
}
