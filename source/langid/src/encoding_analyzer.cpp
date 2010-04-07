/** \file encoding_analyzer.cpp
 * Implementation of class EncodingAnalyzer.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 25, 2009
 */

#include "encoding_analyzer.h"
#include "profile_model.h"

#include <cassert>
#include <iostream>

using namespace std;

#ifndef LANGID_DEBUG_PRINT
    #define LANGID_DEBUG_PRINT 1
#endif

namespace langid
{

EncodingAnalyzer::EncodingAnalyzer(const ProfileModel& profileModel)
    : profileModel_(profileModel)
{
}

bool EncodingAnalyzer::encodingFromString(const char* str, EncodingID& id, int maxInputSize) const
{
    Profile p(profileModel_.getNGramRange(), profileModel_.getProfileLength());

    if(! p.generateFromStr(str, maxInputSize))
    {
        cerr << "error: fail to generate profile from string " << str << endl;
        return false;
    }

    return profileModel_.encodingFromProfile(p, id);
}

bool EncodingAnalyzer::encodingFromFile(const char* fileName, EncodingID& id, int maxInputSize) const
{
    Profile p(profileModel_.getNGramRange(), profileModel_.getProfileLength());

    if(! p.generateFromFile(fileName, maxInputSize))
    {
        cerr << "error: fail to generate profile from file " << fileName << endl;
        return false;
    }

    return profileModel_.encodingFromProfile(p, id);
}

} // namespace langid
