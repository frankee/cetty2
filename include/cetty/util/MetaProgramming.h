#if !defined(CETTY_UTIL_METAPROGRAMMING_H)
#define CETTY_UTIL_METAPROGRAMMING_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <map>
#include <string>
#include <vector>

namespace cetty {
namespace util {

template <typename T>
struct IsReference {
    enum {
        VALUE = 0
    };
};

template <typename T>
struct IsReference<T&> {
    enum {
        VALUE = 1
    };
};

template <typename T>
struct IsReference<const T&> {
    enum {
        VALUE = 1
    };
};

template <typename T>
struct IsPointer {
    enum {
        VALUE = 0
    };
};

template <typename T>
struct IsPointer<T*> {
    enum {
        VALUE = 1
    };
};

template <typename T>
struct IsPointer<T const*> {
    enum {
        VALUE = 1
    };
};

template<typename T>
struct IsString {
    enum {
        VALUE = 0
    };
};

template<>
struct IsString<std::string> {
    enum {
        VALUE = 1
    };
};

template<>
struct IsString<std::string &> {
    enum {
        VALUE = 1
    };
};

template<>
struct IsString<std::string const &> {
    enum {
        VALUE = 1
    };
};

template<typename T>
struct IsVector {
    enum {
        VALUE = 0
    };
};

template<typename T>
struct IsVector<std::vector<T> > {
    enum {
        VALUE = 1
    };
};

template<typename T>
struct IsVector<std::vector<T>& > {
    enum {
        VALUE = 1
    };
};

template<typename T>
struct IsVector<const std::vector<T>& > {
    enum {
        VALUE = 1
    };
};

template<typename T>
struct IsMap {
    enum {
        VALUE = 0
    };
};

template<typename K, typename V>
struct IsMap<std::map<K, V> > {
    enum {
        VALUE = 1
    };
};

template<typename K, typename V>
struct IsMap<std::map<K, V>& > {
    enum {
        VALUE = 1
    };
};

template<typename K, typename V>
struct IsMap<const std::map<K, V>& > {
    enum {
        VALUE = 1
    };
};

/**
 * Use this struct to determine if a template type is a const type.
 */
template <typename T>
struct IsConst {
    enum {
        VALUE = 0
    };
};


template <typename T>
struct IsConst<const T&> {
    enum {
        VALUE = 1
    };
};


template <typename T>
struct IsConst<const T> {
    enum {
        VALUE = 1
    };
};

/**
 * Use the type wrapper if you want to decouple constness and references from template types.
 */
template <typename T>
struct TypeWrapper {
    typedef T Type;
    typedef const T ConstType;
    typedef T& RefType;
    typedef const T& ConstRefType;
};

template <typename T>
struct TypeWrapper<const T> {
    typedef T Type;
    typedef const T ConstType;
    typedef T& RefType;
    typedef const T& ConstRefType;
};

template <typename T>
struct TypeWrapper<const T&> {
    typedef T Type;
    typedef const T ConstType;
    typedef T& RefType;
    typedef const T& ConstRefType;
};

template <typename T>
struct TypeWrapper<T&> {
    typedef T Type;
    typedef const T ConstType;
    typedef T& RefType;
    typedef const T& ConstRefType;
};

template<bool Condition, typename Then, typename Else>
struct IF {
    typedef Then Result;
};

template<typename Then, typename Else>
struct IF <false, Then, Else> {
    typedef Else Result;
};

}
}

#endif //#if !defined(CETTY_UTIL_METAPROGRAMMING_H)

// Local Variables:
// mode: c++
// End:
