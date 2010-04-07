/** \file sentence_break_table.cpp
 * Implementation of class SentenceBreakTable.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 04, 2009
 */

#include "sentence_break_table.h"
#include "property_table.cpp"

using namespace std;

#ifndef LANGID_DEBUG_PRINT
	#define LANGID_DEBUG_PRINT 1
#endif

NS_ILPLIB_LANGID_BEGIN

/**
 * PropertyTable<SentenceBreakType> gives the script type for each UCS2 value.
 */
template<> PropertyTable<SentenceBreakType, uint16_t>::PropertyTable()
    : table_(PROPERTY_TABLE_SIZE, SB_TYPE_OTHER)
{
    strMap_["Other"] = SB_TYPE_OTHER;
    strMap_["CR"] = SB_TYPE_CR;
    strMap_["LF"] = SB_TYPE_LF;
    strMap_["Extend"] = SB_TYPE_EXTEND;
    strMap_["Sep"] = SB_TYPE_SEP;
    strMap_["Format"] = SB_TYPE_FORMAT;
    strMap_["Sp"] = SB_TYPE_SP;
    strMap_["Lower"] = SB_TYPE_LOWER;
    strMap_["Upper"] = SB_TYPE_UPPER;
    strMap_["OLetter"] = SB_TYPE_OLETTER;
    strMap_["Numeric"] = SB_TYPE_NUMERIC;
    strMap_["ATerm"] = SB_TYPE_ATERM;
    strMap_["STerm"] = SB_TYPE_STERM;
    strMap_["Close"] = SB_TYPE_CLOSE;
    strMap_["SContinue"] = SB_TYPE_SCONTINUE;
}

/** specialize PropertyTable<> for SentenceBreakType in cpp compile unit,
 * so that we need only include header "sentence_break_table.h" to use it. */
template class PropertyTable<SentenceBreakType, uint16_t>;

NS_ILPLIB_LANGID_END
