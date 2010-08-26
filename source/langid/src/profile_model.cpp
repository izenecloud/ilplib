/** \file profile_model.cpp
 * Implementation of class ProfileModel.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 25, 2009
 */

#include "profile_model.h"
#include "langid/knowledge.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

#define LANGID_DEBUG_PRINT_LOAD 0
#define LANGID_DEBUG_PRINT_SELECT 0

NS_ILPLIB_LANGID_BEGIN

ProfileModel::ProfileModel()
    : ngramRange_(0), profileLength_(0)
{
}

void ProfileModel::clear()
{
    ngramRange_ = 0;
    profileLength_ = 0;

    profileVec_.clear();
    encodingIDVec_.clear();
}

bool ProfileModel::loadConfig(const char* fileName)
{
#if LANGID_DEBUG_PRINT_LOAD
    cout << ">>> ProfileModel::loadConfig()" << endl;
#endif

    if(! fileName)
    {
        cerr << "error: NULL string is passed as file name." << endl;
        return false;
    }

    ifstream ifs(fileName);
    if(! ifs)
    {
        cerr << "error: could not open file " << fileName << endl;
        return false;
    }

    clear();

    string line, path, name;
    istringstream iss;

    while(getline(ifs, line))
    {
        if(line.empty() || line[0] == '#')
            continue;

        iss.clear();
        iss.str(line);
        iss >> ngramRange_;
#if LANGID_DEBUG_PRINT_LOAD
        cout << "ngram: " << ngramRange_ << endl;
#endif
        break;
    }

    while(getline(ifs, line))
    {
        if(line.empty() || line[0] == '#')
            continue;

        iss.clear();
        iss.str(line);
        iss >> profileLength_;
#if LANGID_DEBUG_PRINT_LOAD
        cout << "profile length: " << profileLength_ << endl;
#endif
        break;
    }

    if(ngramRange_ <= 0 || profileLength_ <= 0)
    {
        cerr << "error: the ngram range or profile length is not configured correctly in file " << fileName << endl;
        return false;
    }

    while(getline(ifs, line))
    {
        if(line.empty() || line[0] == '#')
            continue;

        iss.clear();
        iss.str(line);
        iss >> path >> name;

#if LANGID_DEBUG_PRINT_LOAD
        cout << "name: " << name << ", path: " << path << endl;
#endif

        profileVec_.push_back(Profile(ngramRange_, profileLength_));
        if(! profileVec_.back().loadText(path.c_str()))
        {
            cerr << "error: fail to load profile, which path is in config file " << fileName << endl;
            return false;
        }

        EncodingID id = Knowledge::getEncodingIDFromName(name.c_str());
        if(id == ENCODING_ID_NUM)
        {
            cerr << "error: unknown encoding name " << name << " in config file " << fileName << endl;
            return false;
        }
        encodingIDVec_.push_back(id);
    }

    assert(profileVec_.size() == encodingIDVec_.size() && "the count of profiles and names should be equal.");
#if LANGID_DEBUG_PRINT_LOAD
    cout << profileVec_.size() << " profiles are loaded." << endl;
    cout << endl;
#endif

    if(profileVec_.empty())
    {
        cerr << "error: fail to load profile, for no path configured in file " << fileName << endl;
        return false;
    }

    return true;
}

bool ProfileModel::loadModel(const char* fileName)
{
#if LANGID_DEBUG_PRINT_LOAD
    cout << ">>> ProfileModel::loadModel()" << endl;
#endif

    if(! fileName)
    {
        cerr << "error: NULL string is passed as file name." << endl;
        return false;
    }

    ifstream ifs(fileName, ios::binary | ios_base::in);
    if(! ifs)
    {
        cerr << "error: could not open file " << fileName << endl;
        return false;
    }

    clear();

    if(! ifs.read(reinterpret_cast<char*>(&ngramRange_), sizeof(int)))
    {
        cerr << "error: invalid input stream in reading file " << fileName << endl;
        return false;
    }
    if(! ifs.read(reinterpret_cast<char*>(&profileLength_), sizeof(int)))
    {
        cerr << "error: invalid input stream in reading file " << fileName << endl;
        return false;
    }

    if(ngramRange_ <= 0 || profileLength_ <= 0)
    {
        cerr << "error: the ngram range or profile length should be positive in binary file " << fileName << endl;
        return false;
    }

    int num;
    if(! ifs.read(reinterpret_cast<char*>(&num), sizeof(int)))
    {
        cerr << "error: invalid input stream in reading file " << fileName << endl;
        return false;
    }

#if LANGID_DEBUG_PRINT_LOAD
    cout << "ngram range: " << ngramRange_ << endl;
    cout << "profile length: " << profileLength_ << endl;
    cout << "profile count: " << num << endl;
#endif

    if(num <= 0)
    {
        cerr << "error: the profile count " << num << " should be positive in binary file " << fileName << endl;
        return false;
    }

    EncodingID id;
    for(int i=0; i<num; ++i)
    {
        if(! ifs.read(reinterpret_cast<char*>(&id), sizeof(EncodingID)))
        {
            cerr << "error: invalid input stream in reading file " << fileName << endl;
            return false;
        }
        encodingIDVec_.push_back(id);

#if LANGID_DEBUG_PRINT_LOAD
        cout << "loading profile index: " << i << endl;
        cout << "encoding: " << Knowledge::getEncodingNameFromID(id) << endl;
#endif

        profileVec_.push_back(Profile(ngramRange_, profileLength_));
        if(! profileVec_.back().loadBinary(ifs))
        {
            cerr << "error: fail to load the profile in binary file " << fileName << endl;
            return false;
        }
    }

    assert(num == static_cast<int>(encodingIDVec_.size()) && "the number of profiles loaded should be equal to the count value in binary file.");
    assert(profileVec_.size() == encodingIDVec_.size() && "the count of profiles and names loaded should be equal.");
#if LANGID_DEBUG_PRINT_LOAD
    cout << profileVec_.size() << " profiles are loaded." << endl;
    cout << endl;
#endif

    if(profileVec_.empty())
    {
        cerr << "error: fail to load profile, for no profile is loaded in binary file " << fileName << endl;
        return false;
    }

    return true;
}

int ProfileModel::getNGramRange() const
{
    return ngramRange_;
}

int ProfileModel::getProfileLength() const
{
    return profileLength_;
}

bool ProfileModel::encodingFromProfile(const Profile& document, EncodingID& id) const
{
#if LANGID_DEBUG_PRINT_SELECT
    cout << ">>> ProfileModel::encodingFromProfile()" << endl;
#endif

    if(profileVec_.empty())
    {
        cerr << "error: fail to compare profiles, as no profile is loaded." << endl;
        return false;
    }

    unsigned int minDistance = document.measureDistance(profileVec_[0]);
    unsigned int minIndex = 0;
#if LANGID_DEBUG_PRINT_SELECT
    cout << Knowledge::getEncodingNameFromID(encodingIDVec_[0]) << ",\t";
    cout << "distance: " << minDistance << endl;
#endif

    for(unsigned int i=1; i<profileVec_.size(); ++i)
    {
        unsigned int d = document.measureDistance(profileVec_[i]);
#if LANGID_DEBUG_PRINT_SELECT
        cout << Knowledge::getEncodingNameFromID(encodingIDVec_[i]) << ",\t";
        cout << "distance: " << d << endl;
#endif
        if(d < minDistance)
        {
            minDistance = d;
            minIndex = i;
        }
    }

    id = encodingIDVec_[minIndex];
    return true;
}

bool ProfileModel::buildModel(const char* configFile, const char* modelFile)
{
    ProfileModel model;
    
    if(! model.loadConfig(configFile))
    {
        cerr << "error: load profile config file " << configFile << endl;
        return false;
    }

    if(! model.saveModel(modelFile))
    {
        cerr << "error: save profile model file " << modelFile << endl;
        return false;
    }

    return true;
}

/**
 * Format of binary model file.
 * int: ngram range
 * int: profile length
 * int: number of profiles
 * // for each profile
 * EncodingID: id
 * // profile
 * Profile::saveBinary(ofstream&)
 */
bool ProfileModel::saveModel(const char* fileName) const
{
    if(! fileName)
    {
        cerr << "error: NULL string is passed as file name." << endl;
        return false;
    }

    ofstream ofs(fileName, ios::binary | ios::out);
    if(! ofs)
    {
        cerr << "error: could not create file " << fileName << endl;
        return false;
    }

    ofs.write(reinterpret_cast<const char*>(&ngramRange_), sizeof(int));
    ofs.write(reinterpret_cast<const char*>(&profileLength_), sizeof(int));

    int num = profileVec_.size();
    ofs.write(reinterpret_cast<const char*>(&num), sizeof(int));

    for(int i=0; i<num; ++i)
    {
        ofs.write(reinterpret_cast<const char*>(&encodingIDVec_[i]), sizeof(EncodingID));

        if(! profileVec_[i].saveBinary(ofs))
        {
            cerr << "error: fail to save the profile to binary file " << fileName << endl;
            return false;
        }
    }

    return true;
}

NS_ILPLIB_LANGID_END
