#if !defined(CETTY_UTIL_BAREPOINTER_H)
#define CETTY_UTIL_BAREPOINTER_H

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
//
//  intrusive_ptr.hpp
//
//  Copyright (c) 2001, 2002 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/intrusive_ptr.html for documentation.
//

#include <boost/assert.hpp>

namespace cetty {
namespace util {

template<class T>
class BarePointer {
private:
    typedef BarePointer this_type;

public:
    typedef T element_type;

    BarePointer(): px(0) {
    }

    BarePointer(T* rhs) : px(rhs) {
    }

    BarePointer(BarePointer const& rhs): px(rhs.px) {
    }

    ~BarePointer() {
    }

    // Move support

#if defined( BOOST_HAS_RVALUE_REFS )

    BarePointer(BarePointer && rhs): px(rhs.px) {
        rhs.px = 0;
    }

    BarePointer& operator=(BarePointer && rhs) {
        this_type(static_cast< BarePointer && >(rhs)).swap(*this);
        return *this;
    }

#endif

    BarePointer& operator=(BarePointer const& rhs) {
        this_type(rhs).swap(*this);
        return *this;
    }

    BarePointer& operator=(T* rhs) {
        this_type(rhs).swap(*this);
        return *this;
    }

    void reset() {
        this_type().swap(*this);
    }

    void reset(T* rhs) {
        this_type(rhs).swap(*this);
    }

    T* get() const {
        return px;
    }

    T& operator*() const {
        BOOST_ASSERT(px != 0);
        return *px;
    }

    T* operator->() const {
        BOOST_ASSERT(px != 0);
        return px;
    }

    // implicit conversion to "bool"
//#include <boost/smart_ptr/detail/operator_bool.hpp>
    typedef T * this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const // never throws
    {
        return px == 0? 0: &this_type::px;
    }

    // operator! is redundant, but some compilers need it
    bool operator! () const // never throws
    {
        return px == 0;
    }

    void swap(BarePointer& rhs) {
        T* tmp = px;
        px = rhs.px;
        rhs.px = tmp;
    }

private:
    T* px;
};

template<class T, class U> inline bool operator==(BarePointer<T> const& a, BarePointer<U> const& b) {
    return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(BarePointer<T> const& a, BarePointer<U> const& b) {
    return a.get() != b.get();
}

template<class T, class U> inline bool operator==(BarePointer<T> const& a, U* b) {
    return a.get() == b;
}

template<class T, class U> inline bool operator!=(BarePointer<T> const& a, U* b) {
    return a.get() != b;
}

template<class T, class U> inline bool operator==(T* a, BarePointer<U> const& b) {
    return a == b.get();
}

template<class T, class U> inline bool operator!=(T* a, BarePointer<U> const& b) {
    return a != b.get();
}

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96

// Resolve the ambiguity between our op!= and the one in rel_ops

template<class T> inline bool operator!=(BarePointer<T> const& a, BarePointer<T> const& b) {
    return a.get() != b.get();
}

#endif

template<class T> inline bool operator<(BarePointer<T> const& a, BarePointer<T> const& b) {
    return std::less<T*>()(a.get(), b.get());
}

template<class T> void swap(BarePointer<T>& lhs, BarePointer<T>& rhs) {
    lhs.swap(rhs);
}

// mem_fn support

template<class T> T* get_pointer(BarePointer<T> const& p) {
    return p.get();
}

template<class T, class U> BarePointer<T> static_pointer_cast(BarePointer<U> const& p) {
    return static_cast<T*>(p.get());
}

template<class T, class U> BarePointer<T> const_pointer_cast(BarePointer<U> const& p) {
    return const_cast<T*>(p.get());
}

template<class T, class U> BarePointer<T> dynamic_pointer_cast(BarePointer<U> const& p) {
    return dynamic_cast<T*>(p.get());
}

// hash_value

template< class T > struct hash;

template< class T > std::size_t hash_value(BarePointer<T> const& p) {
    return boost::hash< T* >()(p.get());
}

}
}

#endif //#if !defined(CETTY_UTIL_BAREPOINTER_H)

// Local Variables:
// mode: c++
// End:
