/**
 * @brief Analyzer for multi-languages
 * @file MultiLanguageAnalyzer.h
 *
 * @date Jan 4, 2010
 * @author vernkin
 */

#ifndef MULTILANGUAGEANALYZER_H_
#define MULTILANGUAGEANALYZER_H_

#include <la/Analyzer.h>
#include <la/stem/Stemmer.h>

#include <boost/shared_ptr.hpp>


//#define DEBUG_MLA

namespace la
{

/**
 * @brief process several language at the same time
 */
class MultiLanguageAnalyzer: public la::Analyzer
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

    /**
     * @brief How to process specific language
     */
    enum ProcessMode
    {
        MA_PM, ///< Processing with associated MA
        CHARACTER_PM, ///< Divide the string into each independent characters
        STRING_PM, ///< Do noting on the string
        NONE_PM, ///< Do nothing
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
    bool setAnalyzer( MultiLanguageAnalyzer::Language lang, boost::shared_ptr<Analyzer>& analyzer );

    boost::shared_ptr<Analyzer> getAnalyzer( MultiLanguageAnalyzer::Language lang ) const;

    void setDefaultAnalyzer( boost::shared_ptr<Analyzer>& defAnalyzer );

    boost::shared_ptr<Analyzer> getDefaultAnalyzer() const;

    /**
     * Set the Processing mode for specific language
     * @param lang the specific language, see MultiLanguageAnalyzer::Language
     * @param mode the processing mode, see MultiLanguageAnalyzer::ProcessMode
     * @param return false if lang is invalid
     */
    bool setProcessMode( MultiLanguageAnalyzer::Language lang, MultiLanguageAnalyzer::ProcessMode mode );

    virtual int analyze_index( const TermList & input, TermList & output, unsigned char retFlag );
    virtual int analyze_search( const TermList & input, TermList & output, unsigned char retFlag );

    /**
     * @brief Whether enable case-sensitive search, this method only
     * set the caseSensitive flag. The children class can overwirte
     * this method.
     * @param flag default value is true
     */
    virtual void setCaseSensitive( bool flag );

    /**
     * @brief Whether contain lower form of English
     * set the containLower flag. The children class can overwirte
     * this method.
     * @param flag default value is true
     */
    virtual void setContainLower( bool flag );

    /**
     * @brief   Whether or not to extract English stems.
     */
    inline void setExtractEngStem( bool flag )
    {
        bExtractEngStem_ = flag;
    }

private:
    inline Language getCharType( wiselib::UCS2Char ucs2Char );

    inline void invokeMA( const wiselib::UString& ustr, TermList & output, bool isIndex, Language lang,
            unsigned int woffset, unsigned int &listOffset, bool isEnd );

    inline void performAnalyze( const TermList & input, TermList & output, bool isIndex );

    void printMLA();

    /**
     * Initail all setting in the inner analyzer
     */
    void initAnalyzerSetting( boost::shared_ptr<Analyzer>& analyzer );

private:
    /**
     * Default Analyzer
     */
    boost::shared_ptr<la::Analyzer> defAnalyzer_;

    /**
     * Analyzers for different languages
     */
    boost::shared_ptr<la::Analyzer> analyzers_[ OTHER ];

    /**
     * Processing mode for different languages, default is NONE
     */
    ProcessMode modes_[ OTHER ];

    /**
     * Global new Term
     */
    const Term newTerm_;

    /**
     * To extract English stems. Default: true
     */
    bool bExtractEngStem_;

    stem::Stemmer*  pStemmer_;
};

}
#endif /* MULTILANGUAGEANALYZER_H_ */
