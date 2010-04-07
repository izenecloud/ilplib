/** \file knowledge_impl.cpp
 * Implementation of class KnowledgeImpl.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 24, 2009
 */

#include "knowledge_impl.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;

#define LANGID_DEBUG_PRINT_LOAD 0

namespace langid
{

KnowledgeImpl::KnowledgeImpl()
{
}

KnowledgeImpl::~KnowledgeImpl()
{
}

bool KnowledgeImpl::loadEncodingModel(const char* fileName)
{
#if LANGID_DEBUG_PRINT_LOAD
    cout << ">>> KnowledgeImpl::loadEncodingModel(): " << fileName << endl;
#endif

    return profileModel_.loadModel(fileName);
}

bool KnowledgeImpl::loadLanguageModel(const char* fileName)
{
#if LANGID_DEBUG_PRINT_LOAD
    cout << ">>> KnowledgeImpl::loadLanguageModel(): " << fileName << endl;
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

    if(! scriptTable_.loadBinary(ifs))
    {
        cerr << "error: fail to load binary script content in file " << fileName << endl;
        return false;
    }

    if(! sentenceTable_.loadBinary(ifs))
    {
        cerr << "error: fail to load binary sentence break content in file " << fileName << endl;
        return false;
    }

    return true;
}

const SentenceBreakTable& KnowledgeImpl::getSentenceBreakTable() const
{
    return sentenceTable_;
}

const ScriptTable& KnowledgeImpl::getScriptTable() const
{
    return scriptTable_;
}

const ProfileModel& KnowledgeImpl::getProfileModel() const
{
    return profileModel_;
}

} // namespace langid
