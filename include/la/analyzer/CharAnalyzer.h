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

    /**
     * Whether separate all flag. It true, digits
     * and letter are separated. Default is true.
     * \param flag new status to set
     */
    void setSeparateAll( bool flag = true );

    void setCaseSensitive( bool caseSensitive = false)
    {
        isCaseSensitive = caseSensitive;
    }

protected:

    virtual int analyze_impl( const Term& input, void* data, HookType func );

private:

    /**
     * Letters and Digits are NOT separated
     */
    int separate_part( const Term& input, void* data, HookType func );

    /**
     * Letters and Digits are separated
     */
    int separate_all( const Term& input, void* data, HookType func );

private:
    bool isSeparateAll;
    bool isCaseSensitive;

    UString::CharT tmpCh_;
    UString tmpStr_;
};

}


#endif
