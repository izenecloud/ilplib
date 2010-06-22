/**
 * @file    PlainDictionary.h
 * @author  Vernkin
 * @date    Dec 14, 2009
 * @details
 *  Plain Dictionary only contains a set of words, without any extra information. And can Save to/Load
 *  from the file.
 */

#ifndef PLAINDICTIONARY_H_
#define PLAINDICTIONARY_H_

#include <string>
#include <set>


#include <util/ThreadModel.h>
#include <util/ustring/vector_string.hpp>

#include <la/UpdatableDict.h>
#include <iostream>

#define USE_STD_MAP

#ifdef USE_STD_MAP
    #include <map>
#else
    #include <3rdparty/am/rde_hashmap/hash_map.h>
#endif

namespace la
{

/**
 * \brief Plain Dictionary only contains a set of words, without any extra information. And can Save to/Load
 *  from the file.
 */
class PlainDictionary : public UpdatableDict
{
public:

	PlainDictionary( wiselib::UString::EncodingType encoding = wiselib::UString::UTF_8 );

	~PlainDictionary();

#ifdef USE_STD_MAP
    typedef std::map<wiselib::UString,bool> DictType;
#else
    typedef rde::hash_map<wiselib::UString,bool> DictType;
#endif

	/**
	 * Load the dictionary from the specific file
	 * @param srcFile the source file, with each word per line.
	 * @param ignoreNoExistFile whether ignore when srcFile is not exists, default is false.
	 * @return 0 indicates successes and 1 fails.
	 */
	int loadDict( const char* srcFile, bool ignoreNoExistFile = false );

	/**
     * Reload the dictionary from the specific file, clear the current data before load.
     * @param srcFile the source file, with each word per line.
     * @param ignoreNoExistFile whether ignore when srcFile is not exists, default is false.
     * @return 0 indicates successes and 1 fails.
     */
    int reloadDict( const char* srcFile, bool ignoreNoExistFile = false )
    {
        {
            ScopedWriteLock<ReadWriteLock> swl( lock_ );
            words_.clear();
        }
        return loadDict( srcFile, ignoreNoExistFile );
    }

	/**
	 * Add new word to the dictionary
	 * \param word the new word
	 */
	inline void insert( const wiselib::UString& word )
	{
	    ScopedWriteLock<ReadWriteLock> swl( lock_ );
	    words_[ word ] = true;
	}

	/**
	 * Remove the specific from the dictionary
	 * \param word the word to be removed
	 */
	inline void remove( const wiselib::UString& word )
	{
	    ScopedWriteLock<ReadWriteLock> swl( lock_ );
	    words_.erase( word );
	}

	/**
	 * Whether contains specific word
	 * @param the specific word
	 */
	inline bool contain( const wiselib::UString& word )
	{
	    ScopedReadLock<ReadWriteLock> srl( lock_ );
	    return words_.find( word ) != words_.end();
	}


	inline int size()
	{
	    return words_.size();
	}

	/**
     * @bried Perform updating
     * @param path the dictionary path
     * @param lastModifiedTime last modified time
     * @return 0 indicates success and others indicates fails
     */
    virtual int update( const char* path, unsigned int lastModifiedTime )
    {
        return reloadDict( path, ignoreNoExistFile_ );
    }

    /**
     * Display the content in the Dictionary, with per word per line
     * \param out output stream , default is cout
     */
    void display( ostream& out = std::cout );


    /**
     * Whether contains specific word ( No lock compared with contain() )
     * Invoke it before use getLock()
     * @param the specific word
     */
    inline bool containNoLock( const wiselib::UString& word )
    {
        return words_.find( word ) != words_.end();
    }

    /**
     * Invoke it before XXXXNoLock functions
     * \return the ReadWriteLock of this instance
     */
    inline izenelib::util::ReadWriteLock& getLock()
    {
        return lock_;
    }

private:
	/** The Words Collection */
	DictType words_;

	/** The encoding for the Dictionary file */
    wiselib::UString::EncodingType encoding_;

	/** The latest destination path */
    std::string destPath_;

	/** Read Write Lock */
	izenelib::util::ReadWriteLock lock_;

	/** Whether ignore not exist files */
	bool ignoreNoExistFile_;
};

}
#endif /* PLAINDICTIONARY_H_ */
