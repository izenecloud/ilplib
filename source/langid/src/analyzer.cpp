/** \file analyzer.cpp
 * Implementation of class Analyzer.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 24, 2009
 */

#include "langid/analyzer.h"

#include <cassert>

NS_ILPLIB_LANGID_BEGIN

Analyzer::Analyzer()
    : options_(OPTION_TYPE_NUM)
{
    options_[OPTION_TYPE_LIMIT_ANALYZE_SIZE] = 1024;
    options_[OPTION_TYPE_BLOCK_SIZE_THRESHOLD] = 0;
    options_[OPTION_TYPE_NO_CHINESE_TRADITIONAL] = 0;
}

Analyzer::~Analyzer()
{
}

void Analyzer::setOption(OptionType nOption, int nValue)
{
    options_[nOption] = nValue;
}

int Analyzer::getOption(OptionType nOption) const
{
    return options_[nOption];
}

NS_ILPLIB_LANGID_END
