#if !defined(CETTY_CHANNEL_CHANNELMESSAGE_H)
#define CETTY_CHANNEL_CHANNELMESSAGE_H

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
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/config.hpp>

#include <cetty/channel/ChannelMessageHolder.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

using namespace cetty::util;
using namespace cetty::buffer;

class ChannelMessage {
public:
    static ChannelMessage EMPTY_MESSAGE;
    static std::string    EMPTY_STRING;
    static std::wstring   EMPTY_WSTRING;
    static std::vector<ChannelBufferPtr> EMPTY_BUFFERS;
    static std::vector<ChannelMessage>   EMPTY_MESSAGES;

public:
    ChannelMessage() : buffer(), holder() {}

    template<typename T>
    ChannelMessage(const T& value)
        : buffer(), holder(new ChannelMessageHolderImpl<T>(value)) {}

    template<typename T>
    ChannelMessage(T* value)
        :buffer(),holder(new ChannelMessageHolderImpl<T*>(value)) {}

    ChannelMessage(const std::string& str)
        : buffer(), holder(new ChannelMessageHolderImpl<std::string>(str)) {}

    ChannelMessage(const char* str)
        : buffer(), holder(new ChannelMessageHolderImpl<std::string>(str)) {}

    ChannelMessage(const std::wstring& str)
        : buffer(), holder(new ChannelMessageHolderImpl<std::wstring>(str)) {}

    ChannelMessage(const wchar_t* str)
        : buffer(), holder(new ChannelMessageHolderImpl<std::wstring>(str)) {}

    ChannelMessage(const ChannelMessage& msg)
        : buffer(msg.buffer), holder(msg.holder) {}

    ChannelMessage(const ChannelBufferPtr& buffer)
        : buffer(buffer), holder() {}

    ChannelMessage(const std::vector<ChannelBufferPtr>& buffers)
        : buffer(ChannelBuffers::wrappedBuffer(buffers)), holder() {
    }

    ChannelMessage(const ChannelBufferPtr& buffer0,
                   const ChannelBufferPtr& buffer1)
        : buffer(ChannelBuffers::wrappedBuffer(buffer0, buffer1)), holder() {
    }

    ChannelMessage(const ChannelBufferPtr& buffer0,
                   const ChannelBufferPtr& buffer1,
                   const ChannelBufferPtr& buffer2)
        : buffer(ChannelBuffers::wrappedBuffer(buffer0, buffer1, buffer2)),
          holder() {
    }

    ChannelMessage(const ChannelMessage& msg0,
                   const ChannelMessage& msg1) {
        std::vector<ChannelMessage> msgs(1, msg0);
        msgs.push_back(msg1);
        holder = ChannelMessageHolderPtr(
                     new ChannelMessageHolderImpl<std::vector<ChannelMessage> >(msgs));
    }

    ChannelMessage(const ChannelMessage& msg0,
                   const ChannelMessage& msg1,
                   const ChannelMessage& msg2) {
        std::vector<ChannelMessage> msgs(1, msg0);
        msgs.push_back(msg1);
        msgs.push_back(msg2);
        holder = ChannelMessageHolderPtr(
                     new ChannelMessageHolderImpl<std::vector<ChannelMessage> >(msgs));
    }

    ~ChannelMessage() {}

    ChannelMessage& operator=(const ChannelMessage& msg) {
        holder = msg.holder;
        buffer = msg.buffer;
        return *this;
    }

    bool operator==(const ChannelMessage& msg) const {
        return buffer == msg.buffer && holder == msg.holder;
    }

    bool empty() const { return (!holder && !buffer); }
    bool isChannelBuffer() const { return buffer; }
    bool isString() const { return holder && holder->isString(); }
    bool isWideString() const { return holder && holder->isWideString(); }
    bool isVector() const { return holder && holder->isVector(); }
    bool isRawPointer() const { return holder && holder->isRawPointer(); }
    bool isSmartPointer() const { return buffer || (holder && holder->isSmartPointer()); }

    template<typename T>
    T& value() const {
        if (holder) {
            ChannelMessageHolderImpl<T>* impl = holderImpl<T>();

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
        ChannelMessageHolderImpl<T>* impl = holderImpl<T>();

        if (impl) {
            return &(impl->value());
        }

        return NULL;
    }

    template<typename T, typename U>
    T* rawPointer() const {
        if (buffer) { return NULL; } // not raw pointer type

        ChannelMessageHolderImpl<U*>* impl = holderImpl<U*>();

        if (impl) {
            return dynamic_cast<T*>(impl->value());
        }

        return NULL;
    }

    template<typename T>
    T* rawPointer() const {
        if (buffer) { return NULL; } // not raw pointer type

        ChannelMessageHolderImpl<T*>* impl = holderImpl<T*>();

        if (impl) {
            return impl->value();
        }

        return NULL;
    }

    template<typename T, typename U>
    boost::intrusive_ptr<T> smartPointer() const {
        if (buffer) { return boost::dynamic_pointer_cast<T>(buffer); }

        ChannelMessageHolderImpl<boost::intrusive_ptr<U> >* impl =
            holderImpl<boost::intrusive_ptr<U> >();

        if (impl) {
            return boost::dynamic_pointer_cast<T>(impl->value());
        }

        return boost::intrusive_ptr<T>();
    }

    template<typename T>
    boost::intrusive_ptr<T> smartPointer() const {
        ChannelMessageHolderImpl<boost::intrusive_ptr<T> >* impl =
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

        ChannelMessageHolderImpl<std::vector<T> >* impl
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
            ChannelMessageHolderImpl<std::vector<T> >* impl
                = holderImpl<std::vector<T> >();

            if (impl) {
                return &(impl->value(index));
            }
        }

        return NULL;
    }

    template<typename T>
    T* rawPointer(int index) const {
        if (holder.get() && index >= 0 && index < holder->vectorSize()) {
            ChannelMessageHolderImpl<std::vector<T*> >* impl =
                holderImpl<std::vector<T*> >();

            if (impl) {
                return impl->value(index);
            }
        }

        return NULL;
    }

    template<typename T, typename U>
    T* rawPointer(int index) const {
        if (holder.get() && index >= 0 && index < holder->vectorSize()) {
            ChannelMessageHolderImpl<std::vector<U*> >* impl =
                holderImpl<std::vector<U*> >();

            if (impl) {
                return dynamic_cast<T*>(impl->value(index));
            }
        }

        return NULL;
    }

    template<typename T>
    boost::intrusive_ptr<T> smartPointer(int index) const {
        if (holder.get() && index >= 0 && index < holder->vectorSize()) {
            ChannelMessageHolderImpl<std::vector<boost::intrusive_ptr<T> > >* impl =
                holderImpl<std::vector<boost::intrusive_ptr<T> > >();

            if (impl) {
                return impl->value(index);
            }
        }

        return boost::intrusive_ptr<T>();
    }

    template<typename T, typename U>
    boost::intrusive_ptr<T> smartPointer(int index) const {
        if (holder.get() && index >= 0 && index < holder->vectorSize()) {
            ChannelMessageHolderImpl<std::vector<boost::intrusive_ptr<U> > >* impl =
                holderImpl<std::vector<boost::intrusive_ptr<U> > >();

            if (impl) {
                return boost::dynamic_pointer_cast<T>(impl->value(index));
            }
        }

        return boost::intrusive_ptr<T>();
    }

    std::vector<ChannelBufferPtr>& channelBuffers() const {
        if (holder.get() && holder->isVector()) {
            ChannelMessageHolderImpl<std::vector<ChannelBufferPtr> >* impl =
                holderImpl<std::vector<ChannelBufferPtr> >();

            if (impl) {
                return impl->value();
            }
        }

        return EMPTY_BUFFERS;
    }

    std::vector<ChannelMessage>& channelMessages() const {
        if (holder.get() && holder->isVector()) {
            ChannelMessageHolderImpl<std::vector<ChannelMessage> >* impl =
                holderImpl<std::vector<ChannelMessage> >();

            if (impl) {
                return impl->value();
            }
        }

        return EMPTY_MESSAGES;
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
    template<> std::wstring& value<std::wstring>() const;
    template<> ChannelBufferPtr& value<ChannelBufferPtr>() const;
    template<> ChannelBufferPtr* valuePtr<ChannelBufferPtr>() const;
    template<> ChannelBufferPtr  smartPointer<ChannelBuffer>() const;
    template<> ChannelMessage& value<ChannelMessage>(int index) const;
#endif

private:
    template <typename T>
    ChannelMessageHolderImpl<T>* holderImpl() const {
        if (holder.get() && holder->type() == typeid(T)) {
            return static_cast<ChannelMessageHolderImpl<T>*>(holder.get());
        }

        return NULL;
    }

    // it is incompatible with c++ stardard, but only for remove the
    // link errors under visual studio.
#if defined(BOOST_MSVC)
    template<> ChannelMessageHolderImpl<std::string>* holderImpl<std::string>() const;
    template<> ChannelMessageHolderImpl<std::wstring>* holderImpl<std::wstring>() const;
#endif

private:
    ChannelBufferPtr        buffer;
    ChannelMessageHolderPtr holder;
};

template<> inline
ChannelMessageHolderImpl<std::string>* ChannelMessage::holderImpl<std::string>() const {
    if (holder && holder->isString()) {
        return static_cast<ChannelMessageHolderImpl<std::string>*>(holder.get());
    }

    return NULL;
}

template<> inline
ChannelMessageHolderImpl<std::wstring>* ChannelMessage::holderImpl<std::wstring>() const {
    if (holder && holder->isWideString()) {
        return static_cast<ChannelMessageHolderImpl<std::wstring>*>(holder.get());
    }

    return NULL;
}

template<> inline
std::string& ChannelMessage::value<std::string>() const {
    if (holder) {
        ChannelMessageHolderImpl<std::string>* impl = holderImpl<std::string>();

        if (impl) {
            return impl->value();
        }

        return EMPTY_STRING;
    }

    return EMPTY_STRING;
}

template<> inline
std::wstring& ChannelMessage::value<std::wstring>() const {
    if (holder) {
        ChannelMessageHolderImpl<std::wstring>* impl = holderImpl<std::wstring>();

        if (impl) {
            return impl->value();
        }

        return EMPTY_WSTRING;
    }

    return EMPTY_WSTRING;
}

template<> inline
ChannelBufferPtr& ChannelMessage::value<ChannelBufferPtr>() const {
    if (!buffer && holder && holder->isChannelBuffer()) {
        ChannelMessageHolderImpl<ChannelBufferPtr>* impl = holderImpl<ChannelBufferPtr>();

        if (impl) {
            return impl->value();
        }
    }

    return const_cast<ChannelBufferPtr&>(buffer);
}

template<> inline
ChannelBufferPtr* ChannelMessage::valuePtr<ChannelBufferPtr>() const {
    if (!buffer && holder && holder->isChannelBuffer()) {
        ChannelMessageHolderImpl<ChannelBufferPtr>* impl = holderImpl<ChannelBufferPtr>();

        if (impl) {
            return &(impl->value());
        }
    }

    return const_cast<ChannelBufferPtr*>(&buffer);
}

template<> inline
ChannelBufferPtr ChannelMessage::smartPointer<ChannelBuffer>() const {
    if (!buffer && holder && holder->isChannelBuffer()) {
        ChannelMessageHolderImpl<ChannelBufferPtr>* impl = holderImpl<ChannelBufferPtr>();

        if (impl) {
            return impl->value();
        }
    }

    return buffer;
}

template<> inline
ChannelMessage& ChannelMessage::value<ChannelMessage>(int index) const {
    if (holder.get() && index >= 0 && index < holder->vectorSize()) {
        ChannelMessageHolderImpl<std::vector<ChannelMessage> >* impl =
            holderImpl<std::vector<ChannelMessage> >();

        if (impl) {
            return impl->value(index);
        }
    }

    return ChannelMessage::EMPTY_MESSAGE;
}

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELMESSAGE_H)

// Local Variables:
// mode: c++
// End:
