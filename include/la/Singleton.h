#ifndef _LA_SINGLETON_H_
#define _LA_SINGLETON_H_

#include <iostream>
#include <memory>
#include <boost/serialization/singleton.hpp>

namespace la
{

    template <class T, class P = void>
    class Singleton: public boost::serialization::singleton<T>
    {
        public:
            BOOST_DLLEXPORT static T & getInstance(P* p = 0){
                static T t(p);
                return t;
            }
            friend class boost::serialization::singleton<T>;
    };


}




#endif // _LA_SINGLETON_H_
