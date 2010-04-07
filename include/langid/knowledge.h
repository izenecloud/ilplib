/** \file knowledge.h
 * Definition of class Knowledge.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Oct 30, 2009
 */

#ifndef LANGID_KNOWLEDGE_H
#define LANGID_KNOWLEDGE_H

#include "language_id.h"

namespace langid
{

/**
 * Knowledge manages the linguistic information for language identification.
 */
class Knowledge
{
public:
    /**
     * Constructor.
     */
    Knowledge();

    /**
     * Destructor.
     */
    virtual ~Knowledge();

    /**
     * Load the encoding model for encoding identification.
     * \param fileName file name
     * \return true for success, false for fail
     */
    virtual bool loadEncodingModel(const char* fileName) = 0;

    /**
     * Load the language model for language identification or sentence tokenization.
     * \param fileName file name
     * \return true for success, false for fail
     */
    virtual bool loadLanguageModel(const char* fileName) = 0;

    /**
     * Get the language id given the language name.
     *
     * \param name language name
     * \return language id, note that \e LANGUAGE_ID_NUM would be returned if the language name is undefined.
     */
    static LanguageID getLanguageIDFromName(const char* name);

    /**
     * Get the language name given the language id.
     *
     * \param id language id
     * \return language name, note that \e "UNDEFINED" would be returned if the language id is undefined.
     */
    static const char* getLanguageNameFromID(LanguageID id);

    /**
     * Get the encoding id given the encoding name.
     *
     * \param name encoding name
     * \return encoding id, note that \e ENCODING_ID_NUM would be returned if the encoding name is undefined.
     */
    static EncodingID getEncodingIDFromName(const char* name);

    /**
     * Get the encoding name given the encoding id.
     *
     * \param id encoding id
     * \return encoding name, note that \e "UNDEFINED" would be returned if the encoding id is invalid.
     */
    static const char* getEncodingNameFromID(EncodingID id);
};

} // namespace langid

#endif // LANGID_KNOWLEDGE_H
