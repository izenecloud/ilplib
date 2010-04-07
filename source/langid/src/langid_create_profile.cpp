/** \file langid_create_profile.cpp
 * Create profile from document.
 * Below is the usage examples:
 * \code
 * To create "profile" from "document":
 * $./langid_create_profile document profile
 *
 * You could also specify the ngram value as "5", and profile length as "400":
 * $./langid_create_profile -n 5 -l 400 document profile
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 04, 2009
 */

#include "profile.h"

#include <iostream>
#include <cstdlib>

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
    string input, output;

    try
    {
        po::options_description config("Allowed options");
        config.add_options()
            ("help,h", "print help message")
            ("ngram,n", po::value<int>(&ngram)->default_value(5), "set ngram range")
            ("length,l", po::value<int>(&length)->default_value(400), "set profile length")
            ;

        po::options_description must("Must arguments");
        must.add_options()
            ("input", po::value<string>(&input), "input file")
            ("output", po::value<string>(&output), "output file")
            ;

        po::options_description cmdline_options;
        cmdline_options.add(config).add(must);

        po::positional_options_description positional;
        positional.add("input", 1);
        positional.add("output", 1);

        po::variables_map vm;
        store(po::command_line_parser(argc, argv).options(cmdline_options).positional(positional).run(), vm);
        po::notify(vm);    

        if(vm.count("help"))
        {
            cout << cmdline_options << endl;
            exit(1);
        }

        if(! vm.count("input"))
        {
            cout << "error: no input file is set." << endl;
            cout << cmdline_options << endl;
            exit(1);
        }

        if(! vm.count("output"))
        {
            cout << "error: no output file is set." << endl;
            cout << cmdline_options << endl;
            exit(1);
        }
    }
    catch(exception& e)
    {
        cerr << "error: " << e.what() << "\n";
        exit(1);
    }

    cout << "ngram: " << ngram << endl;
    cout << "length: " << length << endl;
    cout << "input: " << input << endl;
    cout << "output: " << output << endl;
    cout << endl;

    Profile profile(ngram, length);
    bool result = profile.generateFromFile(input.c_str());
    if(! result)
    {
        cerr << "error in generating N-grams from document." << endl;
        exit(1);
    }

    result = profile.saveText(output.c_str());
    if(! result)
    {
        cerr << "error in saving N-grams to profile." << endl;
        exit(1);
    }

    return 0;
}
