#if !defined(CETTY_CHANNEL_COMBINEDCHANNELBUFFERMESSAGEHANDLER_H)
#define CETTY_CHANNEL_COMBINEDCHANNELBUFFERMESSAGEHANDLER_H

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

#include <cetty/channel/ChannelInboundBufferHandler.h>
#include <cetty/channel/ChannelOutboundMessageHandler.h>
#include <cetty/channel/ChannelPipelineMessageTransfer.h>
#include <cetty/channel/CombinedChannelBufferMessageHandlerContext.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

template<typename InboundOutT, typename OutboundInT = InboundOutT>
class CombinedChannelBufferMessageHandler
    : public ChannelInboundBufferHandler,
      public ChannelOutboundMessageHandler<OutboundInT> {
public:
    typedef ChannelInboundBufferHandler InboundHandler;
    typedef ChannelOutboundMessageHandler<OutboundInT> OutboundHandler;

    typedef boost::intrusive_ptr<InboundHandler> InboundHandlerPtr;
    typedef boost::intrusive_ptr<OutboundHandler> OutboundHandlerPtr;

    typedef ChannelInboundMessageHandlerContext<InboundOutT> NextInboundContext;
    typedef ChannelOutboundBufferHandlerContext NextOutboundContext;

public:
    CombinedChannelBufferMessageHandler(const InboundHandlerPtr& inboundHandler,
                                        const OutboundHandlerPtr& outboundHandler) {
        init(inboundHandler, outboundHandler);
    }

    virtual ~CombinedChannelBufferMessageHandler() {}

    virtual void beforeAdd(ChannelHandlerContext& ctx) {
        if (!in) {
            throw IllegalStateException(
                "not initialized yet - call init() in the constructor of the subclass");
        }

        in->beforeAdd(ctx);
        out->beforeAdd(ctx);
    }

    virtual void afterAdd(ChannelHandlerContext& ctx) {
        in->afterAdd(ctx);
        out->afterAdd(ctx);

        inboundTransfer.setContext(ctx);
        outboundTransfer.setContext(ctx);
    }

    virtual void beforeRemove(ChannelHandlerContext& ctx) {
        in->beforeRemove(ctx);
        out->beforeRemove(ctx);
    }

    virtual void afterRemove(ChannelHandlerContext& ctx) {
        in->afterRemove(ctx);
        out->afterRemove(ctx);
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx,
                                 const ChannelException& cause) {
        in->exceptionCaught(ctx, cause);
    }

    virtual void userEventTriggered(ChannelHandlerContext& ctx,
                                    const boost::any& evt) {
        in->userEventTriggered(ctx, evt);
    }

    virtual void channelCreated(ChannelHandlerContext& ctx) {
        in->channelCreated(ctx);
    }

    virtual void channelActive(ChannelHandlerContext& ctx) {
        in->channelActive(ctx);
    }

    virtual void channelInactive(ChannelHandlerContext& ctx) {
        in->channelInactive(ctx);
    }

    virtual void writeCompleted(ChannelHandlerContext& ctx) {
        in->writeCompleted(ctx);
    }

    virtual void messageUpdated(ChannelHandlerContext& ctx) {
        in->messageUpdated(ctx);
    }

    virtual void bind(ChannelHandlerContext& ctx,
                      const SocketAddress& localAddress,
                      const ChannelFuturePtr& future) {
        out->bind(ctx, localAddress, future);
    }

    virtual void connect(ChannelHandlerContext& ctx,
                         const SocketAddress& remoteAddress,
                         const SocketAddress& localAddress,
                         const ChannelFuturePtr& future) {
        out->connect(ctx, remoteAddress, localAddress, future);
    }

    virtual void disconnect(ChannelHandlerContext& ctx,
                            const ChannelFuturePtr& future) {
        out->disconnect(ctx, future);
    }

    virtual void close(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future) {
        out->close(ctx, future);
    }

    virtual void flush(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future) {
        out->flush(ctx, future);
    }

    virtual void setInboundChannelBuffer(const ChannelBufferPtr& buffer) {
        in->setInboundChannelBuffer(buffer);
    }

    virtual void addOutboundMessage(const OutboundInT& message) {
        out->addOutboundMessage(message);
    }

    virtual ChannelHandlerContext* createContext(const std::string& name,
            ChannelPipeline& pipeline,
            ChannelHandlerContext* prev,
            ChannelHandlerContext* next) {
        return new CombinedChannelBufferMessageHandlerContext<OutboundInT>(name,
                pipeline,
                ChannelHandler::shared_from_this(),
                prev,
                next);
    }

    virtual ChannelHandlerContext* createContext(const std::string& name,
            const EventLoopPtr& eventLoop,
            ChannelPipeline& pipeline,
            ChannelHandlerContext* prev,
            ChannelHandlerContext* next) {
        return new CombinedChannelBufferMessageHandlerContext<OutboundInT>(name,
                eventLoop,
                pipeline,
                ChannelHandler::shared_from_this(),
                prev,
                next);
    }

    virtual ChannelHandlerPtr clone() {
        return new CombinedChannelBufferMessageHandler(
            boost::dynamic_pointer_cast<InboundHandler>(in->clone()),
            boost::dynamic_pointer_cast<OutboundHandler>(out->clone()));
    }

    virtual std::string toString() const {
        return "CombinedChannelBufferMessageHandler";
    }

protected:
    CombinedChannelBufferMessageHandler() {
        // User will call init in the subclass constructor.
    }

    void init(const InboundHandlerPtr& inboundHandler,
              const OutboundHandlerPtr& outboundHandler) {
        if (!inboundHandler) {
            throw NullPointerException("inboundHandler");
        }

        if (!outboundHandler) {
            throw NullPointerException("outboundHandler");
        }

        if (in) {
            throw IllegalStateException("init() cannot be called more than once.");
        }

        in = inboundHandler;
        out = outboundHandler;
    }

protected:
    ChannelPipelineMessageTransfer<InboundOutT, NextInboundContext> inboundTransfer;
    ChannelPipelineMessageTransfer<ChannelBufferPtr, NextOutboundContext> outboundTransfer;

private:
    OutboundHandlerPtr out;
    InboundHandlerPtr in;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_COMBINEDCHANNELBUFFERMESSAGEHANDLER_H)

// Local Variables:
// mode: c++
// End:
