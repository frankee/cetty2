#if !defined(CETTY_CHANNEL_USEREVENT_H)
#define CETTY_CHANNEL_USEREVENT_H

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

#include <typeinfo>
#include <string>
#include <boost/shared_ptr.hpp>

#include <cetty/channel/UserEventHolder.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

using namespace cetty::util;
using namespace cetty::buffer;

class UserEvent {
public:
    static UserEvent EMPTY_EVENT;

private:
    static std::string  EMPTY_STRING;

public:
    UserEvent() : buffer(), holder() {}

    template<typename T>
    UserEvent(const T& value)
        : buffer(), holder(new UserEventHolderImpl<T>(value)) {}

    UserEvent(const std::string& str)
        : buffer(), holder(new UserEventHolderImpl<std::string>(str)) {}

    UserEvent(const char* str)
        : buffer(), holder(new UserEventHolderImpl<std::string>(str)) {}

    UserEvent(const UserEvent& msg)
        : buffer(msg.buffer), holder(msg.holder) {}

    UserEvent(const ChannelBufferPtr& buffer)
        : buffer(buffer), holder() {}

    ~UserEvent() {}

    UserEvent& operator=(const UserEvent& msg) {
        holder = msg.holder;
        buffer = msg.buffer;
        return *this;
    }

    bool operator==(const UserEvent& msg) const {
        return buffer == msg.buffer && holder == msg.holder;
    }

    bool empty() const { return (!holder && !buffer); }
    bool isChannelBuffer() const { return !!buffer; }
    bool isString() const { return holder && holder->isString(); }
    bool isVector() const { return holder && holder->isVector(); }
    bool isPointer() const { return holder && holder->isPointer(); }
    bool isIntrusivePointer() const {
        return buffer || (holder && holder->isIntrusivePointer());
    }

    template<typename T>
    T& value() const {
        if (holder) {
            UserEventHolderImpl<T>* impl = holderImpl<T>();

            if (impl) {
                return impl->value();
            }

            throw BadCastException(
                std::string("Can not convert form ") +
                holder->type().name() +
                std::string(" to ") +
                typeid(T).name());
        }

        throw InvalidAccessException("Can not convert empty value.");
    }

    template<typename T>
    T* valuePtr() const {
        UserEventHolderImpl<T>* impl = holderImpl<T>();

        if (impl) {
            return &(impl->value());
        }

        return NULL;
    }

    template<typename T, typename U>
    T* pointer() const {
        if (buffer) { return NULL; } // not raw pointer type

        UserEventHolderImpl<U*>* impl = holderImpl<U*>();

        if (impl) {
            return dynamic_cast<T*>(impl->value());
        }

        return NULL;
    }

    template<typename T>
    T* pointer() const {
        if (buffer) { return NULL; } // not raw pointer type

        UserEventHolderImpl<T*>* impl = holderImpl<T*>();

        if (impl) {
            return impl->value();
        }

        return NULL;
    }

    template<typename T, typename U>
    boost::intrusive_ptr<T> intrusivePointer() const {
        if (buffer) {
            return boost::dynamic_pointer_cast<T>(buffer);
        }

        UserEventHolderImpl<boost::intrusive_ptr<U> >* impl =
            holderImpl<boost::intrusive_ptr<U> >();

        if (impl) {
            return boost::dynamic_pointer_cast<T>(impl->value());
        }

        return boost::intrusive_ptr<T>();
    }

    template<typename T>
    boost::intrusive_ptr<T> intrusivePointer() const {
        UserEventHolderImpl<boost::intrusive_ptr<T> >* impl =
            holderImpl<boost::intrusive_ptr<T> >();

        if (impl) {
            return (impl->value());
        }

        return boost::intrusive_ptr<T>();
    }

    // vector operators.
    int vectorSize() const {
        if (holder) {
            return holder->vectorSize();
        }

        return 0;
    }

    template<typename T>
    T& value(int index) const {
        if (!holder) {
            throw InvalidAccessException("Can not convert empty value.");
        }

        if (index < 0 || index >= holder->vectorSize()) {
            throw RangeException("Out of range.");
        }

        UserEventHolderImpl<std::vector<T> >* impl
            = holderImpl<std::vector<T> >();

        if (impl) {
            return impl->value(index);
        }

        throw BadCastException(std::string("Can not convert form ") +
                               holder->type().name() +
                               std::string(" to ") +
                               typeid(T).name());
    }

    template<typename T>
    T* valuePtr(int index) const {
        if (holder.get() && index >= 0 && index < holder->vectorSize()) {
            UserEventHolderImpl<std::vector<T> >* impl
                = holderImpl<std::vector<T> >();

            if (impl) {
                return &(impl->value(index));
            }
        }

        return NULL;
    }

    template<typename T>
    T* pointer(int index) const {
        if (holder.get() && index >= 0 && index < holder->vectorSize()) {
            UserEventHolderImpl<std::vector<T*> >* impl =
                holderImpl<std::vector<T*> >();

            if (impl) {
                return impl->value(index);
            }
        }

        return NULL;
    }

    template<typename T, typename U>
    T* pointer(int index) const {
        if (holder.get() && index >= 0 && index < holder->vectorSize()) {
            UserEventHolderImpl<std::vector<U*> >* impl =
                holderImpl<std::vector<U*> >();

            if (impl) {
                return dynamic_cast<T*>(impl->value(index));
            }
        }

        return NULL;
    }

    template<typename T>
    boost::intrusive_ptr<T> intrusivePointer(int index) const {
        if (holder.get() && index >= 0 && index < holder->vectorSize()) {
            UserEventHolderImpl<std::vector<boost::intrusive_ptr<T> > >* impl =
                holderImpl<std::vector<boost::intrusive_ptr<T> > >();

            if (impl) {
                return impl->value(index);
            }
        }

        return boost::intrusive_ptr<T>();
    }

    template<typename T, typename U>
    boost::intrusive_ptr<T> intrusivePointer(int index) const {
        if (holder.get() && index >= 0 && index < holder->vectorSize()) {
            UserEventHolderImpl<std::vector<boost::intrusive_ptr<U> > >* impl =
                holderImpl<std::vector<boost::intrusive_ptr<U> > >();

            if (impl) {
                return boost::dynamic_pointer_cast<T>(impl->value(index));
            }
        }

        return boost::intrusive_ptr<T>();
    }

    std::string toString() const {
        if (!holder) {
            return "empty ChannelMessage";
        }

        return holder->type().name();
    }

    // it is incompatible with c++ stardard, but only for remove the
    // link errors under visual studio.
#if defined(BOOST_MSVC)
    template<> std::string& value<std::string>() const;
    template<> ChannelBufferPtr& value<ChannelBufferPtr>() const;
    template<> ChannelBufferPtr* valuePtr<ChannelBufferPtr>() const;
    template<> ChannelBufferPtr  intrusivePointer<ChannelBuffer>() const;
#endif

private:
    template <typename T>
    UserEventHolderImpl<T>* holderImpl() const {
        if (holder.get() && holder->type() == typeid(T)) {
            return static_cast<UserEventHolderImpl<T>*>(holder.get());
        }

        return NULL;
    }

    // it is incompatible with c++ stardard, but only for remove the
    // link errors under visual studio.
#if defined(BOOST_MSVC)
    template<> UserEventHolderImpl<std::string>* holderImpl<std::string>() const;
#endif

private:
    ChannelBufferPtr   buffer;
    UserEventHolderPtr holder;
};

template<> inline
UserEventHolderImpl<std::string>* UserEvent::holderImpl<std::string>() const {
    if (holder && holder->isString()) {
        return static_cast<UserEventHolderImpl<std::string>*>(holder.get());
    }

    return NULL;
}

template<> inline
std::string& UserEvent::value<std::string>() const {
    if (holder) {
        UserEventHolderImpl<std::string>* impl = holderImpl<std::string>();

        if (impl) {
            return impl->value();
        }

        return EMPTY_STRING;
    }

    return EMPTY_STRING;
}

template<> inline
ChannelBufferPtr& UserEvent::value<ChannelBufferPtr>() const {
    return const_cast<ChannelBufferPtr&>(buffer);
}

template<> inline
ChannelBufferPtr* UserEvent::valuePtr<ChannelBufferPtr>() const {
    return const_cast<ChannelBufferPtr*>(&buffer);
}

template<> inline
ChannelBufferPtr UserEvent::intrusivePointer<ChannelBuffer>() const {
    return buffer;
}

}
}

#endif //#if !defined(CETTY_CHANNEL_USEREVENT_H)

// Local Variables:
// mode: c++
// End:

