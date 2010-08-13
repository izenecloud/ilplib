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

    enum mode {indexmode, labelmode} ;

    CommonLanguageAnalyzer( const std::string knowledgePath, bool loadModel = true );

    ~CommonLanguageAnalyzer();

    virtual void setIndexMode() = 0;

    virtual void setLabelMode() = 0;

    virtual void setNBest(unsigned int nbest = 2) {};

    virtual void setAnalysisType( unsigned int type = 2 ) {};

    virtual void setCaseSensitive(bool casesensitive = true, bool containlower = true)
    {
        bCaseSensitive_ = casesensitive;
        bContainLower_ = containlower;
    };

    DECLARE_ANALYZER_METHODS

protected:

    /// Parse given input
    virtual void parse(const char* sentence, int initoffset) = 0;

    /// Fill token_, len_, offset_, morpheme_
    virtual bool nextToken() = 0;

    /// whether morpheme_ indicates foreign language
    virtual bool isFL() = 0;

    inline const char* token() { return token_; }
    inline int len() { return len_; }
    inline int morpheme() { return morpheme_; }
    inline int offset() { return offset_; }
    bool needIndex() { return needIndex_; }

    template <typename IDManagerType>
    int analyze(IDManagerType* idm,
            const Term & input,
            TermIdList & output,
            analyzermode flags);

protected:

    izenelib::am::VSynonymContainer*      pSynonymContainer_;
    izenelib::am::VSynonym*               pSynonymResult_;
    shared_ptr<UpdatableSynonymContainer> uscSPtr_;

    stem::Stemmer *                     pStemmer_;

    char * ustring_convert_buffer1_;
    char * ustring_convert_buffer_;

    const char * token_;
    int len_;
    int offset_;
    int morpheme_;
    bool needIndex_;

    izenelib::util::UString::EncodingType encode_;

    bool bCaseSensitive_;

    bool bContainLower_;
};

}

#endif /* COMMONLANGUAGEANALYZER_H_ */
