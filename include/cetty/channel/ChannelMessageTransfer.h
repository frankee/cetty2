#if !defined(CETTY_CHANNEL_CHANNELPIPELINEMESSAGETRANSFER_H)
#define CETTY_CHANNEL_CHANNELPIPELINEMESSAGETRANSFER_H

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

enum IsInboundType {
    TRANSFER_INBOUND,
    TRANSFER_OUTBOUND
};

template<typename T, typename U, int TransferType>
class ChannelMessageTransfer {
public:
    void resetNextContainer();

    bool unfoldAndAdd(const T& msg);
    void write(const T& msg, const ChannelFuturePtr& future);
};

template<class T>
class ChannelMessageTransfer<T, ChannelMessageContainer<T>, TRANSFER_INBOUND> {
public:
    typedef ChannelMessageContainer<T> MessageContainer;

public:
    ChannelMessageTransfer(ChannelHandlerContext& ctx)
        : ctx(ctx) {
    }

    void resetNextContainer() {
        container = NULL;
    }

    bool unfoldAndAdd(const T& msg) {
        if (!container) {
            container = ctx.nextInboundMessageContainer();
        }

        if (container && !!msg) {
            container->addMessage(msg);
            return true;
        }

        return false;
    }

    void write(const T& msg, const ChannelFuturePtr& future) {
        BOOST_ASSERT("InboundMessage Transfer does not support write");
    }

private:
    ChannelHandlerContext& ctx;
    MessageContainer* container;
};

template<typename T>
class ChannelMessageTransfer<T, ChannelMessageContainer<T>, TRANSFER_OUTBOUND> {
public:
    typedef ChannelMessageContainer<T> MessageContainer;

public:
    ChannelMessageTransfer(ChannelHandlerContext& ctx) : ctx(ctx) {}

    void resetNextContainer() {
        container = NULL;
    }

    bool unfoldAndAdd(const T& msg) {
        if (!container) {
            container = ctx.nextOutboundMessageContainer<T>();
        }

        if (container&& !!msg) {
            container->addMessage(msg);
            return true;
        }

        return false;
    }

    void write(const T& msg, const ChannelFuturePtr& future) {
        if (unfoldAndAdd(msg)) {
            ctx.flush(future);
        }
    }

private:
    ChannelHandlerContext& ctx;
    MessageContainer* container;
};

template<>
class ChannelMessageTransfer<ChannelBufferPtr, ChannelBufferContainer, TRANSFER_INBOUND> {
public:
    ChannelMessageTransfer(ChannelHandlerContext& ctx) : ctx(ctx) {}

    void resetNextContainer() {
        container = NULL;
    }

    bool unfoldAndAdd(const ChannelBufferPtr& msg) {
        if (!container) {
            ctx.nextInboundBufferContainer();
        }

        if (container && msg) {
            container->addMessage(msg);
            return true;
        }

        return false;
    }

    void write(const ChannelBufferPtr& msg, const ChannelFuturePtr& future) {
        BOOST_ASSERT("InboundBuffer Transfer does not support write");
    }

private:
    ChannelHandlerContext& ctx;
    ChannelBufferContainer* container;
};

template<>
class ChannelMessageTransfer<ChannelBufferPtr, ChannelBufferContainer, TRANSFER_OUTBOUND> {
public:
    ChannelMessageTransfer(ChannelHandlerContext& ctx) : ctx(ctx) {}

    void resetNextContainer() {
        container = NULL;
    }

    bool unfoldAndAdd(const ChannelBufferPtr& msg) {
        if (!container) {
            container = ctx.nextOutboundBufferContainer();
        }

        if (container && msg) {
            container->addMessage(msg);
            return true;
        }

        return false;
    }

    void write(const ChannelBufferPtr& msg,
               const ChannelFuturePtr& future) {
        if (unfoldAndAdd(msg)) {
            ctx.flush(future);
        }
    }

private:
    ChannelHandlerContext& ctx;
    ChannelBufferContainer* container;
};

template<typename T, typename U, int TransferType>
class ChannelMessageTransfer<std::vector<T>, U, TransferType> {
public:
    ChannelMessageTransfer() : ctx() {}

    void resetNextContainer() {
        container = NULL;
    }

    bool unfoldAndAdd(const std::vector<T>& msg) {
        if (msg.empty()) {
            return false;
        }

        bool added = false;

        std::size_t j = msg.size();

        for (std::size_t i = 0; i < j; ++i) {
            if (transfer.unfoldAndAdd(msg[i])) {
                added = true;
            }
        }

        return added;
    }

private:
    ChannelHandlerContext* ctx;
    ChannelMessageTransfer<T, U> transfer;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELPIPELINEMESSAGETRANSFER_H)

// Local Variables:
// mode: c++
// End:
