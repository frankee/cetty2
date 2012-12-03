#if !defined(CETTY_CHANNEL_CHANNELMESSAGECONTAINER_H)
#define CETTY_CHANNEL_CHANNELMESSAGECONTAINER_H

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

#include <deque>
#include <boost/assert.hpp>
#include <cetty/channel/VoidMessage.h>

#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/ChannelBuffer.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {

using namespace cetty::buffer;

enum ChannelMessageType {
    MESSAGE_STREAM,
    MESSAGE_BLOCK
};

template<class T, int MessageType>
class ChannelMessageContainer {
public:
    bool empty() const;

    void addMessage(const T& message);
    void setEventLoop(const EventLoopPtr& eventLoop);
};

template<>
class ChannelMessageContainer<VoidMessage, MESSAGE_STREAM> {
public:
    typedef ChannelMessageContainer<VoidMessage, MESSAGE_STREAM> Container;

public:
    bool empty() const {
        return true;
    }

    void addMessage(const VoidMessage& message) {
        LOG_WARN << "void type, should not addMessage";
    }

    void setEventLoop(const EventLoopPtr& eventLoop) {
        LOG_WARN << "void type, should not setEventLoop";
    }
};

template<>
class ChannelMessageContainer<VoidMessage, MESSAGE_BLOCK> {
public:
    typedef std::deque<VoidMessage> MessageQueue;
    typedef ChannelMessageContainer<VoidMessage, MESSAGE_BLOCK> Container;

public:
    bool empty() const {
        return true;
    }

    void addMessage(const VoidMessage& message) {
        LOG_WARN << "void type, should not addMessage";
    }

    void setEventLoop(const EventLoopPtr& eventLoop) {
        LOG_WARN << "void type, should not setEventLoop";
    }
};

template<>
class ChannelMessageContainer<ChannelBufferPtr, MESSAGE_STREAM> {
public:
    typedef ChannelMessageContainer<ChannelBufferPtr, MESSAGE_STREAM> Container;

    static const int DEFAULT_BUFFER_SIZE = 16 * 1024;

public:
    ChannelMessageContainer()
        : accumulated_(false) {
    }

    ChannelMessageContainer(bool accumulatedBuffer)
        : accumulated_(accumulatedBuffer) {
        if (accumulated_) {
            buffer_ = Unpooled::buffer(DEFAULT_BUFFER_SIZE);
        }
    }

    void setEventLoop(const EventLoopPtr& eventLoop) {
        eventLoop_ = eventLoop;
    }

    bool accumulated() const {
        return accumulated_;
    }

    void setAccumulated() {
        if (accumulated_) {
            LOG_INFO << "This ChannelBufferContainer is already accumulated.";
            return;
        }

        if (eventLoop_->inLoopThread()) {
            ChannelBufferPtr buffer = Unpooled::buffer(DEFAULT_BUFFER_SIZE);

            if (!empty()) {
                buffer->writeBytes(buffer_);
                buffer_ = buffer;

                LOG_INFO << "setting the ChannelBufferContaner to accumulated, "
                         "but the container is not empty, then transfer the data first.";
            }
            else {
                buffer_ = buffer;
            }
        }
        else {
            eventLoop_->post(boost::bind(&Container::setAccumulated, this));
        }
    }

    void addMessage(const ChannelBufferPtr& message) {
        if (eventLoop_->inLoopThread()) {
            if (accumulated_) {
                buffer_->writeBytes(message);
            }
            else {
                buffer_ = message;
            }
        }
        else {
            eventLoop_->post(boost::bind(&Container::addMessage,
                                         this,
                                         message->copy()));
        }
    }

    ChannelBufferPtr& getMessages() {
        return buffer_;
    }

    const ChannelBufferPtr& getMessages() const {
        return buffer_;
    }

    bool empty() const {
        return !(buffer_ && buffer_->readable());
    }

private:
    bool accumulated_;
    ChannelBufferPtr buffer_;
    EventLoopPtr eventLoop_;
};

template<class T>
class ChannelMessageContainer<T, MESSAGE_BLOCK> {
public:
    typedef std::deque<T> MessageQueue;
    typedef ChannelMessageContainer<T, MESSAGE_BLOCK> Container;

public:
    ChannelMessageContainer() {}

    void setEventLoop(const EventLoopPtr& eventLoop) {
        eventLoop_ = eventLoop;
    }

    void addMessage(const T& message) {
        if (eventLoop_->inLoopThread()) {
            messageQueue_.push_back(message);
        }
        else {
            eventLoop_->post(boost::bind(
                                 &Container::addMessage,
                                 this,
                                 message));
        }
    }

    MessageQueue& getMessages() {
        return messageQueue_;
    }

    const MessageQueue& getMessages() const {
        return messageQueue_;
    }

    bool empty() const {
        return messageQueue_.empty();
    }

private:
    EventLoopPtr eventLoop_;
    MessageQueue messageQueue_;
};

template<> inline
void ChannelMessageContainer<ChannelBufferPtr, MESSAGE_BLOCK>::addMessage(const ChannelBufferPtr& message) {
    if (eventLoop_->inLoopThread()) {
        messageQueue_.push_back(message);
    }
    else {
        eventLoop_->post(boost::bind(&ChannelMessageContainer<ChannelBufferPtr, MESSAGE_BLOCK>::addMessage,
                                     this,
                                     message->copy()));
    }
}

typedef ChannelMessageContainer<VoidMessage, MESSAGE_STREAM> VoidBufferContainer;
typedef ChannelMessageContainer<VoidMessage, MESSAGE_BLOCK>  VoidMessageContainer;
typedef ChannelMessageContainer<ChannelBufferPtr, MESSAGE_STREAM>   ChannelBufferContainer;

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELMESSAGECONTAINER_H)

// Local Variables:
// mode: c++
// End:
