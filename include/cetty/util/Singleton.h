// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef CETTY_UTIL_SINGLETON_H
#define CETTY_UTIL_SINGLETON_H

#include <stdlib.h> // atexit
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/once.hpp>

namespace cetty {
namespace util {

template<typename T>
class Singleton : boost::noncopyable {
public:
    typedef boost::function<T*(void)> Creater;

public:
    static T& instance() {
        boost::call_once(&Singleton::init, onceFlag_);
        return *value_;
    }

    static T* pointer() {
      return &instance();
    }

    static void setCreater(const Creater& creater) {
        if (creater) {
            creater_ = creater;
        }
    }

private:
    Singleton();
    ~Singleton();

    static void init() {
        value_ = creater_();
        ::atexit(destroy);
    }

    static T* create() {
        return new T();
    }

    static void destroy() {
        typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
        T_must_be_complete_type dummy; (void) dummy;

        delete value_;
    }

private:
    static T* value_;
    static Creater creater_;
    static boost::once_flag onceFlag_;
};

template<typename T>
T* Singleton<T>::value_ = NULL;

template<typename T>
typename Singleton<T>::Creater Singleton<T>::creater_ = &Singleton<T>::create;

template<typename T>
boost::once_flag Singleton<T>::onceFlag_ = BOOST_ONCE_INIT;

}
}

#endif //#ifndef CETTY_UTIL_SINGLETON_H

