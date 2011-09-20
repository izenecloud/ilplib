/** \file language_analyzer.cpp
 * Implementation of class LanguageAnalyzer.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 24, 2009
 */

#include "language_analyzer.h"
#include "script_table.h"
#include "langid/knowledge.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <strstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstring> // strlen

using namespace std;

#define LANGID_DEBUG_PRINT_SENTENCE 0
#define LANGID_DEBUG_PRINT_COUNT_STRING 0
#define LANGID_DEBUG_PRINT_COUNT_FILE 0
#define LANGID_DEBUG_PRINT_MULTIPLE_ID 0
#define LANGID_DEBUG_PRINT_ADD_REGION 0

NS_ILPLIB_LANGID_BEGIN

LanguageAnalyzer::LanguageAnalyzer(const ScriptTable& scriptTable, const SentenceBreakTable& breakTable)
    : scriptTable_(scriptTable), sentenceTokenizer_(breakTable),
    option_(0)
{
}

bool LanguageAnalyzer::primaryIDFromString(const char* str, LanguageID& id, int maxInputSize) const
{
    if(! str)
    {
        cerr << "error: null string is passed to LanguageAnalyzer::primaryidfromstring()." << endl;
        return false;
    }

    // count each language type
    vector<int> countVec(LANGUAGE_ID_NUM);

    countIDFromString(str, str+strlen(str), countVec, maxInputSize);

    id = getPrimaryID(countVec);
    return true;
}

bool LanguageAnalyzer::primaryIDFromFile(const char* fileName, LanguageID& id, int maxInputSize) const
{
    // count each language type
    vector<int> countVec(LANGUAGE_ID_NUM);

    if(! countIDFromFile(fileName, countVec, maxInputSize))
        return false;

    id = getPrimaryID(countVec);
    return true;
}

bool LanguageAnalyzer::multipleIDFromString(const char* str, std::vector<LanguageID>& idVec) const
{
    if(! str)
    {
        cerr << "error: NULL string is passed to LanguageAnalyzer::multipleIDFromString()." << endl;
        return false;
    }

    // count each language type
    vector<int> countVec(LANGUAGE_ID_NUM);

    countIDFromString(str, str+strlen(str), countVec);

    getMultipleID(countVec, idVec);
    return true;
}

bool LanguageAnalyzer::multipleIDFromFile(const char* fileName, std::vector<LanguageID>& idVec) const
{
    // count each language type
    vector<int> countVec(LANGUAGE_ID_NUM);

    if(! countIDFromFile(fileName, countVec))
        return false;

    getMultipleID(countVec, idVec);
    return true;
}

bool LanguageAnalyzer::segmentString(const char* str, std::vector<LanguageRegion>& regionVec) const
{
    if(! str)
    {
        cerr << "error: NULL string is passed to LanguageAnalyzer::segmentString()." << endl;
        return false;
    }

    // remove original result
    regionVec.clear();

    addLanguageRegion(str, str+strlen(str), regionVec);

    combineLanguageRegion(regionVec, getOptionBlockSizeThreshold());

    return true;
}

bool LanguageAnalyzer::segmentFile(const char* fileName, std::vector<LanguageRegion>& regionVec) const
{
    if(! fileName)
    {
        cerr << "error: NULL string is passed to LanguageAnalyzer::segmentFile()." << endl;
        return false;
    }

    ifstream ifs(fileName);
    if(! ifs)
    {
        cerr << "error: could not open file " << fileName << endl;
        return false;
    }

    // remove original result
    regionVec.clear();

    string line;
    while(getline(ifs, line))
    {
        if(! ifs.eof())
            line += '\n'; // include new line character for position accumulation

        addLanguageRegion(line.c_str(), line.c_str()+line.size(), regionVec);
    }

    combineLanguageRegion(regionVec, getOptionBlockSizeThreshold());

    return true;
}

void LanguageAnalyzer::addLanguageRegion(const char* begin, const char* end, std::vector<LanguageRegion>& regionVec) const
{
#if LANGID_DEBUG_PRINT_ADD_REGION
    cerr << ">>> LanguageAnalyzer::addLanguageRegion(): " << begin << endl;
#endif

    assert(begin && end);

    LanguageID previousID = LANGUAGE_ID_NUM;
    size_t pos = 0; // position in total string

    if(! regionVec.empty())
    {
        const LanguageRegion& lastRegion = regionVec.back();
        previousID = lastRegion.languageID_;
        pos = lastRegion.start_ + lastRegion.length_;
    }

    const char* p = begin;
    while(size_t len = sentenceTokenizer_.getSentenceLength(p, end))
    {
        // analyze each sentence
        LanguageID id = analyzeSentenceOnScriptPriority(p, p+len);
        assert(id != LANGUAGE_ID_NUM && "error: result in analyzing sentence is invalid.");

#if LANGID_DEBUG_PRINT_ADD_REGION 
        cerr << "language: " << Knowledge::getLanguageNameFromID(id) << endl;
#endif

        if(id == previousID
                || (id == LANGUAGE_ID_UNKNOWN && ! regionVec.empty()))
        {
            assert(regionVec.size() && "region vector should not be empty.");
            regionVec.back().length_ += len;
        }
        else if(previousID == LANGUAGE_ID_UNKNOWN)
        {
            assert(regionVec.size() && "region vector should not be empty.");
            LanguageRegion& lastRegion = regionVec.back();
            lastRegion.languageID_ = id;
            lastRegion.length_ += len;

            previousID = id;
        }
        else
        {
            LanguageRegion newRegion;
            newRegion.languageID_ = id;
            newRegion.start_ = pos;
            newRegion.length_ = len;
            regionVec.push_back(newRegion);

            previousID = id;
        }

        pos += len;
        p += len;
    }
}

LanguageID LanguageAnalyzer::analyzeSentenceOnScriptPriority(const char* begin, const char* end) const
{
    assert(begin && end);

    // check each script type
    vector<bool> flagVec(SCRIPT_TYPE_NUM, false);
    size_t mblen;
    for(const char* p=begin; p<end; p+=mblen)
    {
        unsigned short value = charTokenizer_.convertToUCS2(p, end, &mblen);
        ScriptType type = scriptTable_.getProperty(value);

        flagVec[type] = true;
    }


    LanguageID result = LANGUAGE_ID_UNKNOWN;
    // check script type for each language
    // with script priority in descending order (KR, JP, CT, CS, EN)
    if(flagVec[SCRIPT_TYPE_HANGUL])
    {
        result = LANGUAGE_ID_KOREAN;
    }
    else if(flagVec[SCRIPT_TYPE_KANA])
    {
        result = LANGUAGE_ID_JAPANESE;
    }
    else if(flagVec[SCRIPT_TYPE_TRADITIONAL])
    {
        if(isOptionNoChineseTraditional())
            result = LANGUAGE_ID_CHINESE_SIMPLIFIED;
        else
            result = LANGUAGE_ID_CHINESE_TRADITIONAL;
    }
    else if(flagVec[SCRIPT_TYPE_SIMPLIFIED])
    {
        result = LANGUAGE_ID_CHINESE_SIMPLIFIED;
    }
    else if(flagVec[SCRIPT_TYPE_ALPHABET])
    {
        result = LANGUAGE_ID_ENGLISH;
    }

#if LANGID_DEBUG_PRINT_SENTENCE
    // print the flags
    cout << "===== script types contained in the sentence =====" << endl;
    cout << dec;
    string propStr;
    for(int i=0; i<SCRIPT_TYPE_NUM; ++i)
    {
        if(flagVec[i])
        {
            if(! scriptTable_.propertyToStr(static_cast<ScriptType>(i), propStr))
            {
                cerr << "error: unknown property: " << i << endl;
                continue;
            }

            cout << "--- " << propStr << endl;
        }
    }
#endif

    return result;
}

LanguageID LanguageAnalyzer::analyzeSentenceOnScriptCount(const char* str)
{
    assert(str);

    const char* begin = str;
    const char* end = begin + strlen(str);

    // count each script type
    vector<int> countVec(SCRIPT_TYPE_NUM);
    size_t mblen;
    for(const char* p=begin; p!=end; p+=mblen)
    {
        unsigned short value = charTokenizer_.convertToUCS2(p, end, &mblen);
        ScriptType type = scriptTable_.getProperty(value);

        ++countVec[type];
    }

    // get the type of max count in ALPHABET, SIMPLIFIED, TRADITIONAL, KANA, HANGUL
    int k = SCRIPT_TYPE_ALPHABET;
    for(int i=k+1; i<=SCRIPT_TYPE_HANGUL; ++i)
    {
        if(countVec[i] >countVec[k])
            k = i;
    }
    ScriptType primary = static_cast<ScriptType>(k);

    LanguageID result = LANGUAGE_ID_UNKNOWN;
    // no CJK/E characters
    if(! countVec[primary])
    {
        result = LANGUAGE_ID_UNKNOWN;
    }
    // primary script type for English, Korean
    else if(primary == SCRIPT_TYPE_ALPHABET &&
            // in case of CJK contain alphabets
            countVec[SCRIPT_TYPE_ALPHABET] > 10 * (countVec[SCRIPT_TYPE_SIMPLIFIED] + countVec[SCRIPT_TYPE_TRADITIONAL] +
                                                 countVec[SCRIPT_TYPE_KANA] + countVec[SCRIPT_TYPE_HANGUL]))
    {
        result = LANGUAGE_ID_ENGLISH;
    }
    else if(primary == SCRIPT_TYPE_HANGUL ||
            // in case of Korean contain alphabets
            (primary == SCRIPT_TYPE_ALPHABET && countVec[SCRIPT_TYPE_ALPHABET] < 10 * countVec[SCRIPT_TYPE_HANGUL]))
    {
        result = LANGUAGE_ID_KOREAN;
    }
    // unique script type for Japanese, Chinese traditional, Chinese simplified
    else if(countVec[SCRIPT_TYPE_KANA])
    {
        result = LANGUAGE_ID_JAPANESE;
    }
    else if(countVec[SCRIPT_TYPE_TRADITIONAL])
    {
        if(isOptionNoChineseTraditional())
            result = LANGUAGE_ID_CHINESE_SIMPLIFIED;
        else
            result = LANGUAGE_ID_CHINESE_TRADITIONAL;
    }
    else if(countVec[SCRIPT_TYPE_SIMPLIFIED])
    {
        result = LANGUAGE_ID_CHINESE_SIMPLIFIED;
    }

#if LANGID_DEBUG_PRINT_SENTENCE
    // print the statistics finally
    cout << "===== statistics of script type =====" << endl;
    cout << dec;
    int total = 0;
    for(int i=0; i<SCRIPT_TYPE_NUM; ++i)
    {
        total += countVec[i];
    }

    cout << fixed << setprecision(2);
    string propStr;
    for(int i=0; i<SCRIPT_TYPE_NUM; ++i)
    {
        if(! scriptTable_.propertyToStr(static_cast<ScriptType>(i), propStr))
        {
            cerr << "error: unknown property: " << i << endl;
            continue;
        }

        cout << propStr << ":\t" << countVec[i] << "\t";
        if(total)
            cout << (double)countVec[i]/total*100 << "%" << endl;
        else
            cout << "0%" << endl;
    }
    if(scriptTable_.propertyToStr(primary, propStr))
    {
        cout << "primary: " << propStr << endl;
    }
    else
    {
        cerr << "error: unknown property: " << primary << endl;
    }
#endif

    return result;
}

void LanguageAnalyzer::countIDFromString(const char* begin, const char* end, std::vector<int>& countVec, int maxInputSize) const
{
    assert(begin && end);
    assert(countVec.size() == LANGUAGE_ID_NUM && "the count vector size should be the number of language types.");

#if LANGID_DEBUG_PRINT_COUNT_STRING
    int sentCount = 0;
    cout << "LanguageAnalyzer::countIDFromString(), maxInputSize: " << maxInputSize << endl;
#endif

    const bool isLimitSize = (maxInputSize > 0);
    int charCount = 0;

    const char* p = begin;
    while(size_t len = sentenceTokenizer_.getSentenceLength(p, end))
    {
        if(isLimitSize && charCount >= maxInputSize)
            break;

        // analyze each sentence
        LanguageID id = analyzeSentenceOnScriptPriority(p, p+len);
        assert(id != LANGUAGE_ID_NUM && "error: result in analyzing sentence is invalid.");

        ++countVec[id];

#if LANGID_DEBUG_PRINT_COUNT_STRING
        cout << "sentence " << sentCount++ << " ";
        cout << Knowledge::getLanguageNameFromID(id) << ": ";
#endif

        p += len;
        charCount += len;
    }
#if LANGID_DEBUG_PRINT_COUNT_STRING
    cout << "analyzed chars: " << charCount << endl;
    cout << "total sentence: " << sentCount << endl;
#endif

#if LANGID_DEBUG_PRINT_COUNT_STRING
    cout << "sentence count for each language type:" << endl;
    for(unsigned int i=0; i<countVec.size(); ++i)
    {
        if(countVec[i])
            cout << Knowledge::getLanguageNameFromID(static_cast<LanguageID>(i)) << ": " << countVec[i] << endl;
    }
    cout << endl;
#endif
}

bool LanguageAnalyzer::countIDFromFile(const char* fileName, std::vector<int>& countVec, int maxInputSize) const
{
#if LANGID_DEBUG_PRINT_COUNT_FILE
    cout << "LanguageAnalyzer::countIDFromFile(), maxInputSize: " << maxInputSize << endl;
#endif

    if(! fileName)
    {
        cerr << "error: NULL string is passed to LanguageAnalyzer::countIDFromFile()." << endl;
        return false;
    }

    ifstream ifs(fileName);
    if(! ifs)
    {
        cerr << "error: could not open file " << fileName << endl;
        return false;
    }

    assert(countVec.size() == LANGUAGE_ID_NUM && "the count vector size should be the number of language types.");

    string line;
    // no limit size
    if(maxInputSize <= 0)
    {
        while(getline(ifs, line))
        {
            countIDFromString(line.c_str(), line.c_str()+line.size(), countVec);
        }
    }
    else
    {
        while(getline(ifs, line) && maxInputSize > 0)
        {
            countIDFromString(line.c_str(), line.c_str()+line.size(), countVec, maxInputSize);

            maxInputSize -= line.size();
            if(! ifs.eof())
                --maxInputSize; // include new line character for limit accumulation

#if LANGID_DEBUG_PRINT_COUNT_FILE
            cout << "line length: " << line.size() << ", EOF: " << ifs.eof() << ", maxInputSize: " << maxInputSize << endl;
#endif
        }
    }

#if LANGID_DEBUG_PRINT_COUNT_FILE
    cout << "sentence count for each language type:" << endl;
    for(unsigned int i=0; i<countVec.size(); ++i)
    {
        if(countVec[i])
            cout << Knowledge::getLanguageNameFromID(static_cast<LanguageID>(i)) << ": " << countVec[i] << endl;
    }
    cout << endl;
#endif

    return true;
}

LanguageID LanguageAnalyzer::getPrimaryID(const std::vector<int>& countVec) const
{
    assert(countVec.size() == LANGUAGE_ID_NUM && "the count vector size should be the number of language types.");

    // get the type of max count in CJK/E, excluding UNKNOWN, which might be digits, punctuations, etc
    int k = LANGUAGE_ID_UNKNOWN + 1;
    for(int i=k+1; i<LANGUAGE_ID_NUM ; ++i)
    {
        // replace the previous max type even if their counts are equal,
        // so that EN, CS, CT, JP, KR are in ascending priority
        if(countVec[i] >= countVec[k])
            k = i;
    }

    LanguageID result = static_cast<LanguageID>(k);
    // no CJK/E characters
    if(! countVec[result])
    {
        result = LANGUAGE_ID_UNKNOWN;

        // no UNKNOWN characters, that is, no characters at all
        if(! countVec[LANGUAGE_ID_UNKNOWN])
        {
            cerr << "warning: empty string or file, the result is identified as " << Knowledge::getLanguageNameFromID(LANGUAGE_ID_UNKNOWN) << endl;
        }
    }

    return result;
}

void LanguageAnalyzer::getMultipleID(const std::vector<int>& countVec, std::vector<LanguageID>& idVec) const
{
    assert(countVec.size() == LANGUAGE_ID_NUM && "the count vector size should be the number of language types.");
    // remove original result
    idVec.clear();

    // sort the count in reverse order for CJK/E, excluding UNKNOWN, which might be digits, punctuations, etc
    for(int i=LANGUAGE_ID_UNKNOWN+1; i<LANGUAGE_ID_NUM; ++i)
    {
        if(countVec[i])
        {
            vector<LanguageID>::iterator it = idVec.begin();
            for(; it!=idVec.end(); ++it)
            {
                // insert before the type even if their counts are equal,
                // so that EN, CS, CT, JP, KR are in ascending priority
                if(countVec[i] >= countVec[*it])
                    break;
            }
            idVec.insert(it, static_cast<LanguageID>(i));
        }
    }
    // set as UNKNOWN if no CJK/E exists
    if(idVec.empty())
    {
        idVec.push_back(LANGUAGE_ID_UNKNOWN);

        // no UNKNOWN characters, that is, no characters at all
        if(! countVec[LANGUAGE_ID_UNKNOWN])
        {
            cerr << "warning: empty string or file, the result is identified as " << Knowledge::getLanguageNameFromID(LANGUAGE_ID_UNKNOWN) << endl;
        }
    }

#if LANGID_DEBUG_PRINT_MULTIPLE_ID
    cout << "multiple language types in reverse order of sentence count:" << endl;
    for(unsigned int i=0; i<idVec.size(); ++i)
    {
        cout << i << ": " << Knowledge::getLanguageNameFromID(idVec[i]) << endl;
    }
    cout << endl;
#endif
}

void LanguageAnalyzer::combineLanguageRegion(std::vector<LanguageRegion>& regionVec, std::size_t minSize) const
{
    if(minSize == 0) // disable combination
        return;

    typedef vector<LanguageRegion>::iterator RegionIter;
    RegionIter prevIter = regionVec.begin();
    RegionIter endIter = regionVec.end();

    for(RegionIter iter=prevIter+1; iter<endIter; ++iter)
    {
        if(iter->length_ <= minSize
            || prevIter->length_ <= minSize
            || prevIter->languageID_ == iter->languageID_)
        {
            if(prevIter->length_ < iter->length_)
                prevIter->languageID_ = iter->languageID_; // use language of larger size

            prevIter->length_ += iter->length_; // combine
        }
        else
        {
            ++prevIter;
            if(prevIter != iter)
                *prevIter = *iter; // move
        }
    }

    regionVec.erase(prevIter+1, endIter);
}

void LanguageAnalyzer::setOptionSrc(const Analyzer* src)
{
    option_ = src;
}

bool LanguageAnalyzer::isOptionNoChineseTraditional() const
{
    if(option_ && option_->getOption(Analyzer::OPTION_TYPE_NO_CHINESE_TRADITIONAL))
        return true;

    return false;
}

int LanguageAnalyzer::getOptionBlockSizeThreshold() const
{
    if (option_)
    {
        int value = option_->getOption(Analyzer::OPTION_TYPE_BLOCK_SIZE_THRESHOLD);
        if (value >= 0)
            return value;
    }

    return 0;
}

NS_ILPLIB_LANGID_END
