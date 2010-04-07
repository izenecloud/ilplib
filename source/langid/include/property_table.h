/** \file property_table.h
 * Definition of class PropertyTable.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 03, 2009
 */

#ifndef LANGID_PROPERTY_TABLE_H
#define LANGID_PROPERTY_TABLE_H

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <cassert>
#include <stdint.h> // uint32_t

namespace langid
{
/**
 * PropertyTable gives the character property for each UCS2 value.
 * \tparam T the type of property value
 * \tparam BIN_T the property value would be converted as bytes in the size of \e BIN_T in binary file IO.
 */
template<class T, class BIN_T = T>
class PropertyTable
{
private:
    /** the table of properties for all UCS2 values. */
    std::vector<T> table_;

    /** the map from string representation to its property. */
    std::map<std::string, T> strMap_;

    /**
     * The range defined in the configuration file, which is used in \e loadConfig().
     */
    struct Range
    {
        unsigned short start_; ///< range start value
        unsigned short end_; ///< range end value
        T property_; ///< property of the range
    };

    enum
    {
        PROPERTY_TABLE_SIZE = 0x10000, ///< the size of property table
    };

    /** type of property value */
    typedef T Value_type;

    /** type of binary value */
    typedef BIN_T Binary_type;

    /** type of unsigned int 32 */
    typedef uint32_t UInt32_type;

public:
    /**
     * Constructor.
     */
    PropertyTable();

    /**
     * Get the property of UCS2 value.
     * \param id the UCS2 value
     * \return the property value
     */
    T getProperty(unsigned short id) const
    {
        assert(id < table_.size() && "error: UCS2 value exceeds the table size.");
        return table_[id];
    }

    /**
     * Load configuration file for multiple properties.
     * \param fileName file name, which file configures the ranges of UCS2 values for each property.
     * \return true for success, false for failure
     */
    bool loadConfig(const char* fileName);

    /**
     * Load character table for single property.
     * \param fileName file name, in which file the first line denotes the property, and its UCS2 values are listed in rest lines.
     * \return true for success, false for failure
     */
    bool loadTable(const char* fileName);

    /**
     * Save \e table_ into a binary file.
     * \param ofs output file stream, which is already opened
     * \return true for success, false for fail
     */
    bool saveBinary(std::ofstream& ofs) const;

    /**
     * Read \e table_ from a binary file.
     * \param ifs input file stream, which is already opened
     * \return true for success, false for fail
     */
    bool loadBinary(std::ifstream& ifs);

    /**
     * Convert the string representation to its property type.
     * \param[in] str the string representation of the property
     * \param[out] property the property type as conversion result
     * \return true for \e property type is assigned, false for \e str is unknown.
     */
    bool strToProperty(const std::string& str, T& property) const;

    /**
     * Convert the property type to its string representation.
     * \param[in] property the property type
     * \param[out] str the string representation as conversion result
     * \return true for \e str is assigned, false for \e property is unknown.
     */
    bool propertyToStr(const T& property, std::string& str) const;
};

} // namespace langid

#endif // LANGID_PROPERTY_TABLE_H
