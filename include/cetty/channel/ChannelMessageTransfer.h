#if !defined(CETTY_CHANNEL_CHANNELMESSAGETRANSFER_H)
#define CETTY_CHANNEL_CHANNELMESSAGETRANSFER_H

/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
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

#include <vector>
#include <cetty/buffer/ChannelBufferPtr.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelMessageContainer.h>

namespace cetty {
namespace channel {

using namespace cetty::buffer;

enum TransferDirection {
    TRANSFER_INBOUND,
    TRANSFER_OUTBOUND
};

template<typename T, typename U, int TransferType>
class ChannelMessageTransfer {
public:
    U* nextContainer();

    void resetNextContainer();
    void resetNextContainer(U*);

    bool unfoldAndAdd(const T& msg);
    void write(const T& msg, const ChannelFuturePtr& future);
};

template<>
class ChannelMessageTransfer<VoidMessage, VoidMessageContainer, TRANSFER_INBOUND> {
public:
    ChannelMessageTransfer(ChannelHandlerContext& ctx) {}

    VoidMessageContainer* nextContainer() { return NULL; }

    void resetNextContainer() {}
    void resetNextContainer(VoidMessageContainer*) {}

    bool unfoldAndAdd(const VoidMessage& msg) { return false; }
    void write(const VoidMessage& msg, const ChannelFuturePtr& future) {}
};

template<>
class ChannelMessageTransfer<VoidMessage, VoidMessageContainer, TRANSFER_OUTBOUND> {
public:
    ChannelMessageTransfer(ChannelHandlerContext& ctx) {}

    VoidMessageContainer* nextContainer() { return NULL; }
    
    void resetNextContainer() {}
    void resetNextContainer(VoidMessageContainer*) {}

    bool unfoldAndAdd(const VoidMessage& msg) { return false; }
    void write(const VoidMessage& msg, const ChannelFuturePtr& future) {}
};

template<class T>
class ChannelMessageTransfer<T, ChannelMessageContainer<T, MESSAGE_BLOCK>, TRANSFER_INBOUND> {
public:
    typedef ChannelMessageContainer<T, MESSAGE_BLOCK> MessageContainer;

public:
    ChannelMessageTransfer(ChannelHandlerContext& ctx)
        : ctx_(ctx),
          container_() {
    }

    ChannelMessageTransfer(ChannelPipeline& pipeline) {}

    void resetNextContainer() {
        container_ = NULL;
    }

    void resetNextContainer(MessageContainer* container) {
        container_ = container;
    }

    MessageContainer* nextContainer() {
        return container_;
    }

    bool unfoldAndAdd(const T& msg) {
        if (!container_) {
            container_ = ctx_.nextInboundMessageContainer<MessageContainer>();
        }

        if (container_ && !!msg) {
            container_->addMessage(msg);
            return true;
        }

        return false;
    }

    void write(const T& msg, const ChannelFuturePtr& future) {
        BOOST_ASSERT("InboundMessage Transfer does not support write");
    }

private:
    ChannelHandlerContext& ctx_;
    MessageContainer* container_;
};

template<typename T>
class ChannelMessageTransfer<T, ChannelMessageContainer<T, MESSAGE_BLOCK>, TRANSFER_OUTBOUND> {
public:
    typedef ChannelMessageContainer<T, MESSAGE_BLOCK> MessageContainer;

public:
    ChannelMessageTransfer(ChannelHandlerContext& ctx)
        : ctx_(ctx),
          container_() {
    }

    MessageContainer* nextContainer() {
        return container_;
    }

    void resetNextContainer() {
        container_ = NULL;
    }

    void resetNextContainer(MessageContainer* container) {
        container_ = container;
    }

    bool unfoldAndAdd(const T& msg) {
        if (!container_) {
            container_ = ctx_.nextOutboundMessageContainer<MessageContainer>();
        }

        if (container_&& !!msg) {
            container_->addMessage(msg);
            return true;
        }

        return false;
    }

    void write(const T& msg, const ChannelFuturePtr& future) {
        if (unfoldAndAdd(msg)) {
            ctx_.flush(future);
        }
    }

private:
    ChannelHandlerContext& ctx_;
    MessageContainer* container_;
};

template<>
class ChannelMessageTransfer<ChannelBufferPtr, ChannelBufferContainer, TRANSFER_INBOUND> {
public:
    ChannelMessageTransfer(ChannelHandlerContext& ctx)
        : ctx_(ctx),
          container_() {
    }

    ChannelBufferContainer* nextContainer() {
        return container_;
    }

    void resetNextContainer() {
        container_ = NULL;
    }

    void resetNextContainer(ChannelBufferContainer* container) {
        container_ = container;
    }

    bool unfoldAndAdd(const ChannelBufferPtr& msg) {
        if (!container_) {
            ctx_.nextInboundMessageContainer<ChannelBufferContainer>();
        }

        if (container_ && msg) {
            container_->addMessage(msg);
            return true;
        }

        return false;
    }

    void write(const ChannelBufferPtr& msg, const ChannelFuturePtr& future) {
        BOOST_ASSERT("InboundBuffer Transfer does not support write");
    }

private:
    ChannelHandlerContext& ctx_;
    ChannelBufferContainer* container_;
};

template<>
class ChannelMessageTransfer<ChannelBufferPtr, ChannelBufferContainer, TRANSFER_OUTBOUND> {
public:
    ChannelMessageTransfer(ChannelHandlerContext& ctx)
        : ctx_(ctx),
          container_() {
    }

    ChannelBufferContainer* nextContainer() {
        return container_;
    }

    void resetNextContainer() {
        container_ = NULL;
    }

    void resetNextContainer(ChannelBufferContainer* container) {
        container_ = container;
    }

    bool unfoldAndAdd(const ChannelBufferPtr& msg) {
        if (!container_) {
            container_ = ctx_.nextOutboundMessageContainer<ChannelBufferContainer>();
        }

        if (container_ && msg) {
            container_->addMessage(msg);
            return true;
        }

        return false;
    }

    void write(const ChannelBufferPtr& msg,
               const ChannelFuturePtr& future) {
        if (unfoldAndAdd(msg)) {
            ctx_.flush(future);
        }
    }

private:
    ChannelHandlerContext& ctx_;
    ChannelBufferContainer* container_;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELMESSAGETRANSFER_H)

// Local Variables:
// mode: c++
// End:
