/** \file script_table.h
 * Definition of class ScriptTable.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 19, 2009
 */

#ifndef LANGID_SCRIPT_TABLE_H
#define LANGID_SCRIPT_TABLE_H

#include "property_table.h"

#include <stdint.h> // uint8_t

NS_ILPLIB_LANGID_BEGIN

/**
 * Enum of script types.
 */
enum ScriptType
{
    SCRIPT_TYPE_ALPHABET, ///< alphabet such as "A", including half and full width
    SCRIPT_TYPE_SIMPLIFIED, ///< Chinese simplified ideograph, such as "国"
    SCRIPT_TYPE_TRADITIONAL, ///< Chinese traditional ideograph, such as "國"
    SCRIPT_TYPE_KANA, ///< Japanese Hiragana and Katakana, such as "あ" and "ア"
    SCRIPT_TYPE_HANGUL, ///< Korean Hangual, such as "가"
    SCRIPT_TYPE_OTHER, ///< other types
    SCRIPT_TYPE_NUM ///< the count of types
};

/** ScriptTable gives the script type for each UCS2 value. */
typedef PropertyTable<ScriptType, uint8_t> ScriptTable;

NS_ILPLIB_LANGID_END

#endif // LANGID_SCRIPT_TABLE_H
