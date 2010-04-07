/** \file factory.cpp
 * Implementation of class Factory.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Oct 30, 2009
 */

#include "langid/factory.h"
#include "analyzer_impl.h"
#include "knowledge_impl.h"

namespace langid
{

Factory* Factory::instance_;

Factory* Factory::instance()
{
    if(instance_ == 0)
    {
        instance_ = new Factory;
    }

    return instance_;
}

Analyzer* Factory::createAnalyzer()
{
    return new AnalyzerImpl;
}

Knowledge* Factory::createKnowledge()
{
    return new KnowledgeImpl;
}

Factory::Factory()
{
}

Factory::~Factory()
{
}

} // namespace langid
