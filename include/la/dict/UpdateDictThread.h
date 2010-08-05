/**
 * @file    UpdatableDictThread.h
 * @author  Vernkin
 * @date    Feb 1, 2009
 * @details
 *  Thread to update dictionary from time to time
 */
#ifndef UPDATEDICTTHREAD_H_
#define UPDATEDICTTHREAD_H_

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <la/dict/UpdatableDict.h>
#include <la/dict/PlainDictionary.h>
#include <map>
#include <util/ThreadModel.h>

namespace la
{

/**
 * @brief get the last modified time of specific file, represented in seconds
 * FIXME only support linux/unix now
 */
long getFileLastModifiedTime( const char* path );

/**
 * \brief The class contains destination path dictionary's source and related dictionaries.
 *
 * DictSource doesn't contain any lock and UpdateDictThread is in charge of it.
 */
class DictSource
{
public:
    DictSource( const std::string& destPath );

    /**
     * Add related dictionary
     */
    void addRelatedDict( const boost::shared_ptr< UpdatableDict >& relatedDict );

    /**
     * Perform a update operation
     * \return how many UpdatableDicts are fail to update
     */
    int update();

private:
    /** Destination Path */
    std::string destPath_;

    /** The previous record of Last Modified Time */
    long lastModifiedTime_;

    /** Related Dictionaries */
    std::vector< boost::shared_ptr< UpdatableDict > > relatedDicts_;
};

/**
 * \brief Thread to update dictionary from time to time
 */
class UpdateDictThread
{
public:
    typedef std::map<std::string,boost::shared_ptr<DictSource> > MapType;

    UpdateDictThread();

    virtual ~UpdateDictThread();

    /**
     * Add related dictionary with specific path
     * \param path the dictionary path
     * \param dict the related dictionary
     */
    void addRelatedDict( const char* path, const boost::shared_ptr< UpdatableDict >& dict );

    /**
     * Utility function to create plain dictionary. These Dictionary is read-only as
     * updating from dictionary source occurs from time to time
     * \param path the dictionary path
     * \param encoding the encoding of this dictionary
     * \param ignoreNoExistFile if true, the files not exists won't cause error
     */
    boost::shared_ptr< PlainDictionary > createPlainDictionary(
            const char* path,
            izenelib::util::UString::EncodingType encoding,
            bool ignoreNoExistFile = false );

    /**
     * Get the check interval ( in seconds )
     */
    unsigned int getCheckInterval()
    {
        return checkInterval_;
    }

    /**
     * Set the check interval ( in seconds )
     */
    void setCheckInterval( unsigned int checkInterval )
    {
        checkInterval_ = checkInterval;
    }

    /**
     * Whether the thread has started
     */
    bool isStarted()
    {
        return isStarted_;
    }

    /**
     * Perform updating
     */
    void update();

    /**
     * Infinite loop for the thread
     */
    void run();

    /**
     * start the thread
     * \return whether start successfully. Return false if has started already
     */
    bool start();

public:
    static UpdateDictThread staticUDT;

private:
    /** check interval */
    unsigned int checkInterval_;

    /** Whether the thread has started */
    bool isStarted_;

    /** path -> DictSource */
    MapType map_;

    /** Read Write Lock */
    izenelib::util::ReadWriteLock lock_;
};

}
#endif /* UPDATEDICTTHREAD_H_ */
