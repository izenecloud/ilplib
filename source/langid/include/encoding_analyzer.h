/** \file encoding_analyzer.h
 * Definition of class EncodingAnalyzer.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 25, 2009
 */

#ifndef LANGID_ENCODING_ANALYZER_H
#define LANGID_ENCODING_ANALYZER_H

#include "langid/language_id.h"

namespace langid
{

class ProfileModel;

/**
 * EncodingAnalyzer identifies character encoding such as UTF-8, GB18030, EUC-JP, EUC-KR, etc.
 */
class EncodingAnalyzer
{
public:
    /**
     * Constructor.
     * \param profileModel profile model for encoding identification
     */
    EncodingAnalyzer(const ProfileModel& profileModel);

    /**
     * Identify the character encoding of \e str.
     * \param[in] str string
     * \param[out] id the character encoding as identification result
     * \param[in] maxInputSize maximum analysis size in bytes, the entire string would be analyzed if a non-positive value is given.
     * \return true for success, false for failure
     */
    bool encodingFromString(const char* str, EncodingID& id, int maxInputSize) const;

    /**
     * Identify the character encoding of file \e fileName.
     * \param[in] fileName file name
     * \param[out] id the character encoding as identification result
     * \param[in] maxInputSize maximum analysis size in bytes, the entire file would be analyzed if a non-positive value is given.
     * \return true for success, false for failure
     */
    bool encodingFromFile(const char* fileName, EncodingID& id, int maxInputSize) const;

private:
    /** profile model for encoding identification */
    const ProfileModel& profileModel_;
};

} // namespace langid

#endif // LANGID_ENCODING_ANALYZER_H
