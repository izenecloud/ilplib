/**
 * @brief A character based analyzer
 * @author Wei
 * @date 2010.08.24
 */

#ifndef _LA_CHAR_ANALYZER_H_
#define _LA_CHAR_ANALYZER_H_

#include <la/analyzer/Analyzer.h>

namespace la
{

class CharAnalyzer : public Analyzer
{

public:

    CharAnalyzer();

    ~CharAnalyzer();

protected:

    virtual int analyze_impl( const Term& input, void* data, HookType func );

};

}


#endif
