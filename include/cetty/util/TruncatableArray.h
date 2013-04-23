#if !defined(CETTY_UTIL_TRUNCATABLEARRAY_H)
#define CETTY_UTIL_TRUNCATABLEARRAY_H

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

#include <boost/array.hpp>
#include <boost/static_assert.hpp>

namespace cetty { namespace util {

template<class T, std::size_t N>
class TruncatableArray : public boost::array<T, N> {
public:
    // type definitions
    typedef T*             iterator;
    typedef const T*       const_iterator;
    typedef T&             reference;
    typedef const T&       const_reference;

public:
    TruncatableArray()
        : truncatedSize_(0) {
        BOOST_STATIC_ASSERT(N > 0);
    }

    ~TruncatableArray() {
    }

public:
    int truncatedSize() const {
        return truncatedSize_;
    }
    
    iterator end() {
        return boost::array<T, N>::elems + truncatedSize_;
    }

    const_iterator end() const {
        return boost::array<T, N>::elems + truncatedSize_;
    }

    reference back()  {
        BOOST_ASSERT(truncatedSize_);
        return boost::array<T, N>::elems[truncatedSize_-1];
    }

    const_reference back() const {
        BOOST_ASSERT(truncatedSize_);
        return boost::array<T, N>::elems[truncatedSize_-1];
    }

    void push_back(const_reference value) {
        boost::array<T, N>::elems[truncatedSize_] = value;
        ++truncatedSize_;
    }

private:
    int truncatedSize_;
};

}}

#endif //#if !defined(CETTY_UTIL_TRUNCATABLEARRAY_H)
