#ifndef _CHINESE_ANALYZER_H_
#define _CHINESE_ANALYZER_H_

#include <la/analyzer/CommonLanguageAnalyzer.h>
#include <la/analyzer/ChineseLanguageAction.h>

namespace la
{
    class ChineseAnalyzer : public CommonLanguageAnalyzer<ChineseLanguageAction, cma::Sentence>
    {
    };
}

#endif
