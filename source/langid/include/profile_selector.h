/** \file profile_selector.h
 * Definition of class ProfileSelector.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 10, 2009
 */

#ifndef LANGID_PROFILE_SELECTOR_H
#define LANGID_PROFILE_SELECTOR_H

#include "profile.h"
#include <string>

#include <vector>

NS_ILPLIB_LANGID_BEGIN

/**
 * ProfileSelector selects the best profile for language identification.
 */
class ProfileSelector
{
public:
    /**
     * Constructor.
     * \param ngram the range of N-grams. For example, given the value of 5, all N-grams for N=1 to 5 would be generated.
     * \param length the length of profile. For example, given the value of 400, 400 N-grams with highest frequency would be generated.
     */
    explicit ProfileSelector(int ngram = 5, int length = 400);

    /**
     * Load the profiles to select from, which profile paths are configured in \e configFile.
     * \param configFile configuration file name
     * \return true for success, false for fail
     */
    bool loadProfiles(const char* configFile);

    /**
     * Select the profile which distance is the shortest from the document \e fileName.
     * \param fileName file name
     * \return profile name, note that 0 would be returned if any error happens in profile selection.
     */
    const char* selectProfileFromFile(const char* fileName) const;

    /**
     * Select the profile which distance is the shortest from the string \e str.
     * \param str string
     * \return profile name, note that 0 would be returned if any error happens in profile selection.
     */
    const char* selectProfileFromStr(const char* str) const;

private:
    /**
     * Select the profile which distance is the shortest from \e document.
     * \param document the document to compare with
     * \return profile name, note that 0 would be returned if any error happens in profile selection.
     */
    const char* selectProfile(const Profile& document) const;

private:
    /** the range of N-grams.
     * For example, given the value of 5, all N-grams for N=1 to 5 would be generated.
     */
    const int ngramRange_;

    /** the length of profile.
     * For example, given the value of 400, 400 N-grams with highest frequency would be generated.
     */
    const int profileLength_;

    /** the array of profiles to select from */
    std::vector<Profile> profiles_;

    /** the array of profile names as selection result */
    std::vector<std::string> profileNames_;
};

NS_ILPLIB_LANGID_END

#endif // LANGID_PROFILE_SELECTOR_H
