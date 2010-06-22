/**
 * @file    UpdatableDictThread.h
 * @author  Vernkin
 * @date    Feb 1, 2009
 * @details
 *  Thread to update dictionary from time to time
 */

#include <la/UpdateDictThread.h>

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

DictSource::DictSource( const string& destPath )
    : destPath_( destPath )
{
    lastModifiedTime_ = getFileLastModifiedTime( destPath.c_str() );
}

int DictSource::update()
{
    long curModifiedTime = getFileLastModifiedTime( destPath_.c_str() );
    if( curModifiedTime == lastModifiedTime_ )
        return 0;

    int failedCount = 0;
    //update all dictionary
    for( vector< shared_ptr<UpdatableDict> >::iterator itr = relatedDicts_.begin();
            itr != relatedDicts_.end(); ++ itr)
    {
        int ret = static_cast<UpdatableDict*>(itr->get())->update( destPath_.c_str(), curModifiedTime );
        if( ret != 0 )
            ++failedCount;
    }

#ifdef DEBUG_UDT
    cout<<"Update "<<destPath_<<", "<<failedCount<<"/"<<relatedDicts_.size()<<" failed."<<endl;
#endif
    lastModifiedTime_ = curModifiedTime;
    return failedCount;
}

void DictSource::addRelatedDict( const shared_ptr< UpdatableDict >& relatedDict )
{
    relatedDicts_.push_back( relatedDict );
}

UpdateDictThread::UpdateDictThread()
    : checkInterval_( DEFAULT_CHECK_INTERVAL ), isStarted_( false )
{
}

UpdateDictThread::~UpdateDictThread()
{
}

UpdateDictThread UpdateDictThread::staticUDT;

void UpdateDictThread::addRelatedDict( const char* path,
        const shared_ptr< UpdatableDict >& dict )
{
    ScopedWriteLock<ReadWriteLock> swl( lock_ );
    string pathStr( path );
    MapType::iterator itr = map_.find( path );
    if( itr == map_.end() )
    {
        shared_ptr<DictSource> dsPtr;
        dsPtr.reset( new DictSource( pathStr) );
        map_[ pathStr ] = dsPtr;
        itr = map_.find( pathStr );
    }
    itr->second.get()->addRelatedDict( dict );
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

void UpdateDictThread::update()
{
    ScopedWriteLock<ReadWriteLock> swl( lock_ );
    for( MapType::iterator itr = map_.begin(); itr != map_.end(); ++itr )
    {
        itr->second.get()->update();
    }
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
