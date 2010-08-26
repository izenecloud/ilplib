/** \file profile.cpp
 * Implementation of class Profile.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 04, 2009
 */

#include "profile.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <strstream>
#include <algorithm>
#include <cstdlib> // abs(int)
#include <cctype> // isspace()

using namespace std;

#define LANGID_DEBUG_PRINT_LOAD 0
#define LANGID_DEBUG_PRINT_NGRAM_SEQUENCE 0
#define LANGID_DEBUG_PRINT_CHAR_READ 0

#define LANGID_DEBUG_PRINT_NO_MATCH_COUNT 0
#define LANGID_DEBUG_PRINT_GENERATE 0

namespace
{
/** the character to replace a sequence of white-spaces or digits */
const char REPLACE_CHAR = '_';

/**
 * Check whether the character needs to replace.
 * \param ch the character to check
 * \return true for replace, false for not replace
 */
bool isReplaceChar(char ch)
{
    return isspace(ch) || isdigit(ch);
}
} // namespace

NS_ILPLIB_LANGID_BEGIN

Profile::Profile(int ngram, int length)
    : ngramRange_(ngram), profileLength_(length)
{
}

bool Profile::generateFromFile(const char* fileName, int maxInputSize)
{
#if LANGID_DEBUG_PRINT_GENERATE
    cout << ">>> Profile::generateFromFile(): " << fileName << endl;
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

    return generateFromStream(ifs, maxInputSize);
}

bool Profile::generateFromStr(const char* str, int maxInputSize)
{
#if LANGID_DEBUG_PRINT_GENERATE
    cout << ">>> Profile::generateFromStr(): " << str << endl;
#endif

    if(! str)
    {
        cerr << "error: NULL string is passed to Profile." << endl;
        return false;
    }

    istrstream iss(str);
    if(! iss)
    {
        cerr << "error: invalid istrstream constructed from string: " << str << endl;
        return false;
    }

    return generateFromStream(iss, maxInputSize);
}

unsigned int Profile::measureDistance(const Profile& profile) const
{
    unsigned int sum = 0;

    TMap::const_iterator profileEnd, profileIt;
    profileEnd = profile.rankMap_.end();
    const int noMatchDist = profile.profileLength_; // max out-of-place value when no match happens

#if LANGID_DEBUG_PRINT_NO_MATCH_COUNT
    int noMatchCount = 0;
#endif

    const int freqVecSize = freqVec_.size();
    for(int i=0; i<freqVecSize; ++i)
    {
        profileIt = profile.rankMap_.find(freqVec_[i].first);
        if(profileIt != profileEnd)
        {
            sum += abs(i - profileIt->second);
        }
        else
        {
            sum += noMatchDist;
#if LANGID_DEBUG_PRINT_NO_MATCH_COUNT
            ++noMatchCount;
#endif
        }
    }

#if LANGID_DEBUG_PRINT_NO_MATCH_COUNT
    cout << "no match count:\t" << noMatchCount << ",\t"; // << endl;
#endif

    return sum;
}

bool Profile::saveText(const char* fileName) const
{
    if(freqVec_.empty())
    {
        cerr << "error: Profile is empty, please generate or open it before save." << endl;
        return false;
    }
    assert(freqVec_.size() <= static_cast<unsigned int>(profileLength_) && "the number of N-grams to save should not exceed the profile length.");

    if(! fileName)
    {
        cerr << "error: NULL string is passed as file name." << endl;
        return false;
    }

    ofstream ofs(fileName);
    if(! ofs)
    {
        cerr << "error: could not create file " << fileName << endl;
        return false;
    }

    for(TVec::const_iterator it=freqVec_.begin(); it!=freqVec_.end(); ++it)
    {
        ofs << it->first << "\t" << it->second << endl;
    }

    return true;
}

bool Profile::loadText(const char* fileName)
{
    if(! validateParam())
    {
        cerr << "error: parameter of Profile is invalid." << endl;
        return false;
    }

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

    // remove previous result
    freqVec_.clear();
    rankMap_.clear();

    string str;
    int freq;
    for(int i=0; i<profileLength_ && ifs >> str >> freq; ++i)
    {
        freqVec_.push_back(TPair(str, freq));
        rankMap_[str] = i;
    }
    assert(freqVec_.size() <= static_cast<unsigned int>(profileLength_));
    assert(rankMap_.size() == freqVec_.size() && "the size of ranking map should be equal to frequency vector.");

#if LANGID_DEBUG_PRINT_LOAD
    cout << freqVec_.size() << " top ngrams loaded from " << fileName << endl;
#endif

    return true;
}

bool Profile::validateParam() const
{
    return (ngramRange_ > 0 && profileLength_ > 0);
}

bool Profile::generateFromStream(std::istream& is, int maxInputSize)
{
    if(! validateParam())
    {
        cerr << "error: parameter of Profile is invalid." << endl;
        return false;
    }

    char ch;
    bool lastReplace = false; // replace the sequence of white-spaces or digits into character '_'

#if LANGID_DEBUG_PRINT_NGRAM_SEQUENCE
    const char* ngramFile = "langid_ngrams.out";
    ofstream ofs(ngramFile);
    if(! ofs)
    {
        cerr << "error: fail to create file " << ngramFile << endl;
        return false;
    }
#endif

#if LANGID_DEBUG_PRINT_CHAR_READ
    const char* charFile = "langid_char_read.out";
    ofstream cfs(charFile);
    if(! cfs)
    {
        cerr << "error: fail to create file " << charFile << endl;
        return false;
    }
#endif

#if LANGID_DEBUG_PRINT_GENERATE
    int ngramCount = 0;
    cout << "Profile::generateFromStream(), maxInputSize: " << maxInputSize << endl;
#endif
    const bool isNoLimitSize = (maxInputSize <= 0);
    int charCount = 0;
    int charIndex = 0;

    vector<string> vec(ngramRange_); // current ngrams in tracking
    TMap freqMap; // map from ngram string to frequency

    // 1st: fill elements into vec
    while(charIndex < ngramRange_ && is.get(ch) && (isNoLimitSize || charCount < maxInputSize))
    {
#if LANGID_DEBUG_PRINT_CHAR_READ
        cfs.put(ch);
#endif
        ++charCount;

        if(isReplaceChar(ch))
        {
            if(lastReplace)
                continue;

            ch = REPLACE_CHAR;
            lastReplace = true;
        }
        else
        {
            lastReplace = false;
        }

        // append to previous results
        for(int i=0; i<charIndex; ++i)
        {
            vec[i] += ch;
            ++freqMap[vec[i]];
#if LANGID_DEBUG_PRINT_NGRAM_SEQUENCE
            ofs << vec[i] << endl;
#endif
#if LANGID_DEBUG_PRINT_GENERATE
            ++ngramCount;
#endif
        }

        vec[charIndex] = ch;
        ++freqMap[vec[charIndex]];
#if LANGID_DEBUG_PRINT_NGRAM_SEQUENCE
        ofs << vec[charIndex] << endl;
#endif
#if LANGID_DEBUG_PRINT_GENERATE
        ++ngramCount;
#endif
        ++charIndex;
    }

    // 2nd: modify the existing elements in vec
    int replaceIndex=0;
    while(is.get(ch) && (isNoLimitSize || charCount < maxInputSize))
    {
#if LANGID_DEBUG_PRINT_CHAR_READ
        cfs.put(ch);
#endif
        ++charCount;

        if(isReplaceChar(ch))
        {
            if(lastReplace)
                continue;

            ch = REPLACE_CHAR;
            lastReplace = true;
        }
        else
        {
            lastReplace = false;
        }

        for(int i=0; i<ngramRange_; ++i)
        {
            if(i == replaceIndex)
            {
                vec[i] = ch; // replace
            }
            else
            {
                vec[i] += ch; // append
            }

            ++freqMap[vec[i]];
#if LANGID_DEBUG_PRINT_NGRAM_SEQUENCE
            ofs << vec[i] << endl;
#endif
#if LANGID_DEBUG_PRINT_GENERATE
            ++ngramCount;
#endif
        }

        replaceIndex = (replaceIndex+1) % ngramRange_;
    }

#if LANGID_DEBUG_PRINT_NGRAM_SEQUENCE
    cout << "total ngrams: " << ngramCount << ", printed into " << ngramFile << endl;
#endif

#if LANGID_DEBUG_PRINT_GENERATE
    cout << "chars read: " << charCount << endl;
    cout << "total ngrams: " << ngramCount << endl;
    cout << "unique ngrams: " << freqMap.size() << endl;
#endif

    // extract top ngrams
    extractTopElement(freqMap, freqVec_, profileLength_);

    return true;
}

void Profile::extractTopElement(const TMap& srcMap, TVec& destVec, int top) const
{
    // remove previous ranking vector
    destVec.clear();

    // extract top profileLength_ ngrams into destVec, which is created as min-heap
    NGramCompare compare;
    for(TMap::const_iterator it=srcMap.begin(); it!=srcMap.end(); ++it)
    {
        if(destVec.size() < static_cast<unsigned int>(top))
        {
            destVec.push_back(*it);
            push_heap(destVec.begin(), destVec.end(), compare);
        }
        else
        {
            if(compare(*it, destVec.front()))
            {
                pop_heap(destVec.begin(), destVec.end(), compare);

                // assignment to the last element, which is equal to below:
                //destVec.pop_back();
                //destVec.push_back(*it);
                destVec.back() = *it;

                push_heap(destVec.begin(), destVec.end(), compare);
            }
        }
    }
    assert(destVec.size() == static_cast<unsigned int>(top) || (destVec.size() < static_cast<unsigned int>(top) && destVec.size() == srcMap.size()));
#if LANGID_DEBUG_PRINT_GENERATE
    cout << "top ngrams: " << destVec.size() << endl;
    cout << endl;
#endif

    // make destVec in sequence
    sort_heap(destVec.begin(), destVec.end(), compare);
}

/**
 * Format of binary file.
 * int: freqVec_.size()
 * // for each ngrams in freqVec_
 * char[ngramRange_]: ngrams string (0 terminated)
 */
bool Profile::saveBinary(std::ofstream& ofs) const
{
    if(! ofs)
    {
        cerr << "error: the output stream is invalid. " << endl;
        return false;
    }

    int num = freqVec_.size();
    ofs.write(reinterpret_cast<const char*>(&num), sizeof(int));

    for(TVec::const_iterator it=freqVec_.begin(); it!=freqVec_.end(); ++it)
    {
        const string& ngramStr = it->first;
        assert(static_cast<int>(ngramStr.size()) <= ngramRange_ && "ngram string is out of size.");
        
        ofs.write(ngramStr.c_str(), ngramStr.size() + 1);
    }

    return true;
}

bool Profile::loadBinary(std::ifstream& ifs)
{
#if LANGID_DEBUG_PRINT_LOAD
    cout << ">>> Profile::loadBinary()" << endl;
#endif

    if(! validateParam())
    {
        cerr << "error: parameter of Profile is invalid." << endl;
        return false;
    }

    if(! ifs)
    {
        cerr << "error: the input stream is invalid." << endl;
        return false;
    }

    int num;
    if(! ifs.read(reinterpret_cast<char*>(&num), sizeof(int)))
    {
        cerr << "error: invalid input stream in reading." << endl;
        return false;
    }

#if LANGID_DEBUG_PRINT_LOAD
    cout << "ngram count: " << num << endl;
#endif

    if(num <= 0)
    {
        cerr << "error: the ngram count " << num << " should be positive in binary profile." << endl;
        return false;
    }

    char ch;
    for(int i=0; i<num; ++i)
    {
        string ngramStr;
        while(ifs.get(ch) && ch)
            ngramStr += ch;

        if(ngramStr.empty())
        {
            cerr << "error: the " << i << "th ngram string loaded is empty in binary profile." << endl;
            return false;
        }

        rankMap_[ngramStr] = i;
#if LANGID_DEBUG_PRINT_LOAD
        cout << ngramStr << endl;
#endif
    }
    assert(static_cast<int>(rankMap_.size()) == num && "the size of ranking map should be equal to ngram count.");

#if LANGID_DEBUG_PRINT_LOAD
    cout << endl;
#endif

    return true;
}

NS_ILPLIB_LANGID_END
