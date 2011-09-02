/** \file langid_measure_profile.cpp
 * Create profile from document.
 * Below is the usage examples:
 * \code
 * To measure distance of "document" from "profile":
 * $./langid_measure_profile profile document
 *
 * You could also specify the ngram value as "5", and profile length as "400":
 * $./langid_measure_profile -n 5 -l 400 profile document
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 09, 2009
 */

#include "profile.h"

#include <iostream>
#include <cstdlib>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;
using namespace ilplib::langid;

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    int ngram, length;
    string profileFile, docFile;

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
            ("profile", po::value<string>(&profileFile), "profile file")
            ("document", po::value<string>(&docFile), "document file")
            ;

        po::options_description cmdline_options;
        cmdline_options.add(config).add(must);

        po::positional_options_description positional;
        positional.add("profile", 1);
        positional.add("document", 1);

        po::variables_map vm;
        store(po::command_line_parser(argc, argv).options(cmdline_options).positional(positional).run(), vm);
        po::notify(vm);    

        if(vm.count("help"))
        {
            cout << cmdline_options << endl;
            exit(1);
        }

        if(! vm.count("profile"))
        {
            cout << "error: no profile file is set." << endl;
            cout << cmdline_options << endl;
            exit(1);
        }

        if(! vm.count("document"))
        {
            cout << "error: no document file is set." << endl;
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
    cout << "profile: " << profileFile << endl;
    cout << "document: " << docFile << endl;
    cout << endl;

    Profile profile(ngram, length);
    bool result = profile.loadText(profileFile.c_str());
    if(! result)
    {
        cerr << "error in opening profile " << profileFile << endl;
        exit(1);
    }

    Profile document(ngram, length);
    result = document.generateFromFile(docFile.c_str());
    if(! result)
    {
        cerr << "error in generating profile from document " << docFile << endl;
        exit(1);
    }

    unsigned int distance = document.measureDistance(profile);
    cout << "distance: " << distance << endl;

    return 0;
}
