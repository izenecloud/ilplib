/*
 * Dictionary.h
 *
 *  Created on: 2009-7-9
 *      Author: zhjay
 */

#ifndef _LA_DICTIONARY_H_
#define _LA_DICTIONARY_H_

#include <la/Singleton.h>

#ifdef USE_WISEKMA
    #include <wk_knowledge.h>
    #include <wk_stopword.h>
#endif

#include <util/ustring/UString.h>
#include <set>
#include <fstream>

#ifdef USE_IZENECMA
	#include <knowledge.h>
	#include <cma_factory.h>
#endif

namespace la
{
#ifdef USE_WISEKMA
    //class KMAKnowledge: public Singleton<KMAKnowledge, std::string>
    class KMAKnowledge: public Singleton<KMAKnowledge, const char >
    {
        private:
            //KMAKnowledge(std::string* knowledgePath)
            KMAKnowledge(const char * knowledgePath)
                :pKnowledge_(NULL)
            {
                if( knowledgePath != NULL )
                {
                    std::cout << "Loading KMA KMAKnowledge..." ;

                    int flag = 1;
                    pKnowledge_ = kmaOrange::WK_Knowledge::createObject();

                    flag *= pKnowledge_->loadKnowledge( knowledgePath );	// Load data from knowledge file by default
                    pKnowledge_->loadUserNoun( knowledgePath );             // Default filename is "user.dic"    
                    pKnowledge_->loadPreAnalysis( knowledgePath );          // Default filename is "preanal.dic" 

                    if( flag == 0 )
                    {
                        string msg = "Failed to load KMA knowledge from path: ";
                        msg.append( knowledgePath );
                        throw std::logic_error( msg );
                    }
                    std::cout << "finish!" << std::endl;

                }
            }
            ~KMAKnowledge()
            {
                std::cout << "KMAKnowledge::~KMAKnowledge()" << std::endl;
                if( pKnowledge_ != NULL )
                    delete pKnowledge_;
            }
        public:
            kmaOrange::WK_Knowledge* pKnowledge_;
            //friend class Singleton<KMAKnowledge, std::string>;
            friend class Singleton<KMAKnowledge, const char>;
            //	DECLARE_SINGLETON_CLASS(KMAKnowledge,std::string);
    };
#endif

#ifdef USE_IZENECMA
    class CMAKnowledge: public Singleton<CMAKnowledge, const char >
    {
        private:
            //KMAKnowledge(std::string* knowledgePath)
			CMAKnowledge(const char * pKnowledgePath)
                :pKnowledge_(NULL)
            {
                if( pKnowledgePath != NULL )
                {
                    std::cout << "Loading CMA CMAKnowledge..." ;
                    std::string knowledgePath = pKnowledgePath;
                    int flag = 1;
                    pKnowledge_ = cma::CMA_Factory::instance()->createKnowledge();

                	if(knowledgePath[ knowledgePath.length() -  1] != '/' )
                		knowledgePath += "/";
                	size_t last = knowledgePath.find_last_of('/');
                	size_t first = knowledgePath.find_last_of('/', last-1);
                	string encodeStr = knowledgePath.substr(first+1, last-first-1);

                	flag *= pKnowledge_->loadModel( encodeStr.c_str() , knowledgePath.c_str() );

                    if( flag == 0 )
                    {
                        string msg = "Failed to load CMA knowledge from path: ";
                        msg.append( knowledgePath );
                        throw std::logic_error( msg );
                    }
                    std::cout << "finish!" << std::endl;

                }
            }
            ~CMAKnowledge()
            {
                std::cout << "CMAKnowledge::~CMAKnowledge()" << std::endl;
                delete pKnowledge_;
            }
        public:
            cma::Knowledge* pKnowledge_;
            //friend class Singleton<KMAKnowledge, std::string>;
            friend class Singleton<CMAKnowledge, const char>;
            //	DECLARE_SINGLETON_CLASS(KMAKnowledge,std::string);
    };

    class CMANoModelKnowledge: public Singleton<CMANoModelKnowledge, const char >
    {
        private:
            //KMAKnowledge(std::string* knowledgePath)
        CMANoModelKnowledge(const char * pKnowledgePath)
                :pKnowledge_(NULL)
            {
                if( pKnowledgePath != NULL )
                {
                    std::cout << "Loading CMA CMANoModelKnowledge..." ;
                    std::string knowledgePath = pKnowledgePath;
                    int flag = 1;
                    pKnowledge_ = cma::CMA_Factory::instance()->createKnowledge();

                    if(knowledgePath[ knowledgePath.length() -  1] != '/' )
                        knowledgePath += "/";
                    size_t last = knowledgePath.find_last_of('/');
                    size_t first = knowledgePath.find_last_of('/', last-1);
                    string encodeStr = knowledgePath.substr(first+1, last-first-1);

                    flag *= pKnowledge_->loadModel( encodeStr.c_str() , knowledgePath.c_str(), false );

                    if( flag == 0 )
                    {
                        string msg = "Failed to load CMA knowledge from path: ";
                        msg.append( knowledgePath );
                        throw std::logic_error( msg );
                    }
                    std::cout << "finish!" << std::endl;

                }
            }
            ~CMANoModelKnowledge()
            {
                std::cout << "CMANoModelKnowledge::~CMANoModelKnowledge()" << std::endl;
                delete pKnowledge_;
            }
        public:
            cma::Knowledge* pKnowledge_;
            //friend class Singleton<KMAKnowledge, std::string>;
            friend class Singleton<CMANoModelKnowledge, const char>;
            //  DECLARE_SINGLETON_CLASS(KMAKnowledge,std::string);
    };
#endif


    struct StopDicParam{
        StopDicParam(std::string p, izenelib::util::UString::EncodingType e): path_(p), etype_(e)
        {}
        std::string path_;
        izenelib::util::UString::EncodingType etype_;
    };

    class StopWordDictionay: public Singleton<StopWordDictionay, StopDicParam>
    {
        private:
            std::set<izenelib::util::UString> stopDic_; ///< stop word dictionary

            StopWordDictionay(StopDicParam* param)
            {
                std::cout << "Loading stop word dictionary..." ;
                std::ifstream ifs(param->path_.c_str());
                if(ifs.fail()){
                    std::cerr << "File open error:" << param->path_ << std::endl;
                    throw std::logic_error("File open error");
                }
                char buff[2000];
                while( !ifs.eof() ){
                    ifs.getline( buff, 2000 );
                    izenelib::util::UString str(buff, param->etype_);
                    if(!str.empty())
                        stopDic_.insert(str);
                }
                ifs.close();
                std::cout << "finish!" << std::endl;
            }


        public:
            bool found(izenelib::util::UString t){
                return  (stopDic_.find(t) != stopDic_.end());
            }

            friend class Singleton<StopWordDictionay, StopDicParam>;

            //	DECLARE_SINGLETON_CLASS(StopWordDictionay, StopDicParam);
    };


}
#endif /* _LA_DICTIONARYFACTORY_H_ */
