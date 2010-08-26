/** \file script_table.cpp
 * Implementation of class ScriptTable.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 19, 2009
 */

#include "script_table.h"
#include "property_table.cpp"

using namespace std;

NS_ILPLIB_LANGID_BEGIN

/**
 * PropertyTable<ScriptType> gives the script type for each UCS2 value.
 */
template<> PropertyTable<ScriptType, uint8_t>::PropertyTable()
    : table_(PROPERTY_TABLE_SIZE, SCRIPT_TYPE_OTHER)
{
    strMap_["ALPHABET"] = SCRIPT_TYPE_ALPHABET;
    strMap_["SIMPLIFIED"] = SCRIPT_TYPE_SIMPLIFIED;
    strMap_["TRADITIONAL"] = SCRIPT_TYPE_TRADITIONAL;
    strMap_["KANA"] = SCRIPT_TYPE_KANA;
    strMap_["HANGUL"] = SCRIPT_TYPE_HANGUL;
    strMap_["OTHER"] = SCRIPT_TYPE_OTHER;
}

/** specialize PropertyTable<> for ScriptType in cpp compile unit,
 * so that we need only include header "script_table.h" to use it. */
template class PropertyTable<ScriptType, uint8_t>;

NS_ILPLIB_LANGID_END
