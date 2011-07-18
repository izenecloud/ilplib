#include <la/dict/UpdatableSynonymContainer.h>
#include <iostream>
namespace la
{

int UpdatableSynonymContainer::update( const char* path, unsigned int lastModifiedTime )
{
    //just update the current curModifiedTime_
    curModifiedTime_ = lastModifiedTime;
	std::cout<<"lastModifiedTime_ "<<lastModifiedTime_<<" lastModifiedTime "<<lastModifiedTime<<std::endl;
    if(lastModifiedTime_ != lastModifiedTime)
    {
        // perform updating
        pSynonymContainer_->clear( false );
        pSynonymContainer_->loadSynonym( path );
        lastModifiedTime_ = lastModifiedTime;
        return 0;
    }
    else return 1;
}

}
