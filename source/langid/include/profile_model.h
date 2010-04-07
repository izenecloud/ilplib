/** \file profile_model.h
 * Definition of class ProfileModel.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 25, 2009
 */

#ifndef LANGID_PROFILE_MODEL_H
#define LANGID_PROFILE_MODEL_H

#include "profile.h"
#include "langid/language_id.h"

#include <vector>

namespace langid
{

/**
 * ProfileModel manages a list of profiles for encoding identification.
 */
class ProfileModel
{
public:
    /**
     * Constructor.
     */
    ProfileModel();

    /**
     * Load the profiles to select from, which profile paths are configured in file \e fileName.
     * \param fileName configuration file name
     * \return true for success, false for fail
     */
    bool loadConfig(const char* fileName);

    /**
     * Load the profiles to select from, which profile contents are in file \e fileName.
     * \param fileName model file name
     * \return true for success, false for fail
     */
    bool loadModel(const char* fileName);

    /**
     * Get the range of N-grams.
     * \return N-grams range
     *
     * For example, given the range value of 5, all N-grams for N=1 to 5 would be generated.
     */
    int getNGramRange() const;

    /**
     * Get the length of profile.
     * \return profile length
     *
     * For example, given the length value of 400, 400 N-grams with highest frequency would be generated.
     */
    int getProfileLength() const;

    /**
     * Get encoding of the profile which distance is the shortest from \e document.
     * \param[in] document the document to compare with
     * \param[out] id the character encoding as identification result
     * \return true for success, false for failure
     */
    bool encodingFromProfile(const Profile& document, EncodingID& id) const;

    /**
     * Load the profiles content from \e configFile, and build them into binary file \e modelFile.
     * \param[in] configFile configuration file name to load
     * \param[in] modelFile model file name to output
     * \return true for success, false for fail
     */
    static bool buildModel(const char* configFile, const char* modelFile);

private:
    /**
     * Release the profiles loaded previously.
     */
    void clear();

    /**
     * Save the profiles content into binary file \e fileName, the profiles content should have been loaded by \e loadConfig() previously.
     * \param fileName model file name
     * \return true for success, false for fail
     */
    bool saveModel(const char* fileName) const;

private:
    /** the range of N-grams.
     * For example, given the value of 5, all N-grams for N=1 to 5 would be generated.
     */
    int ngramRange_;

    /** the length of profile.
     * For example, given the value of 400, 400 N-grams with highest frequency would be generated.
     */
    int profileLength_;

    /** the array of profiles to select from */
    std::vector<Profile> profileVec_;

    /** the array of encoding IDs as selection result */
    std::vector<EncodingID> encodingIDVec_;
};

} // namespace langid

#endif // LANGID_PROFILE_MODEL_H
