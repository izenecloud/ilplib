/** \file langid_select_profile.cpp
 * Select the profile, which distance from the document is the shortest.
 * Below is the usage examples:
 * \code
 * Use "-i" to select the best profile for "input", the candidate profiles are configured in "config":
 * $./langid_select_profile -c config -i input
 *
 * If "-i" is not specified, it would read string from standard input, please press CTRL-C to exit the loop:
 * $./langid_select_profile -c config
 *
 * If "-t" is specifid with "-i", it would test each line just like below:
 * 1. select the best profile for the entire file "input".
 * 2. select the best profile for each line in file "input", and print those lines which result is not the same as that of step 1.
 *
 * You could also specify the ngram value as "5", and profile length as "400":
 * $./langid_select_profile -n 5 -l 400 -c config -i input
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 10, 2009
 */

#include "profile_selector.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;
using namespace langid;

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    int ngram, length;
    string configFile, inputFile;
    bool isInputFile = false;
    bool isTestLine = false;

    try
    {
        po::options_description config("Allowed options");
        config.add_options()
            ("help,h", "print help message")
            ("ngram,n", po::value<int>(&ngram)->default_value(5), "set ngram range")
            ("length,l", po::value<int>(&length)->default_value(400), "set profile length")
            ("input,i", po::value<string>(&inputFile), "input file")
            ("test,t", "test each line in input file")
            ;

        po::options_description must("Must arguments");
        must.add_options()
            ("config,c", po::value<string>(&configFile), "config file")
            ;

        po::options_description cmdline_options;
        cmdline_options.add(config).add(must);

        //po::positional_options_description positional;
        //positional.add("document", 1);

        po::variables_map vm;
        //store(po::command_line_parser(argc, argv).options(cmdline_options).positional(positional).run(), vm);
        store(po::command_line_parser(argc, argv).options(cmdline_options).run(), vm);
        po::notify(vm);    

        if(vm.count("help"))
        {
            cout << cmdline_options << endl;
            exit(1);
        }

        if(! vm.count("config"))
        {
            cout << "error: no config file is set." << endl;
            cout << cmdline_options << endl;
            exit(1);
        }

        if(vm.count("input"))
        {
            isInputFile = true;
        }

        if(vm.count("test"))
        {
            isTestLine = true;
        }
    }
    catch(exception& e)
    {
        cerr << "error: " << e.what() << "\n";
        exit(1);
    }

    cout << "ngram: " << ngram << endl;
    cout << "length: " << length << endl;
    cout << "config: " << configFile << endl;
    if(isInputFile)
    {
        cout << "input: " << inputFile << endl;

        if(isTestLine)
        {
            cout << "test each line: " << isTestLine << endl;
        }
    }
    cout << endl;

    ProfileSelector selector(ngram, length);
    bool result = selector.loadProfiles(configFile.c_str());
    if(! result)
    {
        cerr << "error in loading config file " << configFile << endl;
        exit(1);
    }

    const char* best;
    if(isInputFile)
    {
        best = selector.selectProfileFromFile(inputFile.c_str());
        if(! best)
        {
            cerr << "error in selecting profile for input file " << inputFile << endl;
            exit(1);
        }
        cout << "best profile: " << best << endl;

        if(isTestLine)
        {
            ifstream ifs(inputFile.c_str());
            if(! ifs)
            {
                cerr << "error in opening file " << inputFile << endl;
                exit(1);
            }

            cout << "testing each line, output format: 1. diff result\t2.line length\t3.line string" << endl;

            string line;
            const char* lineResult;
            int totalLine = 0;
            int diffLine = 0;
            while(getline(ifs, line))
            {
                if(line.empty())
                {
                    continue;
                }

                ++totalLine;

                lineResult = selector.selectProfileFromStr(line.c_str());
                if(! lineResult)
                {
                    cerr << "error in selecting profile for line string: " << line << endl;
                    exit(1);
                }

                if(strcmp(best, lineResult) != 0)
                {
                    cout << lineResult << "\t" << line.length() << "\t" << line << endl;
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
    else
    {
        string line;
        while(getline(cin, line))
        {
            best = selector.selectProfileFromStr(line.c_str());
            if(! best)
            {
                cerr << "error in selecting profile for input string: " << line << endl;
                exit(1);
            }
            cout << "best profile: " << best << endl;
            cout << endl;
        }
    }

    return 0;
}
