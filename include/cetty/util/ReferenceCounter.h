#if !defined(CETTY_UTIL_REFERENCECOUNTER_H)
#define CETTY_UTIL_REFERENCECOUNTER_H

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <boost/checked_delete.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/detail/atomic_count.hpp>

namespace cetty {
namespace util {

template<typename T, typename Count = boost::detail::atomic_count>
class ReferenceCounter {
public:
    ReferenceCounter() : refCount(0) {}
    virtual ~ReferenceCounter() {}

#if 0
    friend void intrusive_ptr_add_ref(ReferenceCounter<T, Count> const* s) {
        BOOST_ASSERT(s != 0);
        s->duplicate();
    }

    friend void intrusive_ptr_release(ReferenceCounter<T, Count> const* s) {
        BOOST_ASSERT(s != 0);
        s->release();
    }
#endif

    /**
     * Increments the reference count.
     */
    inline void duplicate() const {
        BOOST_ASSERT(refCount >= 0);
        ++refCount;
    }

    /**
     * Decrements the reference count and deletes the object if the count
     * reaches zero.
     */
    inline void release() const {
        BOOST_ASSERT(refCount > 0);

        if (--refCount == 0) {
            boost::checked_delete(static_cast<T const*>(this));
        }
    }

    boost::intrusive_ptr<T> shared_from_this() {
        return boost::intrusive_ptr<T>((T*)this);
    }

    boost::intrusive_ptr<T const> shared_from_this() const {
        return boost::intrusive_ptr<T const>((T const*)this);
    }

    int refcount() const {
        return refCount;
    }

protected:
    /**
     * does not support assignment
     */
    ReferenceCounter& operator=(ReferenceCounter const& rhs) {
        return *this;
    }

    boost::intrusive_ptr<T> selfptr() {
        return boost::intrusive_ptr<T>((T*)this, false);
    }

private:
    /**
     * only construct an intrusive_ptr from another intrusive_ptr. That is it.
     */
    ReferenceCounter(ReferenceCounter<T, Count> const&);

private:
    /**
     * should be modifiable even from const intrusive_ptr objects
     */
    mutable Count refCount;
};

}
}

template<typename T, typename U>
inline void intrusive_ptr_add_ref(cetty::util::ReferenceCounter<T, U> const* counter) {
    BOOST_ASSERT(counter != 0);
    counter->duplicate();
}

template<typename T, typename U>
inline void intrusive_ptr_release(cetty::util::ReferenceCounter<T, U> const* counter) {
    BOOST_ASSERT(counter != 0);
    counter->release();
}

#endif //#if !defined(CETTY_UTIL_REFERENCECOUNTER_H)

// Local Variables:
// mode: c++
// End:

