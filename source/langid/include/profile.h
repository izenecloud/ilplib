/** \file profile.h
 * Definition of class Profile.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 04, 2009
 */

#ifndef LANGID_PROFILE_H
#define LANGID_PROFILE_H

#include <string>
#include <istream>
#include <fstream>
#include <vector>
#include <utility>

#define USE_HASH_MAP 1

#if USE_HASH_MAP
#include <ext/hash_map>
/** Namespace of GNU C++ library for SGI's STL extensions. */
namespace __gnu_cxx
{
/** hash function used for __gnu_cxx::hash_map. */
template<> struct hash<std::string>
{
    /**
     * Get hash value from the string.
     * \param key string as input
     * \return hash value
     */
    size_t operator()(const std::string& key) const
    {
        // comment the default hash function by __gnu_cxx::hash
        // return hash<const char*>()(x.c_str());
        // use below hash function instead for performance advantage
        size_t h = key.length() * 13;
        std::string::const_iterator p, end;
        p = key.begin();
        end = key.end();
        while(p != end)
        {
            h = (h << 5) - h + *p++;
        }
        return h;
    }
};
}
#else
#include <map>
#endif

namespace langid
{

/**
 * Profile operates N-grams of a document or string, such as generate, compare, open and save into file.
 */
class Profile
{
private:
    /** type of the map associating N-gram string with a value. */
#if USE_HASH_MAP
    typedef __gnu_cxx::hash_map<std::string, int> TMap;
#else
    typedef std::map<std::string, int> TMap;
#endif

    /** type of the pair associating N-gram string with a value. */
    typedef std::pair<std::string, int> TPair;

    /** type of the \e TPair array. */
    typedef std::vector<TPair> TVec;

    /** sort by descending second item, ascending first item. */
    template<class T> struct great_second_less_first : std::binary_function<T, T, bool>
    {
        /** compare by great second item, less first item.
         * \param lhs left value
         * \param rhs right value
         * \return true if \e lhs goes before \e rhs in sorted sequence, and false otherwise
         */
        inline bool operator()(const T& lhs, const T& rhs)
        {
            return lhs.second > rhs.second || (lhs.second == rhs.second && lhs.first < rhs.first);
        }
    };

    /** n-gram comparison used for sort */
    typedef great_second_less_first<TPair> NGramCompare;

public:
    /**
     * Constructor.
     * \param ngram the range of N-grams. For example, given the value of 5, all N-grams for N=1 to 5 would be generated.
     * \param length the length of profile. For example, given the value of 400, 400 N-grams with highest frequency would be generated.
     */
    explicit Profile(int ngram = 5, int length = 400);

    /**
     * Generate N-grams given a file.
     * \param[in] fileName file name
     * \param[in] maxInputSize maximum analysis size in bytes, the entire file would be analyzed if a non-positive value is given.
     * \return true for success, false for fail
     */
    bool generateFromFile(const char* fileName, int maxInputSize = 0);

    /**
     * Generate N-grams given a string.
     * \param[in] str string
     * \param[in] maxInputSize maximum analysis size in bytes, the entire string would be analyzed if a non-positive value is given.
     * \return true for success, false for fail
     */
    bool generateFromStr(const char* str, int maxInputSize = 0);

    /**
     * Measure the distance from \e profile.
     * \param profile the profile to measure from
     * \return the out-of-place measure for distance, smaller this value, more similar between these two profiles.
     */
    unsigned int measureDistance(const Profile& profile) const;

    /**
     * Save \e freqVec_ (ngrams with frequency) into a text file.
     * \param fileName file name
     * \return true for success, false for fail
     */
    bool saveText(const char* fileName) const;

    /**
     * Read \e freqVec_ (ngrams with frequency) from a text file.
     * \param fileName file name
     * \return true for success, false for fail
     */
    bool loadText(const char* fileName);

    /**
     * Save \e rankMap_ (ngrams in rank order) into a binary file.
     * \param ofs output file stream, which is already opened
     * \return true for success, false for fail
     */
    bool saveBinary(std::ofstream& ofs) const;

    /**
     * Read \e rankMap_ (ngrams in rank order) from a binary file.
     * \param ifs input file stream, which is already opened
     * \return true for success, false for fail
     */
    bool loadBinary(std::ifstream& ifs);

private:
    /**
     * Check whether the parameters are valid.
     * \return true for valid, false for invalid
     */
    bool validateParam() const;

    /**
     * Generate N-grams from stream.
     * \param[in] is input stream
     * \param[in] maxInputSize maximum analysis size in bytes, the entire stream would be analyzed if a non-positive value is given.
     * \return true for success, false for fail
     */
    bool generateFromStream(std::istream& is, int maxInputSize);

    /**
     * extract \e top elements.
     * \param srcMap map to extract from
     * \param destVec vector to save top elements
     * \param top element count to extract
     */
    void extractTopElement(const TMap& srcMap, TVec& destVec, int top) const;

private:
    /** the range of N-grams.
     * For example, given the value of 5, all N-grams for N=1 to 5 would be generated.
     */
    int ngramRange_;

    /** the length of profile.
     * For example, given the value of 400, 400 N-grams with highest frequency would be generated.
     */
    int profileLength_;

    /** map from N-gram string to its ranking value.
     * used to read from file. */
    TMap rankMap_;

    /** array of N-gram strings with descendent frequency value.
     * used to write into file, also read from file when convert from text to binary mode. */
    TVec freqVec_;
};

} // namespace langid

#endif // LANGID_PROFILE_H
