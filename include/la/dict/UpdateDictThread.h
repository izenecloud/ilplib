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
 * \brief Thread to update dictionary from time to time
 */
class UpdateDictThread
{
public:
    typedef struct {
        long lastModifiedTime_;
        boost::shared_ptr<UpdatableDict> relatedDict_;
    } DictSource;

    typedef std::map<std::string, DictSource> MapType;

    UpdateDictThread();

    virtual ~UpdateDictThread();

    /**
     * Add related dictionary with specific path
     * \param path the dictionary path
     * \param dict the related dictionary
     */
    boost::shared_ptr< UpdatableDict > addRelatedDict( const char* path, const boost::shared_ptr< UpdatableDict >& dict );

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
    int update();

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
