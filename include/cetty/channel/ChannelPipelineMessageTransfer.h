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
#include <cetty/channel/ChannelInboundBufferHandlerContext.h>
#include <cetty/channel/ChannelOutboundBufferHandlerContext.h>
#include <cetty/channel/ChannelInboundMessageHandlerContext.h>
#include <cetty/channel/ChannelOutboundMessageHandlerContext.h>

namespace cetty {
namespace channel {

using namespace cetty::buffer;

template<typename T, typename U>
class ChannelPipelineMessageTransfer {
public:
    void setContext(ChannelHandlerContext& ctx);

    bool unfoldAndAdd(const T& msg);
    void write(const T& msg, const ChannelFuturePtr& future);
};

template<typename T>
class ChannelPipelineMessageTransfer<T, ChannelInboundMessageHandlerContext<T> > {
public:
    ChannelPipelineMessageTransfer() : ctx(), nextCtx() {}

    void setContext(ChannelHandlerContext& ctx) {
        this->ctx = &ctx;
    }

    bool unfoldAndAdd(const T& msg) {
        BOOST_ASSERT(ctx && "before unfoldAndAdd, must set ctx");

        if (!nextCtx) {
            nextCtx
                = ctx->nextInboundMessageHandlerContext<ChannelInboundMessageHandlerContext<T> >();
        }

        if (nextCtx && !!msg) {
            nextCtx->addInboundMessage(msg);
            return true;
        }

        return false;
    }

    void write(const T& msg, const ChannelFuturePtr& future) {
        BOOST_ASSERT("InboundMessage Transfer does not support write");
    }

private:
    ChannelHandlerContext* ctx;
    ChannelInboundMessageHandlerContext<T>* nextCtx;
};

template<typename T>
class ChannelPipelineMessageTransfer<T, ChannelOutboundMessageHandlerContext<T> > {
public:
    ChannelPipelineMessageTransfer() : ctx(), nextCtx(), nextOutCtx() {}

    void setContext(ChannelHandlerContext& ctx) {
        this->ctx = &ctx;
    }

    bool unfoldAndAdd(const T& msg) {
        if (!nextCtx) {
            nextCtx
                = ctx->nextOutboundMessageHandlerContext<ChannelOutboundMessageHandlerContext<T> >();
        }

        if (nextCtx && !!msg) {
            nextCtx->addOutboundMessage(msg);
            return true;
        }

        return false;
    }

    void write(const T& msg,
               const ChannelFuturePtr& future) {
        if (!nextOutCtx) {
            nextOutCtx = ctx->getNextOutboundContext();
        }

        if (unfoldAndAdd(msg) && nextOutCtx) {
            nextOutCtx->flush(*nextOutCtx, future);
        }
    }

private:
    ChannelHandlerContext* ctx;
    ChannelOutboundMessageHandlerContext<T>* nextCtx;
    ChannelHandlerContext* nextOutCtx;
};

template<>
class ChannelPipelineMessageTransfer<ChannelBufferPtr, ChannelInboundBufferHandlerContext> {
public:
    ChannelPipelineMessageTransfer() : ctx(), nextCtx() {}

    void setContext(ChannelHandlerContext& ctx) {
        this->ctx = &ctx;
    }

    bool unfoldAndAdd(const ChannelBufferPtr& msg) {
        if (!nextCtx) {
            nextCtx = ctx->nextInboundBufferHandlerContext();
        }

        if (nextCtx && msg) {
            nextCtx->setInboundChannelBuffer(msg);
            return true;
        }

        return false;
    }

    void write(const ChannelBufferPtr& msg, const ChannelFuturePtr& future) {
        BOOST_ASSERT("InboundBuffer Transfer does not support write");
    }

private:
    ChannelHandlerContext* ctx;
    ChannelInboundBufferHandlerContext* nextCtx;
};

template<>
class ChannelPipelineMessageTransfer<ChannelBufferPtr, ChannelOutboundBufferHandlerContext> {
public:
    ChannelPipelineMessageTransfer() : ctx(), nextCtx(), nextOutCtx() {}

    void setContext(ChannelHandlerContext& ctx) {
        this->ctx = &ctx;
    }

    bool unfoldAndAdd(const ChannelBufferPtr& msg) {
        if (!nextCtx) {
            nextCtx = ctx->nextOutboundBufferHandlerContext();
        }

        if (nextCtx && msg) {
            nextCtx->setOutboundChannelBuffer(msg);
            return true;
        }

        return false;
    }

    void write(const ChannelBufferPtr& msg,
               const ChannelFuturePtr& future) {
        if (!nextOutCtx) {
            nextOutCtx = ctx->getNextOutboundContext();
        }

        if (unfoldAndAdd(msg) && nextOutCtx) {
            nextOutCtx->flush(*nextOutCtx, future);
        }
    }

private:
    ChannelHandlerContext* ctx;
    ChannelOutboundBufferHandlerContext* nextCtx;
    ChannelHandlerContext* nextOutCtx;
};

template<typename T>
class ChannelPipelineMessageTransfer<T, ChannelPipeline> {
public:
    ChannelPipelineMessageTransfer() : ctx() {}

    void setContext(ChannelHandlerContext& ctx) {
        this->ctx = &ctx;
    }

private:
    ChannelHandlerContext* ctx;
};

template<>
class ChannelPipelineMessageTransfer<ChannelBufferPtr, ChannelPipeline> {
public:
    ChannelPipelineMessageTransfer() : ctx() {}

    void setContext(ChannelHandlerContext& ctx) {
        this->ctx = &ctx;
    }

private:
    ChannelHandlerContext* ctx;
};

template<typename T, typename U>
class ChannelPipelineMessageTransfer<std::vector<T>, U> {
public:
    ChannelPipelineMessageTransfer() : ctx() {}

    void setContext(ChannelHandlerContext& ctx) {
        this->ctx = &ctx;
        transfer.setContext(ctx);
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
    ChannelPipelineMessageTransfer<T, U> transfer;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELPIPELINEMESSAGETRANSFER_H)

// Local Variables:
// mode: c++
// End:
