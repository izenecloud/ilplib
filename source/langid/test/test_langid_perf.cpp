/** \file test_langid_perf.cpp
 * Test the performance in language identification.
 * Below is the usage examples:
 * \code
 * for "inputFile" in UTF-8 encoding, count primary language for each line,
 * set analyze size limit to 1024 (the default value),
 * and print the count results and time statistics.
 * $./test_langid_perf -f inputFile -l 1024
 *
 * \endcode
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 15, 2011
 */

#include "langid/langid.h"

#include <util/ClockTimer.h>
#include <util/ustring/UString.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cassert>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;
using namespace ilplib::langid;

void countLanguageFromFileLine(
    Analyzer& analyzer,
    const char* fileName,
    bool isConvertUStr
)
{
    vector<int> countVec(LANGUAGE_ID_NUM);

    ifstream ifs(fileName);
    if(! ifs)
    {
        cerr << "error in opening file " << fileName << endl;
        exit(1);
    }

    string line;
    LanguageID lineID = LANGUAGE_ID_UNKNOWN;
    int totalLine = 0;

    izenelib::util::ClockTimer timer;
    while(getline(ifs, line))
    {
        if(line.empty())
            continue;

        ++totalLine;

        bool result = true;
        if (isConvertUStr)
        {
            izenelib::util::UString ustr(line, izenelib::util::UString::UTF_8);
            //string utf8;
            //ustr.convertString(utf8, UString::UTF_8);
            //result = analyzer.languageFromString(utf8.c_str(), lineID);
            result = analyzer.languageFromString(ustr, lineID);
        }
        else
        {
            result = analyzer.languageFromString(line.c_str(), lineID);
        }

        if(! result)
        {
            cerr << "error to get language ID from string " << line << endl;
            exit(1);
        }
        ++countVec[lineID];
    }

    cout << "costs " << timer.elapsed() << " seconds" << endl;
    cout << "total lines: " << totalLine << endl;
    for(unsigned int i=0; i<countVec.size(); ++i)
    {
        if(countVec[i])
        {
            cout << Knowledge::getLanguageNameFromID(static_cast<LanguageID>(i))
                 << ": " << countVec[i] << endl;
        }
    }
}

int main(int argc, char* argv[])
{
    string inputFile;
    int limit = 1024;
    bool isConvertUStr = false;

    try
    {
        po::options_description config("Allowed options");
        config.add_options()
            ("help,h", "print help message")
            ("file,f", po::value<string>(&inputFile), "input file name, must required")
            ("limit,l", po::value<int>(&limit)->default_value(limit), "limit analyze size, zero for no limit")
            ("ustr,u", "convert each line to UString")
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

        if(!vm.count("file"))
        {
            cerr << cmdline_options << endl;
            exit(1);
        }

        if(vm.count("ustr"))
        {
            isConvertUStr = true;
        }
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
    analyzer->setOption(Analyzer::OPTION_TYPE_LIMIT_ANALYZE_SIZE, limit);

    analyzer->setKnowledge(knowledge);

    countLanguageFromFileLine(*analyzer, inputFile.c_str(), isConvertUStr);

    delete knowledge;
    delete analyzer;

    return 0;
}
