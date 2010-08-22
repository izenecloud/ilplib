#ifndef _LA_TEST_DEF_H_
#define _LA_TEST_DEF_H_

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

#define TO_STRING_(x) #x "/knowledge"
#define TO_STRING(x) TO_STRING_(x)
#define KMA_KNOWLEDGE TO_STRING(WISEKMA)

#define TO_CMA_STRING_(x) #x "/db/icwb/utf8"
#define TO_CMA_STRING(x) TO_CMA_STRING_(x)
#define CMA_KNOWLEDGE TO_CMA_STRING(IZENECMA)

class KnowledgeDir {
public:
    KnowledgeDir(const path & orig, const path & tmp)
    : origdir_(orig), tmpdir_(current_path()/tmp)
    {
        remove_all(tmpdir_);
        create_directories(tmpdir_);
        directory_iterator end_itr; // default construction yields past-the-end
        for ( directory_iterator itr( origdir_ ); itr != end_itr; ++itr )
        {
            if(!is_directory(itr->status()) ) {
                  copy_file(itr->path(), tmpdir_/itr->leaf());
            }
        }
    }

    ~KnowledgeDir()
    {
        path parent = tmpdir_.parent_path();
        while(current_path() != parent) {
            tmpdir_ = parent;
            parent = parent.parent_path();
        }
        remove_all(tmpdir_);
    }

    string getDir()
    {
        return tmpdir_.string();
    }

    void appendFile(const string& filename, const string& content)
    {
        ofstream of( (tmpdir_/filename).string().c_str(), ios_base::app);
        of << content;
        of.close();
    }

private:
    path origdir_;
    path tmpdir_;
};

#endif  //_LA_TEST_DEF_H_
