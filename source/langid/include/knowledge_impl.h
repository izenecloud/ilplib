/** \file knowledge_impl.h
 * Definition of class KnowledgeImpl.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 24, 2009
 */

#ifndef LANGID_KNOWLEDGE_IMPL_H
#define LANGID_KNOWLEDGE_IMPL_H

#include "langid/knowledge.h"
#include "sentence_break_table.h"
#include "script_table.h"
#include "profile_model.h"

namespace langid
{

/**
 * KnowledgeImpl is the implementation class for Knowledge.
 * It manages the linguistic information for language identification.
 */
class KnowledgeImpl : public Knowledge
{
public:
    /**
     * Constructor.
     */
    KnowledgeImpl();

    /**
     * Destructor.
     */
    virtual ~KnowledgeImpl();

    /**
     * Load the encoding model for encoding identification.
     * \param fileName file name
     * \return true for success, false for fail
     */
    virtual bool loadEncodingModel(const char* fileName);

    /**
     * Load the language model for language identification or sentence tokenization.
     * \param fileName file name
     * \return true for success, false for fail
     */
    virtual bool loadLanguageModel(const char* fileName);

    /**
     * Get the sentence break type table to determine sentence boundary.
     * \return the sentence break table
     */
    const SentenceBreakTable& getSentenceBreakTable() const;

    /**
     * Get the script type table to determine scitpy type.
     * \return the script type table
     */
    const ScriptTable& getScriptTable() const;

    /**
     * Get the profile model to determine encoding.
     * \return the profile model
     */
    const ProfileModel& getProfileModel() const;

private:
    /** the sentence break type table for sentence tokenization */
    SentenceBreakTable sentenceTable_;

    /** the script type table for language identification */
    ScriptTable scriptTable_;

    /** the profile model for encoding identification */
    ProfileModel profileModel_;
};

} // namespace langid

#endif // LANGID_KNOWLEDGE_IMPL_H
