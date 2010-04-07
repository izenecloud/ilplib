/** \file sentence_break_table.h
 * Definition of class SentenceBreakTable.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 04, 2009
 */

#ifndef LANGID_SENTENCE_BREAK_TABLE_H
#define LANGID_SENTENCE_BREAK_TABLE_H

#include "property_table.h"

#include <stdint.h> // uint16_t

namespace langid
{
/**
 * Enum of sentence break type, used to determine sentence boundary.
 * These types are based on "5.1 Default Sentence Boundary Specification" in "Unicode Standard Annex #29"
 * (http://www.unicode.org/reports/tr29/#Default_Sentence_Boundaries).
 */
enum SentenceBreakType
{
    SB_TYPE_OTHER = 0x0001, ///< types other than below
    SB_TYPE_CR = 0x0002, ///< carriage return
    SB_TYPE_LF = 0x0004, ///< line feed
    SB_TYPE_EXTEND = 0x0008, ///< combining grave accent, etc
    SB_TYPE_SEP = 0x0010, ///< line separator, etc
    SB_TYPE_FORMAT = 0x0020, ///< U+FEFF as zero width no-break space, etc
    SB_TYPE_SP = 0x0040, ///< space, tab, ideographic space
    SB_TYPE_LOWER = 0x0080, ///< small letter such as "a" in half and full width
    SB_TYPE_UPPER = 0x0100, ///< capital letter such as "A" in half and full width
    SB_TYPE_OLETTER = 0x0200, ///< CJK ideographic and Hangul letter, etc
    SB_TYPE_NUMERIC = 0x0400, ///< digit such as "0" in half and full width
    SB_TYPE_ATERM = 0x0800, ///< full stop such as "." in half and full width
    SB_TYPE_STERM = 0x1000, ///< exclamation and question mark such as "!?" in half and full width, etc
    SB_TYPE_CLOSE = 0x2000, ///< quotation mark such as "\"" in half and full width, etc
    SB_TYPE_SCONTINUE = 0x4000, ///< comma such as "," in half and full width, etc
};

/** SentenceBreakTable gives the sentence break type for each UCS2 value. */
typedef PropertyTable<SentenceBreakType, uint16_t> SentenceBreakTable;

} // namespace langid

#endif // LANGID_SENTENCE_BREAK_TABLE_H
