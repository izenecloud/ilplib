/** \file profile_selector.cpp
 * Implementation of class ProfileSelector.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 10, 2009
 */

#include "profile_selector.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

#define LANGID_DEBUG_PRINT_LOAD 0
#define LANGID_DEBUG_PRINT_SELECT 0

NS_ILPLIB_LANGID_BEGIN

ProfileSelector::ProfileSelector(int ngram, int length)
    : ngramRange_(ngram), profileLength_(length)
{
}

bool ProfileSelector::loadProfiles(const char* configFile)
{
#if LANGID_DEBUG_PRINT_LOAD
    cout << ">>> ProfileSelector::loadProfiles()" << endl;
#endif

    if(! configFile)
    {
        cerr << "error: NULL string is passed as file name." << endl;
        return false;
    }

    ifstream ifs(configFile);
    if(! ifs)
    {
        cerr << "error: could not open file " << configFile << endl;
        return false;
    }

    string line, path, name;
    istringstream iss;
    while(getline(ifs, line))
    {
        if(line.empty() || line[0] == '#')
        {
            continue;
        }

        iss.clear();
        iss.str(line);
        iss >> path >> name;

#if LANGID_DEBUG_PRINT_LOAD
        cout << "name: " << name << ", path: " << path << endl;
#endif

        profileNames_.push_back(name);
        profiles_.push_back(Profile(ngramRange_, profileLength_));
        if(! profiles_.back().loadText(path.c_str()))
        {
            cerr << "error: fail to load profile, which path is in config file " << configFile << endl;
            return false;
        }
    }

    assert(profiles_.size() == profileNames_.size() && "the count of profiles and names should be equal.");
#if LANGID_DEBUG_PRINT_LOAD
    cout << profiles_.size() << " profiles are loaded." << endl;
    cout << endl;
#endif

    if(profiles_.empty())
    {
        cerr << "error: fail to load profile, for no path configured in file " << configFile << endl;
        return false;
    }

    return true;
}

const char* ProfileSelector::selectProfileFromFile(const char* fileName) const
{
    Profile p(ngramRange_, profileLength_);

    if(! p.generateFromFile(fileName))
    {
        cerr << "error: fail to generate profile from file " << fileName << endl;
        return 0;
    }

    return selectProfile(p);
}

const char* ProfileSelector::selectProfileFromStr(const char* str) const
{
    Profile p(ngramRange_, profileLength_);

    if(! p.generateFromStr(str))
    {
        cerr << "error: fail to generate profile from string." << endl;
        return 0;
    }

    return selectProfile(p);
}

const char* ProfileSelector::selectProfile(const Profile& document) const
{
#if LANGID_DEBUG_PRINT_SELECT
    cout << ">>> ProfileSelector::selectProfile()" << endl;
#endif

    if(profiles_.empty())
    {
        cerr << "no profile is loaded, which paths are in configuration file." << endl;
        return 0;
    }

#if LANGID_DEBUG_PRINT_SELECT
    cout << profileNames_[0] << ",\t";
#endif
    unsigned int minDistance = document.measureDistance(profiles_[0]);
    unsigned int minIndex = 0;
#if LANGID_DEBUG_PRINT_SELECT
    //cout << profileNames_[0] << ":\t" << minDistance << endl;
    cout << "distance: " << minDistance << endl;
#endif
    for(unsigned int i=1; i<profiles_.size(); ++i)
    {
#if LANGID_DEBUG_PRINT_SELECT
        cout << profileNames_[i] << ",\t";
#endif
        unsigned int d = document.measureDistance(profiles_[i]);
#if LANGID_DEBUG_PRINT_SELECT
        //cout << profileNames_[i] << ":\t" << d << endl;
        cout << "distance: " << d << endl;
#endif
        if(d < minDistance)
        {
            minDistance = d;
            minIndex = i;
        }
    }

    return profileNames_[minIndex].c_str();
}

NS_ILPLIB_LANGID_END
