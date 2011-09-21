/** \file sentence_tokenizer.h
 * Definition of class SentenceTokenizer.
 * The sentence boundary rules are based on "5 Sentence Boundaries" in "Unicode Standard Annex #29"
 * (http://www.unicode.org/reports/tr29/#Sentence_Boundaries).
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 04, 2009
 */

#ifndef LANGID_SENTENCE_TOKENIZER_H
#define LANGID_SENTENCE_TOKENIZER_H

#include <langid/language_id.h>
#include "sentence_break_table.h"

#include <cstddef> // std::size_t

NS_ILPLIB_LANGID_BEGIN

/**
 * SentenceTokenizer tokenizes a raw input stream to a sequence of sentences.
 * Typically, the usage is like below:
 *
 * \code
 * // create instance of sentence break table
 * SentenceBreakTable table;
 *
 * // load configuration and table
 * const char* configFile = "../db/config/sentence_break.def";
 * if(! table.loadConfig(configFile))
 * {
 *     cerr << "error: fail to load file " << configFile << endl;
 *     exit(1);
 * }
 *
 * // create instance of SentenceTokenizer
 * SentenceTokenizer tokenizer(table);
 *
 * // the raw input string
 * const char* p = "Abcde. FGHIJ";
 * const char* end = p + strlen(p);
 *
 * while(std::size_t len = tokenizer.getSentenceLength<ENCODING_ID_UTF8>(p, end))
 * {
 *      // print each sentence
 *      cout << string(p, len) << endl;
 *
 *      p += len;
 * }
 * \endcode
 */
class SentenceTokenizer
{
public:
    /**
     * Constructor.
     * \param table sentence break table used to determine sentence boundary
     */
    SentenceTokenizer(const SentenceBreakTable& table)
        : breakTable_(table)
    {}

    /**
     * Get the length of the first sentence between [begin, end).
     * \param begin pointer to the start of raw input string encoded in \p encoding
     * \param end pointer to the end of raw input string encoded in \p encoding
     * \return the length in bytes of the first sentence starting from \p begin,
     * 0 is returned if there is no sentence left, that is, when begin >= end.
     */
    template<EncodingID encoding>
    std::size_t getSentenceLength(const char* begin, const char* end) const;
    
private:
    /**
     * Execute the "*" quantifier match in regular expression.
     * That is, starting from \e begin, match with \e mask as many times as possible,
     * and return the count of bytes matched. The character is matched if and only if its sentence break type is included in \e mask.
     * \param begin pointer to the first character, the range used is [\e begin, \e end)
     * \param mask bit mask in match. The match pattern is (each type & \e mask).
     * \return the count of bytes matched.
     */
    template<EncodingID encoding>
    std::size_t starMatch(const char* begin, const char* end, int mask) const;

    /**
     * Print character information for debug use.
     * \param s start of character string
     * \param n byte count of the character
     * \param ucs UCS value of the character
     * \param type sentence break type
     */
    void debugPrintUCS(const char* s, std::size_t n, unsigned short ucs, SentenceBreakType type) const;

    /**
     * Print character information for debug use.
     * \param s start of character string
     * \param n byte count of the character
     * \param type sentence break type
     */
    void debugPrintUCS(const char* s, std::size_t n, SentenceBreakType type) const;

private:
    /** sentence break table */
    const SentenceBreakTable& breakTable_;

    enum BitMask
    {
        MASK_SEP = SB_TYPE_SEP | SB_TYPE_CR | SB_TYPE_LF, ///< paragraph separators
        MASK_TERM = SB_TYPE_STERM | SB_TYPE_ATERM, ///< sentence terminators
        MASK_SEP_TERM_CONTINUE = MASK_SEP | MASK_TERM | SB_TYPE_SCONTINUE, ///< sentence separators, terminators and continues
        MASK_NEG_LETTER = ~(SB_TYPE_OLETTER | SB_TYPE_UPPER | SB_TYPE_LOWER | MASK_SEP | MASK_TERM) ///< negation of letters, sentence separators and terminators
    };
};

NS_ILPLIB_LANGID_END

#include "sentence_tokenizer.inl"

#endif // LANGID_SENTENCE_TOKENIZER_H
