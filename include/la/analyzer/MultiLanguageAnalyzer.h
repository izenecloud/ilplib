/**
 * @brief Analyzer for multi-languages
 * @file MultiLanguageAnalyzer.h
 *
 * @date Jan 4, 2010
 * @author vernkin
 */

/**
 * @brief Rewrite MLA using a fast language detction,
 *     String mode is moved to EnglishAnalyzer,
 *      Char mode is moved to CharAnalyzer.
 * @author Wei
 */

#ifndef MULTILANGUAGEANALYZER_H_
#define MULTILANGUAGEANALYZER_H_

#include <la/analyzer/Analyzer.h>
#include <la/stem/Stemmer.h>

#include <boost/shared_ptr.hpp>

namespace la
{

/**
 * @brief process several language at the same time
 */
class MultiLanguageAnalyzer: public Analyzer
{
public:
    enum Language
    {
        CHINESE,
        JAPANESE,
        KOREAN,
        ENGLISH,
        OTHER
    };

    MultiLanguageAnalyzer();

    /**
     * delete the all the analyzers that MultiLanguageAnalyzer holds
     */
    virtual ~MultiLanguageAnalyzer();

    /**
     * Set the Analyzer for specific language type
     * @param lang the specific language, see MultiLanguageAnalyzer::Language
     * @param analyzer associated analyzer. <b>WARNING</b>, this analyzer should be initialized
     * ( including configuring ) and won't be used in external program
     * @param return false if lang is invalid
     */
    void setAnalyzer( MultiLanguageAnalyzer::Language lang, boost::shared_ptr<Analyzer>& analyzer );

    boost::shared_ptr<Analyzer> getAnalyzer( MultiLanguageAnalyzer::Language lang ) const;

    void setDefaultAnalyzer( boost::shared_ptr<Analyzer>& defAnalyzer );

    boost::shared_ptr<Analyzer> getDefaultAnalyzer() const;

    Language getCharType( izenelib::util::UCS2Char ucs2Char );

    Language detectLanguage( const izenelib::util::UString & input );

protected:

    virtual int analyze_impl( const Term& input, void* data, HookType func );

private:
    /**
     * Default Analyzer
     */
    boost::shared_ptr<la::Analyzer> defAnalyzer_;

    /**
     * Analyzers for different languages
     */
    boost::shared_ptr<la::Analyzer> analyzers_[ OTHER ];
};


}
#endif /* MULTILANGUAGEANALYZER_H_ */
