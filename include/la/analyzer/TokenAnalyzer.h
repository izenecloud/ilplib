/**
 * @brief A tokenizer based analyzer
 * @author Wei
 * @date 2010.08.24
 */

#ifndef _LA_TOKEN_ANALYZER_H_
#define _LA_TOKEN_ANALYZER_H_

#include <la/analyzer/Analyzer.h>

namespace la
{

class TokenAnalyzer : public Analyzer
{

public:

    TokenAnalyzer();

    ~TokenAnalyzer();

protected:

    virtual int analyze_impl( const Term& input, void* data, HookType func );

};

}


#endif
