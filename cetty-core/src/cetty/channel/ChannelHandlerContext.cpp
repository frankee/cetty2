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

#include <cetty/channel/ChannelHandlerContext.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelInboundHandler.h>
#include <cetty/channel/ChannelOutboundHandler.h>
#include <cetty/channel/ChannelInboundBufferHandlerContext.h>
#include <cetty/channel/ChannelOutboundBufferHandlerContext.h>

namespace cetty {
namespace channel {

ChannelHandlerContext::ChannelHandlerContext(const std::string& name,
        ChannelPipeline& pipeline,
        const ChannelHandlerPtr& handler,
        ChannelHandlerContext* prev,
        ChannelHandlerContext* next)
    : pipeline(pipeline),
      eventloop(),
      next(next),
      prev(prev),
      nextInboundContext(NULL),
      nextOutboundContext(NULL),
      canHandleInbound(false),
      canHandleOutbound(false),
      hasInboundBufferHandler(false),
      hasInboundMessageHandler(false),
      hasOutboundBufferHandler(false),
      hasOutboundMessageHandler(false),
      name(name),
      handler(handler) {
          init(handler);
}

ChannelHandlerContext::ChannelHandlerContext(const std::string& name,
        const EventLoopPtr& eventLoop,
        ChannelPipeline& pipeline,
        const ChannelHandlerPtr& handler,
        ChannelHandlerContext* prev,
        ChannelHandlerContext* next)
    : pipeline(pipeline),
      eventloop(eventloop),
      next(next),
      prev(prev),
      nextInboundContext(NULL),
      nextOutboundContext(NULL),
      canHandleInbound(false),
      canHandleOutbound(false),
      hasInboundBufferHandler(false),
      hasInboundMessageHandler(false),
      hasOutboundBufferHandler(false),
      hasOutboundMessageHandler(false),
      name(name),
      handler(handler) {
    init(handler);
}

inline
const ChannelPtr& ChannelHandlerContext::getChannel() const {
    return pipeline.getChannel();
}

const ChannelHandlerPtr& ChannelHandlerContext::getHandler() const {
    return this->handler;
}

const ChannelInboundHandlerPtr& ChannelHandlerContext::getInboundHandler() const {
    return this->inboundHandler;
}

const ChannelOutboundHandlerPtr& ChannelHandlerContext::getOutboundHandler() const {
    return this->outboundHandler;
}

void ChannelHandlerContext::fireChannelCreated() {
    ChannelHandlerContext* next = nextInboundContext;

    if (next) {
        fireChannelCreated(*next);
    }
}

void ChannelHandlerContext::fireChannelCreated(ChannelHandlerContext& ctx) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.inboundHandler->channelCreated(ctx);
        }
        catch (const Exception& t) {
            pipeline.notifyHandlerException(t);
        }
    }
    else {
        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::fireChannelCreated,
                        this,
                        boost::ref(ctx)));
    }
}

void ChannelHandlerContext::fireChannelInactive() {
    ChannelHandlerContext* next = nextInboundContext;

    if (next) {
        fireChannelInactive(*next);
    }
}

void ChannelHandlerContext::fireChannelInactive(ChannelHandlerContext& ctx) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.inboundHandler->channelInactive(ctx);
        }
        catch (const Exception& e) {

        }
        catch (const std::exception& e) {
            notifyHandlerException(e);
        }
        catch (...) {

        }
    }
    else {
        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::fireChannelInactive,
                        this,
                        boost::ref(ctx)));
    }
}

void ChannelHandlerContext::fireChannelActive() {
    ChannelHandlerContext* next = nextInboundContext;

    if (next) {
        fireChannelActive(*next);
    }
}

void ChannelHandlerContext::fireChannelActive(ChannelHandlerContext& ctx) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.inboundHandler->channelActive(ctx);
        }
        catch (const Exception& e) {

        }
        catch (const std::exception& e) {
            notifyHandlerException(e);
        }
        catch (...) {

        }
    }
    else {
        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::fireChannelActive,
                        this,
                        boost::ref(ctx)));
    }
}

void ChannelHandlerContext::fireExceptionCaught(const ChannelException& cause) {
    ChannelHandlerContext* next = nextInboundContext;

    if (next) {
        fireExceptionCaught(*next, cause);
    }
    else {
        //logger.warn(
        //    "An exceptionCaught() event was fired, and it reached at the end of the " +
        //    "pipeline.  It usually means the last inbound handler in the pipeline did not " +
        //    "handle the exception.", cause);
    }
}

void ChannelHandlerContext::fireExceptionCaught(ChannelHandlerContext& ctx,
        const ChannelException& cause) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.inboundHandler->exceptionCaught(ctx, cause);
        }
        catch (const Exception& e) {
            //logger.warn(
            //    "An exception was thrown by a user handler's " +
            //    "exceptionCaught() method while handling the following exception:", cause);
        }
        catch (const std::exception& e) {
            notifyHandlerException(e);
        }
        catch (...) {

        }
    }
    else {
        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::fireExceptionCaught,
                        this,
                        boost::ref(ctx),
                        cause));
    }
}

void ChannelHandlerContext::fireUserEventTriggered(const UserEvent& event) {
    ChannelHandlerContext* next = nextInboundContext;

    if (next) {
        fireUserEventTriggered(*next, event);
    }
}

void ChannelHandlerContext::fireUserEventTriggered(ChannelHandlerContext& ctx, const UserEvent& event) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.inboundHandler->userEventTriggered(ctx, event);
        }
        catch (const Exception& e) {
            //logger.warn(
            //    "An exception was thrown by a user handler's " +
            //    "exceptionCaught() method while handling the following exception:", cause);
        }
        catch (const std::exception& e) {
            notifyHandlerException(e);
        }
        catch (...) {

        }
    }
    else {
        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::fireUserEventTriggered,
                        this,
                        boost::ref(ctx),
                        boost::cref(event)));
    }
}

void ChannelHandlerContext::fireMessageUpdated() {
    ChannelHandlerContext* next = nextInboundContext;

    if (next) {
        fireMessageUpdated(*next);
    }
}

void ChannelHandlerContext::fireMessageUpdated(ChannelHandlerContext& ctx) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.inboundHandler->messageUpdated(ctx);
        }
        catch (const Exception& e) {
            //logger.warn(
            //    "An exception was thrown by a user handler's " +
            //    "exceptionCaught() method while handling the following exception:", cause);
        }
        catch (const std::exception& e) {
            notifyHandlerException(e);
        }
        catch (...) {

            // clear pipeline inbound ChannelBuffer
            //if (buf != null) {
            //    if (!buf.readable()) {
            //        buf.discardReadBytes();
            //    }
            //}

        }
    }
    else {
        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::fireMessageUpdated,
                        this,
                        boost::ref(ctx)));
    }
}

void ChannelHandlerContext::fireWriteCompleted() {
    ChannelHandlerContext* next = nextInboundContext;

    if (next) {
        fireWriteCompleted(*next);
    }
}

void ChannelHandlerContext::fireWriteCompleted(ChannelHandlerContext& ctx) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.inboundHandler->writeCompleted(ctx);
        }
        catch (const Exception& e) {
            //logger.warn(
            //    "An exception was thrown by a user handler's " +
            //    "exceptionCaught() method while handling the following exception:", cause);
        }
        catch (const std::exception& e) {
            notifyHandlerException(e);
        }
    }
    else {
        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::fireWriteCompleted,
                        this,
                        boost::ref(ctx)));
    }
}

ChannelFuturePtr ChannelHandlerContext::bind(const SocketAddress& localAddress) {
    return bind(localAddress, getChannel()->newFuture());
}

const ChannelFuturePtr& ChannelHandlerContext::bind(const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
    if (nextOutboundContext) {
        return bind(*nextOutboundContext, localAddress, future);
    }

    return future;
}

const ChannelFuturePtr& ChannelHandlerContext::bind(ChannelHandlerContext& ctx,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
    if (!localAddress.validated()) {
        future->setFailure(ChannelException("localAddress is invalid."));
        return future;
    }

    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.outboundHandler->bind(ctx, localAddress, future);
        }
        catch (const Exception& e) {
            //logger.warn(
            //    "An exception was thrown by a user handler's " +
            //    "exceptionCaught() method while handling the following exception:", cause);
        }
        catch (const std::exception& e) {
            notifyHandlerException(e);
        }
        catch (...) {
        }
    }
    else {
        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::bind,
                        this,
                        boost::ref(ctx),
                        localAddress,
                        future));
    }

    return future;
}

ChannelFuturePtr ChannelHandlerContext::connect(const SocketAddress& remoteAddress) {
    return connect(remoteAddress, getChannel()->newFuture());
}

ChannelFuturePtr ChannelHandlerContext::connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress) {
    return connect(remoteAddress, localAddress, getChannel()->newFuture());
}

const ChannelFuturePtr& ChannelHandlerContext::connect(const SocketAddress& remoteAddress,
        const ChannelFuturePtr& future) {
    return connect(remoteAddress, SocketAddress::NULL_ADDRESS, future);
}

const ChannelFuturePtr& ChannelHandlerContext::connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
    if (nextOutboundContext) {
        return connect(*nextOutboundContext, remoteAddress, localAddress, future);
    }
}

const ChannelFuturePtr& ChannelHandlerContext::connect(ChannelHandlerContext& ctx,
        const SocketAddress& remoteAddress,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {

    if (!remoteAddress.validated()) {
        future->setFailure(ChannelException("remoteAddress is invalid."));
        return future;
    }

    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.outboundHandler->connect(ctx, remoteAddress, localAddress, future);
        }
        catch (const Exception& e) {
            //logger.warn(
            //    "An exception was thrown by a user handler's " +
            //    "exceptionCaught() method while handling the following exception:", cause);
        }
        catch (const std::exception& e) {
            notifyHandlerException(e);
        }
        catch (...) {
        }
    }
    else {
        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::connect,
                        this,
                        boost::ref(ctx),
                        remoteAddress,
                        localAddress,
                        future));
    }

    return future;
}

cetty::channel::ChannelFuturePtr ChannelHandlerContext::disconnect() {
    return disconnect(getChannel()->newFuture());
}

const ChannelFuturePtr& ChannelHandlerContext::disconnect(const ChannelFuturePtr& future) {
    if (nextOutboundContext) {
        return disconnect(*nextOutboundContext, future);
    }
}

const ChannelFuturePtr& ChannelHandlerContext::disconnect(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    // TODO: for connectless, like UDP.
    // Translate disconnect to close if the channel has no notion of disconnect-reconnect.
    // So far, UDP/IP is the only transport that has such behavior.
    //if (!ctx.channel().hasDisconnect()) {
    //    return close(ctx, future);
    //}

    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.outboundHandler->disconnect(ctx, future);
        }
        catch (const Exception& e) {
            //logger.warn(
            //    "An exception was thrown by a user handler's " +
            //    "exceptionCaught() method while handling the following exception:", cause);
        }
        catch (const std::exception& e) {
            notifyHandlerException(e);
        }
        catch (...) {
        }
    }
    else {
        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::disconnect,
                        this,
                        boost::ref(ctx),
                        future));
    }

    return future;
}

cetty::channel::ChannelFuturePtr ChannelHandlerContext::close() {
    return close(getChannel()->newFuture());
}

const ChannelFuturePtr& ChannelHandlerContext::close(const ChannelFuturePtr& future) {
    if (nextOutboundContext) {
        return close(*nextOutboundContext, future);
    }
}

const ChannelFuturePtr& ChannelHandlerContext::close(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.outboundHandler->close(ctx, future);
        }
        catch (const Exception& e) {
            //logger.warn(
            //    "An exception was thrown by a user handler's " +
            //    "exceptionCaught() method while handling the following exception:", cause);
        }
        catch (const std::exception& e) {
            notifyHandlerException(e);
        }
        catch (...) {
        }
    }
    else {
        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::close,
                        this,
                        boost::ref(ctx),
                        future));
    }

    return future;
}

cetty::channel::ChannelFuturePtr ChannelHandlerContext::flush() {
    return flush(getChannel()->newFuture());
}

const ChannelFuturePtr& ChannelHandlerContext::flush(const ChannelFuturePtr& future) {
    if (nextOutboundContext) {
        return flush(*nextOutboundContext, future);
    }
}

const ChannelFuturePtr& ChannelHandlerContext::flush(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.outboundHandler->flush(ctx, future);
        }
        catch (const Exception& e) {
            //logger.warn(
            //    "An exception was thrown by a user handler's " +
            //    "exceptionCaught() method while handling the following exception:", cause);
        }
        catch (const std::exception& e) {
            notifyHandlerException(e);
        }
        catch (...) {
            // clear the outbound ChannelBuffer
            //if (ctx.outByteBuf != null) {
            //    ByteBuf buf = ctx.outByteBuf;

            //   if (!buf.readable()) {
            //       buf.discardReadBytes();
            //   }
            //}
        }
    }
    else {
        eventloop->post(boost::bind(&ChannelHandlerContext::flush,
                                    this,
                                    boost::ref(ctx),
                                    future));
    }

    return future;
}

ChannelFuturePtr ChannelHandlerContext::newFuture() {
    return getChannel()->newFuture();
}

ChannelFuturePtr ChannelHandlerContext::newSucceededFuture() {
    return getChannel()->newSucceededFuture();
}

ChannelFuturePtr ChannelHandlerContext::newFailedFuture(const ChannelException& cause) {
    return getChannel()->newFailedFuture(cause);
}

void ChannelHandlerContext::notifyHandlerException() {
    pipeline.notifyHandlerException(Exception(""));
}

void ChannelHandlerContext::notifyHandlerException(const std::exception& e) {
    pipeline.notifyHandlerException(Exception(e.what()));
}

void ChannelHandlerContext::notifyHandlerException(const Exception& e) {
    pipeline.notifyHandlerException(e);
}

ChannelInboundBufferHandlerContext* ChannelHandlerContext::inboundBufferHandlerContext() {
    return dynamic_cast<ChannelInboundBufferHandlerContext*>(this);
}

ChannelOutboundBufferHandlerContext* ChannelHandlerContext::outboundBufferHandlerContext() {
    return dynamic_cast<ChannelOutboundBufferHandlerContext*>(this);
}

void ChannelHandlerContext::init(const ChannelHandlerPtr& handler) {
    inboundHandler = boost::dynamic_pointer_cast<ChannelInboundHandler>(handler);

    if (inboundHandler) {
        this->canHandleInbound = true;
    }

    outboundHandler = boost::dynamic_pointer_cast<ChannelOutboundHandler>(handler);

    if (outboundHandler) {
        this->canHandleOutbound = true;
    }

    if (!canHandleInbound && !canHandleOutbound) {
        throw InvalidArgumentException(
            "handler must be either ChannelUpstreamHandler or ChannelDownstreamHandler.");
    }
}

void ChannelHandlerContext::attach() {
    if (!eventloop) {
        this->eventloop = pipeline.getEventLoop();
    }
}

void ChannelHandlerContext::detach() {
    if (eventloop == pipeline.getEventLoop()) {
        eventloop.reset();
    }
}

ChannelInboundBufferHandlerContext* ChannelHandlerContext::nextInboundBufferHandlerContext(ChannelHandlerContext* ctx) {
    ChannelHandlerContext* next = ctx;

    if (!next) {
        return (ChannelInboundBufferHandlerContext*)NULL;
    }

    if (next->hasInboundBufferHandler) {
        return dynamic_cast<ChannelInboundBufferHandlerContext*>(next);
    }

    next = next->nextInboundContext;

    while (true) {
        if (!next) {
            return (ChannelInboundBufferHandlerContext*)NULL;
        }

        if (next->hasInboundBufferHandler) {
            return dynamic_cast<ChannelInboundBufferHandlerContext*>(next);
        }

        next = next->nextInboundContext;
    }
}

ChannelOutboundBufferHandlerContext* ChannelHandlerContext::nextOutboundBufferHandlerContext(
    ChannelHandlerContext* ctx) {
    ChannelHandlerContext* next = ctx;

    if (!next) {
        return (ChannelOutboundBufferHandlerContext*)NULL;
    }

    if (next->hasOutboundBufferHandler) {
        return dynamic_cast<ChannelOutboundBufferHandlerContext*>(next);
    }

    next = next->nextOutboundContext;

    while (true) {
        if (!next) {
            return (ChannelOutboundBufferHandlerContext*)NULL;
        }

        if (next->hasOutboundBufferHandler) {
            return dynamic_cast<ChannelOutboundBufferHandlerContext*>(next);
        }

        next = next->nextOutboundContext;
    }
}

}
}