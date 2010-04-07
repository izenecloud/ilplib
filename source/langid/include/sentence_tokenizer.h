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

#include "ucs2_converter.h"
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
 *
 * while(int len = tokenizer.getSentenceLength(p))
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
    SentenceTokenizer(const SentenceBreakTable& table);

    /**
     * Get the length of the first sentence starting from \e str.
     * \param str pointer to the raw input string encoded in UTF-8
     * \return the length in bytes of the first sentence starting from \e str,
     * 0 is returned if there is no sentence left, that is, when \e *str is null.
     */
    int getSentenceLength(const char* str) const;
    
private:
    /**
     * Execute the "*" quantifier match in regular expression.
     * That is, starting from \e begin, match with \e mask as many times as possible,
     * and return the count of bytes matched. The character is matched if and only if its sentence break type is included in \e mask.
     * \param begin pointer to the first character, the range used is [\e begin, \e end)
     * \param mask bit mask in match. The match pattern is (each type & \e mask).
     * \return the count of bytes matched.
     */
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

    /** character tokenizer */
    Utf8ToUcs2 encoding_;
};

NS_ILPLIB_LANGID_END

#endif // LANGID_SENTENCE_TOKENIZER_H
