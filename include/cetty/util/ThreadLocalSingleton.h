#ifndef CETTY_UTIL_THREADLOCALSINGLETON_H
#define CETTY_UTIL_THREADLOCALSINGLETON_H

// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <boost/function.hpp>
#include <boost/thread/tss.hpp>
#include <boost/noncopyable.hpp>

namespace cetty {
namespace util {

template<typename T>
class ThreadLocalSingleton : boost::noncopyable {
public:
    typedef boost::function<T*(void)> Creater;

public:
    static T& instance() {
        if (!value_.get()) {
            value_.reset(creater_());
        }
        return *value_;
    }

    static T* pointer() {
        if (!value_.get()) {
            value_.reset(creater_());
        }

        return value_.get();
    }

    static void setCreater(const Creater& creater) {
        if (creater) {
            creater_ = creater;
        }
    }

private:
    ThreadLocalSingleton();
    ~ThreadLocalSingleton();

    static T* create() {
        return new T();
    }

private:
    static Creater creater_;
    static boost::thread_specific_ptr<T> value_;
};

template<typename T>
boost::thread_specific_ptr<T> ThreadLocalSingleton<T>::value_;

template<typename T>
typename ThreadLocalSingleton<T>::Creater ThreadLocalSingleton<T>::creater_ = &ThreadLocalSingleton<T>::create;

}
}

#endif //#ifndef CETTY_UTIL_THREADLOCALSINGLETON_H
