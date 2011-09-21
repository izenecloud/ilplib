/** \file language_analyzer.h
 * Definition of class LanguageAnalyzer.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 24, 2009
 */

#ifndef LANGID_LANGUAGE_ANALYZER_H
#define LANGID_LANGUAGE_ANALYZER_H

#include "langid/language_id.h"
#include "langid/analyzer.h"

#include "script_table.h"
#include "sentence_tokenizer.h"
#include <util/ustring/UString.h>

#include <vector>

NS_ILPLIB_LANGID_BEGIN

/**
 * LanguageAnalyzer identifies the language type for UTF-8 encoding.
 */
class LanguageAnalyzer
{
public:
    /**
     * Constructor.
     * \param scriptTable script type table
     * \param breakTable sentence break table used to determine sentence boundary
     */
    LanguageAnalyzer(const ScriptTable& scriptTable, const SentenceBreakTable& breakTable);

    /**
     * Identify the single primary language type contained in \e str in UTF-8 encoding.
     * \param[in] str string in UTF-8 encoding
     * \param[out] id the single primary language type as identification result
     * \param[in] maxInputSize maximum analysis size in bytes, the entire string would be analyzed if a non-positive value is given.
     * \return true for success, false for failure
     */
    bool primaryIDFromString(const char* str, LanguageID& id, int maxInputSize = 0) const;

    /**
     * Identify the single primary language type contained in \e ustr.
     * \param[in] ustr UString instance
     * \param[out] id the single primary language type as identification result
     * \param[in] maxInputSize maximum analysis size in bytes, the entire string would be analyzed if a non-positive value is given.
     * \return true for success, false for failure
     */
    bool primaryIDFromString(const izenelib::util::UString& ustr, LanguageID& id, int maxInputSize = 0) const;

    /**
     * Identify the single primary language type contained in \e fileName in UTF-8 encoding.
     * \param[in] fileName file name, which content is in UTF-8 encoding
     * \param[out] id the single primary language type as identification result
     * \param[in] maxInputSize maximum analysis size in bytes, the entire file would be analyzed if a non-positive value is given.
     * \return true for success, false for failure
     */
    bool primaryIDFromFile(const char* fileName, LanguageID& id, int maxInputSize = 0) const;

    /**
     * Identify the list of multiple language types contained in \e str in UTF-8 encoding.
     * \param[in] str string in UTF-8 encoding
     * \param[out] idVec the list of multiple language types as identification result,
     * the items in \e idVec are sorted by its sentence count in descending order,
     * that is, <em> idVec[0] </em> would be the primary language type, and <em> idVec[1] </em> the next primary type, etc.
     * \return true for success, false for failure
     * \attention the original data in \e idVec would be removed.
     */
    bool multipleIDFromString(const char* str, std::vector<LanguageID>& idVec) const;

    /**
     * Identify the list of multiple language types contained in \e ustr.
     * \param[in] ustr UString instance
     * \param[out] idVec the list of multiple language types as identification result,
     * the items in \e idVec are sorted by its sentence count in descending order,
     * that is, <em> idVec[0] </em> would be the primary language type, and <em> idVec[1] </em> the next primary type, etc.
     * \return true for success, false for failure
     * \attention the original data in \e idVec would be removed.
     */
    bool multipleIDFromString(const izenelib::util::UString& ustr, std::vector<LanguageID>& idVec) const;

    /**
     * Identify the list of multiple language types contained in \e fileName in UTF-8 encoding.
     * \param[in] fileName file name, which content is in UTF-8 encoding
     * \param[out] idVec the list of multiple language types as identification result,
     * the items in \e idVec are sorted by its sentence count in descending order,
     * that is, <em> idVec[0] </em> would be the primary language type, and <em> idVec[1] </em> the next primary type, etc.
     * \return true for success, false for failure
     * \attention the original data in \e idVec would be removed.
     */
    bool multipleIDFromFile(const char* fileName, std::vector<LanguageID>& idVec) const;

    /**
     * Segment the UTF-8 multi-lingual string into single-language regions.
     * \param[in] str string in UTF-8 encoding
     * \param[out] regionVec region results, each region is in a single language type
     * \return true for success, false for failure
     * \attention the original data in \e regionVec would be removed.
     */
    bool segmentString(const char* str, std::vector<LanguageRegion>& regionVec) const;

    /**
     * Segment the multi-lingual UString into single-language regions.
     * \param[in] ustr UString instance
     * \param[out] regionVec region results, each region is in a single language type
     * \return true for success, false for failure
     * \attention the original data in \e regionVec would be removed.
     */
    bool segmentString(const izenelib::util::UString& ustr, std::vector<LanguageRegion>& regionVec) const;

    /**
     * Segment the UTF-8 multi-lingual document into single-language regions.
     * \param[in] fileName file name, which content is in UTF-8 encoding
     * \param[out] regionVec region results, each region is in a single language type
     * \return true for success, false for failure
     * \attention the original data in \e regionVec would be removed.
     */
    bool segmentFile(const char* fileName, std::vector<LanguageRegion>& regionVec) const;

    /**
     * Set the option supplier.
     * \param src the option supplier
     */
    void setOptionSrc(const Analyzer* src);

private:
    /**
     * Identify the single primary language type for sentence, using script priority in descending order (KR, JP, CT, CS, EN).
     * \param begin start of sentence string in \p encoding
     * \param end end of sentence string in \p encoding
     * \return the single primary language type
     */
    template<EncodingID encoding>
    LanguageID analyzeSentenceOnScriptPriority(const char* begin, const char* end) const;

    /**
     * Identify the single primary language type for sentence, using script count, especially for (EN, KR).
     * \param begin start of sentence string in \p encoding
     * \param end end of sentence string in \p encoding
     * \return the single primary language type
     */
    template<EncodingID encoding>
    LanguageID analyzeSentenceOnScriptCount(const char* begin, const char* end) const;

    /**
     * Count the sentences for each language type contained between [begin, end) in \p encoding.
     * \param[in] begin start of string in \p encoding
     * \param[in] end end of string in \p encoding, excluding this end
     * \param[out] countVec sentence counts of each language type
     * \param[in] maxInputSize maximum analysis size in bytes, the entire string would be analyzed if a non-positive value is given.
     */
    template<EncodingID encoding>
    void countIDFromString(const char* begin, const char* end, std::vector<int>& countVec, int maxInputSize) const;

    /**
     * Count the sentences for each language type contained in \e fileName in UTF-8 encoding.
     * \param[in] fileName file name, which content is in UTF-8 encoding
     * \param[out] countVec sentence counts of each language type
     * \param[in] maxInputSize maximum analysis size in bytes, the entire file would be analyzed if a non-positive value is given.
     * \return true for success, false for failure
     */
    bool countIDFromFile(const char* fileName, std::vector<int>& countVec, int maxInputSize) const;

    /**
     * Identify the single primary language type in the sentence counts of each language type \e countVec.
     * \param countVec sentence counts of each language type
     * \return the single primary language type, \e LANGUAGE_ID_UNKNOWN is returned if no count exsits for CJK/E.
     */
    LanguageID getPrimaryID(const std::vector<int>& countVec) const;

    /**
     * Sort the language types by sentence count in descending order, excluding those types with zero sentence count.
     * \param[in] countVec sentence count of each language type
     * \param[out] idVec the list of multiple language types sorted by its sentence count in descending order,
     * that is, <em> idVec[0] </em> would be the primary language type, and <em> idVec[1] </em> the next primary type, etc.
     * \attention the original data in \e idVec would be removed.
     */
    void getMultipleID(const std::vector<int>& countVec, std::vector<LanguageID>& idVec) const;

    /**
     * Segment \e str into language regions, and append them into \e regionVec.
     * \param[in] begin start of string in \p encoding
     * \param[in] end start of string in \p encoding
     * \param[out] regionVec region results
     */
    template<EncodingID encoding>
    void addLanguageRegion(const char* begin, const char* end, std::vector<LanguageRegion>& regionVec) const;

    /**
     * Combine the language regions to ensure each size is larger than \e minSize.
     * For each region in \e regionVec, if its size is not larger than \e minSize,
     * it is combined into adjacent larger block in a different language.
     * \param[in][out] regionVec regions, used as input of language blocks, and also as output of combination result.
     */
    void combineLanguageRegion(std::vector<LanguageRegion>& regionVec, std::size_t minSize) const;

    void convertUStringLanguageRegion(std::vector<LanguageRegion>& regionVec) const;

    bool isOptionNoChineseTraditional() const;
    int getOptionBlockSizeThreshold() const;

private:
    /** script type table */
    const ScriptTable& scriptTable_;

    /** sentence tokenizer */
    SentenceTokenizer sentenceTokenizer_;

    /** get option value */
    const Analyzer* option_;
};

NS_ILPLIB_LANGID_END

#endif // LANGID_LANGUAGE_ANALYZER_H
