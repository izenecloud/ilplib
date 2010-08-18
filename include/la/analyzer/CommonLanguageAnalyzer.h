/**
 * @file    CommonLanguageAnalyzer.h
 * @author  Kent, Vernkin
 * @date    Nov 23, 2009
 * @details
 *  Common Language Analyzer for Chinese/Japanese/Korean.
 */

#ifndef COMMONLANGUAGEANALYZER_H_
#define COMMONLANGUAGEANALYZER_H_

#include <la/analyzer/Analyzer.h>
#include <la/dict/SingletonDictionary.h>
#include <la/dict/UpdatableSynonymContainer.h>
#include <la/dict/UpdateDictThread.h>
#include <am/vsynonym/VSynonym.h>

#include <la/stem/Stemmer.h>

#include <util/ustring/UString.h>

namespace la
{

class CommonLanguageAnalyzer : public Analyzer
{
public:

    CommonLanguageAnalyzer( const std::string knowledgePath, bool loadModel = true );

    ~CommonLanguageAnalyzer();

    virtual void setIndexMode() {};

    virtual void setLabelMode() {};

    virtual void setNBest(unsigned int nbest = 2) {};

    virtual void setAnalysisType( unsigned int type = 2 ) {};

    virtual void setCaseSensitive(bool casesensitive = true, bool containlower = true)
    {
        bCaseSensitive_ = casesensitive;
        bContainLower_ = containlower;
    };

    virtual inline void setExtractEngStem( bool extractEngStem = true )
    {
        bExtractEngStem_ = extractEngStem;
    }

    virtual inline void setExtractSynonym( bool extractSynonym = true)
    {
        bExtractSynonym_ = extractSynonym;
    }

    void setSynonymUpdateInterval(unsigned int seconds);

protected:

    /// Parse given input
    virtual void parse(const char* sentence, int initoffset) = 0;

    /// Fill token_, len_, offset_, morpheme_
    virtual bool nextToken() = 0;

    /// whether morpheme_ indicates foreign language
    virtual bool isFL() = 0;

    /// whether morpheme_ indicates special character, e.g. punctuations
    virtual bool isSpecialChar() = 0;

    inline const char* token()
    {
        return token_;
    }
    inline int len()
    {
        return len_;
    }
    inline int morpheme()
    {
        return morpheme_;
    }
    inline int offset()
    {
        return offset_;
    }
    bool needIndex()
    {
        return needIndex_;
    }

protected:

    IMPLEMENT_ANALYZER_METHODS

    int analyze_(const Term & input,
                 TermList & output,
                 analyzermode flags);

    template <typename IDManagerType>
    int analyze_(IDManagerType* idm,
                 const Term & input,
                 TermIdList & output,
                 analyzermode flags);

    typedef void (*HookType) ( void* data, const UString::CharT* text, const size_t len, const int offset );

    template<typename IDManagerType>
    static void appendTermIdList( void* data, const UString::CharT* text, const size_t len, const int offset );

    static void appendTermList( void* data, const UString::CharT* text, const size_t len, const int offset );

    int analyze_impl( const Term& input, analyzermode flags, void* data, HookType func );

protected:

    izenelib::am::VSynonymContainer*      pSynonymContainer_;
    izenelib::am::VSynonym*               pSynonymResult_;
    shared_ptr<UpdatableSynonymContainer> uscSPtr_;

    stem::Stemmer *                     pStemmer_;

    static const size_t input_string_buffer_size_ = 4096*3;
    char * input_string_buffer_;
    char * input_lowercase_string_buffer_;

    static const size_t output_ustring_buffer_size_ = 4096;
    UString::CharT * output_ustring_buffer_;
    UString::CharT * output_lowercase_ustring_buffer_;
    UString::CharT * output_synonym_ustring_buffer_;
    UString::CharT * output_stemming_ustring_buffer_;

    const char * token_;
    int len_;
    int offset_;
    int morpheme_;
    bool needIndex_;

    izenelib::util::UString::EncodingType encode_;

    bool bCaseSensitive_;

    bool bContainLower_;

    bool bExtractEngStem_;

    bool bExtractSynonym_;
};

}

#endif /* COMMONLANGUAGEANALYZER_H_ */
