/** \file analyzer.h
 * Definition of class Analyzer.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 24, 2009
 */

#ifndef LANGID_ANALYZER_H
#define LANGID_ANALYZER_H

#include "language_id.h"
#include <ilplib.h>

#include <vector>

/**
 * namespace of library Language Identification.
 */
NS_ILPLIB_LANGID_BEGIN

class Knowledge;

/**
 * LanguageRegion gives the position information of a text block in a single language type.
 */
struct LanguageRegion
{
    /** the language type */
    LanguageID languageID_;

    /** region start position (zero-indexed in bytes) */
    unsigned int start_;

    /** region length (in bytes) */
    unsigned int length_;
};

/**
 * Analyzer could analyze things below:
 * - identify encoding such as UTF-8, GB18030, EUC-JP, EUC-KR, etc,
 * - identify language in UTF-8 text, such as Chinese (simplified, traditional), Japanese, Korean and English,
 * - sentence tokenization in UTF-8 text.
 *
 * Typically, the usage is like below:
 * \code
 * // create instances
 * Factory* factory = Factory::instance();
 * Analyzer* analyzer = factory->createAnalyzer();
 * Knowledge* knowledge = factory->createKnowledge();
 *
 * // load encoding model for encoding identification
 * knowledge->loadEncodingModel("db/langid/model/encoding.bin");
 *
 * // load language model for language identification or sentence tokenization
 * knowledge->loadLanguageModel("db/langid/model/language.bin");
 *
 * // set knowledge
 * analyzer->setKnowledge(knowledge);
 *
 * // identify character encoding of string
 * EncodingID encID;
 * analyzer->encodingFromString("...", encID);
 *
 * // identify character encoding of file
 * analyzer->encodingFromFile("...", encID);
 *
 * // identify the single primary language of string in UTF-8 encoding
 * LanguageID langID;
 * analyzer->languageFromString("...", langID);
 *
 * // identify the single primary language of file in UTF-8 encoding
 * analyzer->languageFromFile("...", langID);
 *
 * // identify the list of multiple languages of string in UTF-8 encoding
 * vector<LanguageID> langIDVec;
 * analyzer->languageListFromString("...", langIDVec);
 *
 * // identify the list of multiple languages of file in UTF-8 encoding
 * analyzer->languageListFromFile("...", langIDVec);
 *
 * // segment the UTF-8 multi-lingual string into single-language regions
 * vector<LanguageRegion> regionVec;
 * analyzer->segmentString("...", regionVec);
 *
 * // segment the UTF-8 multi-lingual document into single-language regions
 * analyzer->segmentFile("...", regionVec);
 *
 * // get the length of the first sentence of string in UTF-8 encoding
 * int len = analyzer->sentenceLength("...");
 *
 * delete knowledge;
 * delete analyzer;
 * \endcode
 */
class Analyzer
{
public:
    /**
     * Constructor.
     */
    Analyzer();

    /**
     * Destructor.
     */
    virtual ~Analyzer();

    /**
     * Option type for analysis.
     */
    enum OptionType
    {
        /** Configure the maximum input size for encoding and language identification.
         * If a non-zero value is configured,
         * it is used as the maximum input size for below APIs:
         * \e encodingFromString(), \e encodingFromFile(),
         * \e languageFromString(), \e languageFromFile(),
         * that is, only the input bytes within this range would be used in these APIs,
         * and the rest bytes are just ignored,
         * so that a good performance could be expected.
         *
         * If a zero value is configured,
         * it uses all the input bytes for these APIs,
         * so that a high accuracy could be expected.
         *
         * Default value: 1024
         */
        OPTION_TYPE_LIMIT_ANALYZE_SIZE,

        /** Configure the threshold of language block size.
         * If a non-zero value is configured, for below APIs:
         * \e segmentString(), \e segmentFile(),
         * the language block, which size is not larger than this value,
         * would be combined into adjacent larger block in a different language.
         *
         * If a zero value is configured,
         * it disables combining blocks in different languages.
         *
         * Default value: 0
         */
        OPTION_TYPE_BLOCK_SIZE_THRESHOLD,

        /** Configure not to use Chinese Traditional language.
         * If a non-zero value is configured, for below APIs:
         * \e languageFromString(), \e languageFromFile(),
         * \e languageListFromString(), \e languageListFromFile(),
         * \e segmentString(), \e segmentFile(),
         * Chinese Traditional text would be identified as Chinese Simplified language.
         *
         * If a zero value is configured,
         * Chinese Traditional text would be identified as Chinese Traditional language as original.
         *
         * Default value: 0
         */
        OPTION_TYPE_NO_CHINESE_TRADITIONAL,

        /** the count of option types */
        OPTION_TYPE_NUM
    };

    /**
     * Set the option value for analysis.
     * \param nOption the option type
     * \param nValue the option value
     */
    void setOption(OptionType nOption, int nValue);

    /**
     * Get the option value.
     * \param nOption the option type
     * \return the option value
     */
    int getOption(OptionType nOption) const;

    /**
     * Set the \e Knowledge for analysis.
     * \param pKnowledge the pointer of \e Knowledge
     */
    virtual void setKnowledge(Knowledge* pKnowledge) = 0;

    /**
     * Identify the character encoding of \e str.
     * \param[in] str string
     * \param[out] id the character encoding as identification result
     * \return true for success, false for failure
     */
    virtual bool encodingFromString(const char* str, EncodingID& id) = 0;

    /**
     * Identify the character encoding of file \e fileName.
     * \param[in] fileName file name
     * \param[out] id the character encoding as identification result
     * \return true for success, false for failure
     */
    virtual bool encodingFromFile(const char* fileName, EncodingID& id) = 0;

    /**
     * Identify the single primary language type contained in \e str in UTF-8 encoding.
     * \param[in] str string in UTF-8 encoding
     * \param[out] id the single primary language type as identification result
     * \return true for success, false for failure
     */
    virtual bool languageFromString(const char* str, LanguageID& id) = 0;

    /**
     * Identify the single primary language type contained in file \e fileName in UTF-8 encoding.
     * \param[in] fileName file name, which content is in UTF-8 encoding
     * \param[out] id the single primary language type as identification result
     * \return true for success, false for failure
     */
    virtual bool languageFromFile(const char* fileName, LanguageID& id) = 0;

    /**
     * Identify the list of multiple language types contained in \e str in UTF-8 encoding.
     * \param[in] str string in UTF-8 encoding
     * \param[out] idVec the list of multiple language types as identification result,
     * the items in \e idVec are sorted by its sentence count in descending order,
     * that is, <em> idVec[0] </em> would be the primary language type, and <em> idVec[1] </em> the next primary type, etc.
     * \return true for success, false for failure
     * \attention the original data in \e idVec would be removed.
     */
    virtual bool languageListFromString(const char* str, std::vector<LanguageID>& idVec) = 0;

    /**
     * Identify the list of multiple language types contained in file \e fileName in UTF-8 encoding.
     * \param[in] fileName file name, which content is in UTF-8 encoding
     * \param[out] idVec the list of multiple language types as identification result,
     * the items in \e idVec are sorted by its sentence count in descending order,
     * that is, <em> idVec[0] </em> would be the primary language type, and <em> idVec[1] </em> the next primary type, etc.
     * \return true for success, false for failure
     * \attention the original data in \e idVec would be removed.
     */
    virtual bool languageListFromFile(const char* fileName, std::vector<LanguageID>& idVec) = 0;

    /**
     * Segment the UTF-8 multi-lingual string into single-language regions.
     * \param[in] str string in UTF-8 encoding
     * \param[out] regionVec region results, each region is in a single language type
     * \return true for success, false for failure
     * \attention the original data in \e regionVec would be removed.
     */
    virtual bool segmentString(const char* str, std::vector<LanguageRegion>& regionVec) = 0;

    /**
     * Segment the UTF-8 multi-lingual document into single-language regions.
     * \param[in] fileName file name, which content is in UTF-8 encoding
     * \param[out] regionVec region results, each region is in a single language type
     * \return true for success, false for failure
     * \attention the original data in \e regionVec would be removed.
     */
    virtual bool segmentFile(const char* fileName, std::vector<LanguageRegion>& regionVec) = 0;

    /**
     * Get the length of the first sentence starting from \e str in UTF-8 encoding.
     * \param[in] str string in UTF-8 encoding
     * \return the length in bytes of the first sentence starting from \e str,
     * 0 is returned if there is no sentence left, that is, when \e *str is null.
     */
    virtual int sentenceLength(const char* str) = 0;

private:
    /** option values */
    std::vector<int> options_;
};

NS_ILPLIB_LANGID_END

#endif // LANGID_ANALYZER_H
