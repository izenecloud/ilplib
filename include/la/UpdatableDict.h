/**
 * @file    UpdatableDict.h
 * @author  Vernkin
 * @date    Feb 1, 2009
 * @details
 *  Dictionary that can be updated from time to time
 */

#ifndef UPDATABLEDICT_H_
#define UPDATABLEDICT_H_

namespace la
{

/**
 * @brief Dictionary that can be updated from time to time
 */
class UpdatableDict
{
public:
    virtual ~UpdatableDict(){}

    /**
     * @bried Perform updating
     * @param path the dictionary path
     * @param lastModifiedTime last modified time
     * @return 0 indicates success and others indicates fails
     */
    virtual int update( const char* path, unsigned int lastModifiedTime ) = 0;

};

}
#endif /* UPDATABLEDICT_H_ */
