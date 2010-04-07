/** \file langid_build_encoding_model.cpp
 * Build encoding model from text to binary format.
 * Below is the usage examples:
 * \code
 * To create "encoding.bin" from "encoding.config":
 * $./langid_build_encoding_model ../db/langid/config/encoding.config encoding.bin
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 28, 2009
 */

#include "profile_model.h"

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
    string input, output;

    try
    {
        po::options_description config("Allowed options");
        config.add_options()
            ("help,h", "print help message")
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

    cout << "input: " << input << endl;
    cout << "output: " << output << endl;
    cout << endl;

    if(! ProfileModel::buildModel(input.c_str(), output.c_str()))
    {
        cerr << "error in encoding model build." << endl;
        exit(1);
    }

    return 0;
}
