/** \file property_table.cpp
 * Implementation of class PropertyTable.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Nov 19, 2009
 */

#include "property_table.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept> // runtime_error
#include <cstring> // strcmp

using namespace std;

#define LANGID_DEBUG_PRINT_LOAD 0

namespace
{
/**
 * Convert string to UCS2 value.
 * \param str the string to convert, such like "0x0041".
 * \return UCS2 value, which range is [0x0000, 0xFFFF].
 * \attention if any error happens in conversion, \e runtime_error would be thrown as exception.
 */
unsigned short atoucs2(const char* str)
{
    assert(str);

    size_t len = strlen(str);
    if(len < 3 || str[0] != '0' || (str[1] != 'x' && str[1] != 'X'))
    {
        string err = "error: in converting hex value from string ";
        err += str;
        err += ", which should start from '0x' in hex mode.";
        throw runtime_error(err);
    }

    if(len > 6)
    {
        string err = "error: in converting hex value from string ";
        err += str;
        err += ", which range should be within 0xFFFF for UCS2 value.";
        throw runtime_error(err);
    }

    unsigned short result = 0;
    for(size_t i = 2; i<len; ++i)
    {
        char ch = str[i];
        unsigned short t;
        if(ch >= '0' && ch <= '9')
        {
            t = ch - '0';
        }
        else if(ch >= 'A' && ch <= 'F')
        {
            t = ch - 'A' + 10;
        }
        else if(ch >= 'a' && ch <= 'f')
        {
            t = ch - 'a' + 10;
        }
        else
        {
            string err = "error: in converting hex value from string ";
            err += str;
            err += ", which string should be in hex mode.";
            throw runtime_error(err);
        }

        result = (result << 4) + t;
    }

    return result;
}
} // namespace

NS_ILPLIB_LANGID_BEGIN

template<class T, class BIN_T> PropertyTable<T, BIN_T>::PropertyTable()
: table_(PROPERTY_TABLE_SIZE, T(0))
{
}

/**
 * The format of configuration file.
# comment
0x0041..0x005A ALPHABET
0x3005         SIMPLIFIED # comment
*/
template<class T, class BIN_T> bool PropertyTable<T, BIN_T>::loadConfig(const char* fileName)
{
#if LANGID_DEBUG_PRINT_LOAD
    cout << ">>> PropertyTable::loadConfig(): " << fileName << endl;
#endif

    if(! fileName)
    {
        cerr << "error: NULL string is passed as file name." << endl;
        return false;
    }

    ifstream ifs(fileName);
    if(! ifs)
    {
        cerr << "error: could not open file " << fileName << endl;
        return false;
    }

    string line;
    string startStr, endStr, catStr;
    unsigned short start, end;
    T cat;
    vector<Range> rangeVec;
    size_t i, j;
    const char* dots = "..";
    const size_t DOTS_LEN = strlen(dots);
    const char* whitespaces = " \t";
    
    while(getline(ifs, line))
    {
        if(line.empty() || line[0] == '#')
            continue;

        i = line.find(dots);
        // 0x0041..0x005A ALPHABET
        if(i != string::npos)
        {
            startStr = line.substr(0, i);

            i += DOTS_LEN;
            j = line.find_first_of(whitespaces, i);
            if(j == string::npos)
            {
                cerr << "error: no category defined in line: " << line << endl;
                return false;
            }
            endStr = line.substr(i, j-i);
        }
        // 0x3005   SIMPLIFIED
        else
        {
            j = line.find_first_of(whitespaces);
            if(j == string::npos)
            {
                cerr << "error: no category defined in line: " << line << endl;
                return false;
            }
            startStr = endStr = line.substr(0, j);
        }

        i = line.find_first_not_of(whitespaces, j);
        if(i == string::npos)
        {
            cerr << "error: no category defined in line: " << line << endl;
            return false;
        }
        j = line.find_first_of(whitespaces, i);
        if(j == string::npos)
            catStr = line.substr(i);
        else
            catStr = line.substr(i, j-i);

        if(! strToProperty(catStr, cat))
        {
            cerr << "error: unknown category (" << catStr << ") defined in line: " << line << endl;
            return false;
        }

        try
        {
            start = atoucs2(startStr.c_str());
            end = atoucs2(endStr.c_str());
        }
        catch(runtime_error& e)
        {
            cerr << e.what() << endl;
            cerr << "error: in converting value in line: " << line << endl;
            return false;
        }

        Range r;
        r.start_ = start;
        r.end_ = end;
        r.property_ = cat;
        rangeVec.push_back(r);

#if LANGID_DEBUG_PRINT_LOAD
        cout << startStr << "\t" << endStr << "\t" << catStr << endl;
        cout << "0x" << setw(4) << setfill('0') << hex << uppercase << start << "\t0x" << setw(4) << setfill('0') << end << "\t" << cat << endl;
        cout << endl;
#endif
    }

    for(typename vector<Range>::const_iterator it=rangeVec.begin(); it!=rangeVec.end(); ++it)
    {
        for(unsigned short i=it->start_; i<=it->end_; ++i)
        {
            table_[i] = it->property_;
        }
    }

    return true;
}

template<class T, class BIN_T> bool PropertyTable<T, BIN_T>::loadTable(const char* fileName)
{
#if LANGID_DEBUG_PRINT_LOAD
    cout << ">>> PropertyTable::loadTable(): " << fileName << endl;
#endif

    if(! fileName)
    {
        cerr << "error: NULL string is passed as file name." << endl;
        return false;
    }

    ifstream ifs(fileName);
    if(! ifs)
    {
        cerr << "error: could not open file " << fileName << endl;
        return false;
    }

    string line;
    T cat;
    bool isCat = false;
    
    // category as the 1st line
    while(getline(ifs, line))
    {
        if(line.empty() || line[0] == '#')
            continue;

        if(! strToProperty(line, cat))
        {
            cerr << "error: unknown category (" << line << ") defined in the first line: " << line << endl;
            return false;
        }

        isCat = true;
        break;
    }

    if(! isCat)
    {
        cerr << "error: no category is defined in file " << fileName << endl;
        return false;
    }

#if LANGID_DEBUG_PRINT_LOAD
    cout << "category: " << line << ", " << cat << endl;
#endif

    string indexStr;
    unsigned short index;
    vector<unsigned short> indexVec;
    size_t i;
    const char* whitespaces = " \t";
    
    // UCS2 index in each line
    while(getline(ifs, line))
    {
        if(line.empty() || line[0] == '#')
            continue;

        // 0x3005   # 々
        i = line.find_first_of(whitespaces);
        if(i == string::npos)
            indexStr = line;
        else
            indexStr = line.substr(0, i);

        try
        {
            index = atoucs2(indexStr.c_str());
        }
        catch(runtime_error& e)
        {
            cerr << e.what() << endl;
            cerr << "error: in converting value in line: " << line << endl;
            return false;
        }

        indexVec.push_back(index);

#if LANGID_DEBUG_PRINT_LOAD
        cout << indexStr << "\t" << "0x" << setw(4) << setfill('0') << hex << uppercase << index << endl;
#endif
    }

    for(vector<unsigned short>::const_iterator it=indexVec.begin(); it!=indexVec.end(); ++it)
    {
        table_[*it] = cat;
    }

    return true;
}

template<class T, class BIN_T> bool PropertyTable<T, BIN_T>::strToProperty(const std::string& str, T& property) const
{
    typename map<string, T>::const_iterator it = strMap_.find(str);
    if(it != strMap_.end())
    {
        property = it->second;
        return true;
    }

    return false;
}

template<class T, class BIN_T> bool PropertyTable<T, BIN_T>::propertyToStr(const T& property, std::string& str) const
{
    for(typename map<string, T>::const_iterator it=strMap_.begin(); it!=strMap_.end(); ++it)
    {
        if(it->second == property)
        {
            str = it->first;
            return true;
        }
    }

    return false;
}

/**
 * Format of binary file.
 * UInt32_type: table_.size()
 * UInt32_type: sizeof(Binary_type)
 * // for table_.size()
 * Binary_type: table_[i]
 */
template<class T, class BIN_T> bool PropertyTable<T, BIN_T>::saveBinary(std::ofstream& ofs) const
{
    if(! ofs)
    {
        cerr << "error: the output stream is invalid. " << endl;
        return false;
    }

    UInt32_type num = table_.size();
    ofs.write(reinterpret_cast<const char*>(&num), sizeof(UInt32_type));

    UInt32_type unitLen = sizeof(Binary_type);
    ofs.write(reinterpret_cast<const char*>(&unitLen), sizeof(UInt32_type));

    Binary_type binVal;
    for(unsigned int i=0; i<num; ++i)
    {
        binVal = static_cast<Binary_type>(table_[i]);
        ofs.write(reinterpret_cast<const char*>(&binVal), unitLen);
    }

    return true;
}

template<class T, class BIN_T> bool PropertyTable<T, BIN_T>::loadBinary(std::ifstream& ifs)
{
#if LANGID_DEBUG_PRINT_LOAD
    cout << ">>> PropertyTable<T, BIN_T>::loadBinary()" << endl;
#endif

    if(! ifs)
    {
        cerr << "error: the input stream is invalid." << endl;
        return false;
    }

    UInt32_type num;
    if(! ifs.read(reinterpret_cast<char*>(&num), sizeof(UInt32_type)))
    {
        cerr << "error: invalid input stream in reading." << endl;
        return false;
    }
#if LANGID_DEBUG_PRINT_LOAD
    cout << "table size: " << num << endl;
#endif
    if(num <= 0)
    {
        cerr << "error: the table size " << num << " should be positive in binary property table." << endl;
        return false;
    }

    UInt32_type unitLen;
    if(! ifs.read(reinterpret_cast<char*>(&unitLen), sizeof(UInt32_type)))
    {
        cerr << "error: invalid input stream in reading." << endl;
        return false;
    }
#if LANGID_DEBUG_PRINT_LOAD
    cout << "unit length: " << unitLen << endl;
    cout << endl;
#endif
    if(unitLen != sizeof(Binary_type))
    {
        cerr << "the unit length " << unitLen << " should be equal to sizeof(Binary_type) " << sizeof(Binary_type) << ".)";
        return false;
    }

    Binary_type binVal;
    for(unsigned int i=0; i<num; ++i)
    {
        if(! ifs.read(reinterpret_cast<char*>(&binVal), unitLen))
        {
            cerr << "error: invalid input stream in reading." << endl;
            return false;
        }
        table_[i] = static_cast<Value_type>(binVal);
    }

    return true;
}

NS_ILPLIB_LANGID_END
