/**
 * @file    UpdatableDictThread.h
 * @author  Vernkin
 * @date    Feb 1, 2009
 * @details
 *  Thread to update dictionary from time to time
 */

#include <la/dict/UpdateDictThread.h>

// for getLastModifiedTime
#include <sys/stat.h>

#include <boost/thread/thread.hpp>

//#define DEBUG_UDT

#ifdef DEBUG_UDT
    #include <iostream>
    using namespace std;

    #define DEFAULT_CHECK_INTERVAL 2
#else
    #define DEFAULT_CHECK_INTERVAL 600
#endif

using izenelib::util::UString;

namespace la
{

/**
 * @brief get the last modified time of specific file, represented in seconds
 * FIXME only support linux/unix now
 */
long getFileLastModifiedTime( const char* path )
{
    struct stat attrib;   // create a file attribute structure
    stat( path , &attrib );  // get the attributes of specific file
    return static_cast<long>( attrib.st_mtime );
}

UpdateDictThread::UpdateDictThread()
    : checkInterval_( DEFAULT_CHECK_INTERVAL ), isStarted_( false )
{
}

UpdateDictThread::~UpdateDictThread()
{
}

UpdateDictThread UpdateDictThread::staticUDT;

shared_ptr< UpdatableDict > UpdateDictThread::addRelatedDict( const char* path,
        const shared_ptr< UpdatableDict >& dict )
{
    ScopedWriteLock<ReadWriteLock> swl( lock_ );
    string pathStr( path );
    MapType::iterator itr = map_.find( pathStr );
    if ( itr == map_.end() )
    {
        if ( dict.get() )
        {
            DictSource ds;
            ds.lastModifiedTime_ = getFileLastModifiedTime(path);
            ds.relatedDict_ = dict;
            map_[ pathStr ] = ds;
        }
        return dict;
    }
    else
        return itr->second.relatedDict_;
}

shared_ptr< PlainDictionary > UpdateDictThread::createPlainDictionary(
        const char* path,
        UString::EncodingType encoding,
        bool ignoreNoExistFile )
{
    shared_ptr< PlainDictionary > pdPtr;
    pdPtr.reset( new PlainDictionary( encoding ) );
    pdPtr->loadDict( path, ignoreNoExistFile );
    addRelatedDict( path, pdPtr );
    return pdPtr;
}

int UpdateDictThread::update()
{
    ScopedWriteLock<ReadWriteLock> swl( lock_ );
    int failedCount = 0;
    for( MapType::iterator itr = map_.begin(); itr != map_.end(); ++itr )
    {
        long curModifiedTime = getFileLastModifiedTime( itr->first.c_str() );
        if( curModifiedTime == itr->second.lastModifiedTime_ )
            continue;

        //update all dictionary
        int ret = static_cast<UpdatableDict*>(itr->second.relatedDict_.get())->update( itr->first.c_str(), curModifiedTime );
        if( ret != 0 )
            ++failedCount;

#ifdef DEBUG_UDT
        cout << "Update " << itr->first << " failed." << endl;
#endif
        itr->second.lastModifiedTime_ = curModifiedTime;
    }
    return failedCount;
}

void UpdateDictThread::run()
{
    while( true )
    {
        boost::this_thread::sleep( boost::posix_time::seconds( checkInterval_ ) );
        update();
    }
}

bool UpdateDictThread::start()
{
    ScopedWriteLock<ReadWriteLock> swl( lock_ );
    if( isStarted_ )
        return false;
    boost::thread( boost::bind( &UpdateDictThread::run, this ) );
    isStarted_ = true;
    return true;
}

}
