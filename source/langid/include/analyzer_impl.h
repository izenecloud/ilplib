/** \file analyzer_impl.h
 * Definition of class AnalyzerImpl.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 24, 2009
 */

#ifndef LANGID_ANALYZER_IMPL_H
#define LANGID_ANALYZER_IMPL_H

#include "langid/analyzer.h"

NS_ILPLIB_LANGID_BEGIN

class KnowledgeImpl;
class SentenceTokenizer;
class LanguageAnalyzer;
class EncodingAnalyzer;

/**
 * AnalyzerImpl is the implementation class for Analyzer.
 */
class AnalyzerImpl : public Analyzer
{
public:
    /**
     * Constructor.
     */
    AnalyzerImpl();

    /**
     * Destructor.
     */
    virtual ~AnalyzerImpl();

    /**
     * Set the \e Knowledge for analysis.
     * \param pKnowledge the pointer of \e Knowledge
     */
    virtual void setKnowledge(Knowledge* pKnowledge);

    /**
     * Identify the character encoding of \e str.
     * \param[in] str string
     * \param[out] id the character encoding as identification result
     * \return true for success, false for failure
     */
    virtual bool encodingFromString(const char* str, EncodingID& id);

    /**
     * Identify the character encoding of file \e fileName.
     * \param[in] fileName file name
     * \param[out] id the character encoding as identification result
     * \return true for success, false for failure
     */
    virtual bool encodingFromFile(const char* fileName, EncodingID& id);

    /**
     * Get the length of the first sentence starting from \e str in UTF-8 encoding.
     * \param[in] str string in UTF-8 encoding
     * \return the length in bytes of the first sentence starting from \e str,
     * 0 is returned if there is no sentence left, that is, when \e *str is null.
     */
    virtual std::size_t sentenceLength(const char* str);

    /**
     * Get the length of the first sentence starting from character \e ustr[pos].
     * \param[in] ustr UString instance
     * \param[in] pos position of a character in \e ustr to be used as starting character for the sentence
     * \return the count of characters in the first sentence starting from character \e ustr[pos],
     * 0 is returned if there is no sentence left, that is, when \e pos >= \e ustr.length().
     */
    virtual std::size_t sentenceLength(const izenelib::util::UString& ustr, std::size_t pos = 0);

    /**
     * Identify the single primary language type contained in \e str in UTF-8 encoding.
     * \param[in] str string in UTF-8 encoding
     * \param[out] id the single primary language type as identification result
     * \return true for success, false for failure
     */
    virtual bool languageFromString(const char* str, LanguageID& id);

    /**
     * Identify the single primary language type contained in \e ustr.
     * \param[in] ustr UString instance
     * \param[out] id the single primary language type as identification result
     * \return true for success, false for failure
     */
    virtual bool languageFromString(const izenelib::util::UString& ustr, LanguageID& id);

    /**
     * Identify the single primary language type contained in file \e fileName in UTF-8 encoding.
     * \param[in] fileName file name, which content is in UTF-8 encoding
     * \param[out] id the single primary language type as identification result
     * \return true for success, false for failure
     */
    virtual bool languageFromFile(const char* fileName, LanguageID& id);

    /**
     * Identify the list of multiple language types contained in \e str in UTF-8 encoding.
     * \param[in] str string in UTF-8 encoding
     * \param[out] idVec the list of multiple language types as identification result,
     * the items in \e idVec are sorted by its sentence count in descending order,
     * that is, <em> idVec[0] </em> would be the primary language type, and <em> idVec[1] </em> the next primary type, etc.
     * \return true for success, false for failure
     * \attention the original data in \e idVec would be removed.
     */
    virtual bool languageListFromString(const char* str, std::vector<LanguageID>& idVec);

    /**
     * Identify the list of multiple language types contained in \e ustr.
     * \param[in] ustr UString instance
     * \param[out] idVec the list of multiple language types as identification result,
     * the items in \e idVec are sorted by its sentence count in descending order,
     * that is, <em> idVec[0] </em> would be the primary language type, and <em> idVec[1] </em> the next primary type, etc.
     * \return true for success, false for failure
     * \attention the original data in \e idVec would be removed.
     */
    virtual bool languageListFromString(const izenelib::util::UString& ustr, std::vector<LanguageID>& idVec);

    /**
     * Identify the list of multiple language types contained in file \e fileName in UTF-8 encoding.
     * \param[in] fileName file name, which content is in UTF-8 encoding
     * \param[out] idVec the list of multiple language types as identification result,
     * the items in \e idVec are sorted by its sentence count in descending order,
     * that is, <em> idVec[0] </em> would be the primary language type, and <em> idVec[1] </em> the next primary type, etc.
     * \return true for success, false for failure
     * \attention the original data in \e idVec would be removed.
     */
    virtual bool languageListFromFile(const char* fileName, std::vector<LanguageID>& idVec);

    /**
     * Segment the UTF-8 multi-lingual string into single-language regions.
     * \param[in] str string in UTF-8 encoding
     * \param[out] regionVec region results, each region is in a single language type
     * \return true for success, false for failure
     * \note in \p regionVec, the count unit for \c LanguageRegion::start_ and \c LanguageRegion::length_ is byte.
     * \attention the original data in \e regionVec would be removed.
     */
    virtual bool segmentString(const char* str, std::vector<LanguageRegion>& regionVec);

    /**
     * Segment the multi-lingual UString into single-language regions.
     * \param[in] ustr UString instance
     * \param[out] regionVec region results, each region is in a single language type
     * \return true for success, false for failure
     * \note in \p regionVec, the count unit for \c LanguageRegion::start_ and \c LanguageRegion::length_ is character,
     *       that is, for UString, we use character count instead of byte count for region result.
     * \attention the original data in \e regionVec would be removed.
     */
    virtual bool segmentString(const izenelib::util::UString& ustr, std::vector<LanguageRegion>& regionVec);

    /**
     * Segment the UTF-8 multi-lingual document into single-language regions.
     * \param[in] fileName file name, which content is in UTF-8 encoding
     * \param[out] regionVec region results, each region is in a single language type
     * \return true for success, false for failure
     * \note in \p regionVec, the count unit for \c LanguageRegion::start_ and \c LanguageRegion::length_ is byte.
     * \attention the original data in \e regionVec would be removed.
     */
    virtual bool segmentFile(const char* fileName, std::vector<LanguageRegion>& regionVec);

private:
    /**
     * Release the resources owned by \e AnalyzerImpl itself.
     */
    void clear();

private:
    /** pointer to knowledge */
    KnowledgeImpl* knowledge_;

    /** sentence tokenizer */
    SentenceTokenizer* sentenceTokenizer_;

    /** language analyzer */
    LanguageAnalyzer* languageAnalyzer_;

    /** encoding analyzer */
    EncodingAnalyzer* encodingAnalyzer_;
};

NS_ILPLIB_LANGID_END

#endif // LANGID_ANALYZER_IMPL_H
