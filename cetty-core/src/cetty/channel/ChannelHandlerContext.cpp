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
#include <cetty/channel/ChannelPipelineException.h>
#include <cetty/channel/ChannelInboundBufferHandler.h>
#include <cetty/channel/ChannelOutboundBufferHandler.h>
#include <cetty/channel/ChannelInboundBufferHandlerContext.h>
#include <cetty/channel/ChannelOutboundBufferHandlerContext.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {

ChannelHandlerContext::ChannelHandlerContext(const std::string& name,
        ChannelPipeline& pipeline,
        const ChannelHandlerPtr& handler,
        ChannelHandlerContext* prev,
        ChannelHandlerContext* next)
    : canHandleInbound(false),
      canHandleOutbound(false),
      hasInboundBufferHandler(false),
      hasInboundMessageHandler(false),
      hasOutboundBufferHandler(false),
      hasOutboundMessageHandler(false),
      handler(handler),
      next(next),
      prev(prev),
      nextInboundContext(NULL),
      nextOutboundContext(NULL),
      name(name),
      pipeline(pipeline),
      eventloop() {
    init(handler);
}

ChannelHandlerContext::ChannelHandlerContext(const std::string& name,
        const EventLoopPtr& eventLoop,
        ChannelPipeline& pipeline,
        const ChannelHandlerPtr& handler,
        ChannelHandlerContext* prev,
        ChannelHandlerContext* next)
    : canHandleInbound(false),
      canHandleOutbound(false),
      hasInboundBufferHandler(false),
      hasInboundMessageHandler(false),
      hasOutboundBufferHandler(false),
      hasOutboundMessageHandler(false),
      handler(handler),
      next(next),
      prev(prev),
      nextInboundContext(NULL),
      nextOutboundContext(NULL),
      name(name),
      pipeline(pipeline),
      eventloop(eventloop) {
    init(handler);
}

inline
const ChannelPtr& ChannelHandlerContext::getChannel() const {
    return pipeline.getChannel();
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
        catch (const Exception& e) {
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's channelCreated() method";

            notifyHandlerException(
                ChannelPipelineException(e.getMessage(), e.getCode()));
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's channelCreated() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's"
                      " channelCreated() method";

            notifyHandlerException(
                ChannelPipelineException("unknown exception from handle channelCreated"));
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
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's channelInactive() method";

            notifyHandlerException(
                ChannelPipelineException(e.getMessage(), e.getCode()));
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's channelInactive() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's"
                      " channelInactive() method";

            notifyHandlerException(
                ChannelPipelineException("unknown exception from handle channelInactive"));
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
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's channelActive() method";

            notifyHandlerException(
                ChannelPipelineException(e.getMessage(), e.getCode()));
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's channelActive() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's"
                      " channelActive() method";

            notifyHandlerException(
                ChannelPipelineException("unknown exception from handle channelActive"));
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
    ChannelHandlerContext* next = this->next;

    if (next) {
        fireExceptionCaught(*next, cause);
    }
    else {
        LOG_WARN_E(cause) << "An exceptionCaught() event was fired, and it reached at the end of the "
                          "pipeline.  It usually means the last inbound handler in the pipeline did not "
                          "handle the exception.";
    }
}

void ChannelHandlerContext::fireExceptionCaught(ChannelHandlerContext& ctx,
        const ChannelException& cause) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.handler->exceptionCaught(ctx, cause);
        }
        catch (const Exception& e) {
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's "
                      "exceptionCaught() method while handling the following exception:"
                      << cause.getDisplayText();
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's "
                      "exceptionCaught() method while handling the following exception:"
                      << cause.getDisplayText();
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's "
                      "exceptionCaught() method while handling the following exception:"
                      << cause.getDisplayText();
        }
    }
    else {
        LOG_INFO << "fireExceptionCaught not in the same thread, will using eventloop->post.";

        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::fireExceptionCaught,
                        this,
                        boost::ref(ctx),
                        cause));
    }
}

void ChannelHandlerContext::fireUserEventTriggered(const boost::any& evt) {
    ChannelHandlerContext* next = this->next;

    if (next) {
        fireUserEventTriggered(*next, evt);
    }
}

void ChannelHandlerContext::fireUserEventTriggered(ChannelHandlerContext& ctx, const boost::any& evt) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.handler->userEventTriggered(ctx, evt);
        }
        catch (const Exception& e) {
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's userEventTriggered() method";

            notifyHandlerException(
                ChannelPipelineException(e.getMessage(), e.getCode()));
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's userEventTriggered() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's"
                      " userEventTriggered() method";

            notifyHandlerException(
                ChannelPipelineException("unknown exception from handle userEventTriggered"));
        }
    }
    else {
        ctx.eventloop->post(
            boost::bind(&ChannelHandlerContext::fireUserEventTriggered,
                        this,
                        boost::ref(ctx),
                        boost::cref(evt)));
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
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's messageUpdated() method";

            notifyHandlerException(ChannelPipelineException(e.getMessage(),
                                   e.getCode()));
            clearInboundChannelBuffer(ctx);
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's messageUpdated() method";

            notifyHandlerException(ChannelPipelineException(e.what()));
            clearInboundChannelBuffer(ctx);
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's"
                      " messageUpdated() method";

            notifyHandlerException(ChannelPipelineException(
                                       "unknown exception from handle messageUpdated"));
            clearInboundChannelBuffer(ctx);
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
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's writeCompleted() method";

            notifyHandlerException(
                ChannelPipelineException(e.getMessage(), e.getCode()));
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's writeCompleted() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's"
                      " writeCompleted() method";

            notifyHandlerException(
                ChannelPipelineException("unknown exception from handle writeCompleted"));
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
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's bind() method";

            notifyHandlerException(
                ChannelPipelineException(e.getMessage(), e.getCode()));
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's bind() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's"
                      " bind() method";

            notifyHandlerException(
                ChannelPipelineException("unknown exception from handle bind"));
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
    else {
        LOG_ERROR << "has no handler to handle to connect";
        future->setFailure(ChannelPipelineException("has no handler to handle to connect"));
        return future;
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
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's connect() method";

            notifyHandlerException(
                ChannelPipelineException(e.getMessage(), e.getCode()));
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's connect() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's"
                      " connect() method";

            notifyHandlerException(
                ChannelPipelineException("unknown exception from handle connect"));
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
    else {
        LOG_ERROR << "has no handler to handle to disconnect";
        future->setFailure(ChannelPipelineException("has no handler to handle to disconnect"));
        return future;
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
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's disconnect() method";

            notifyHandlerException(ChannelPipelineException(e.getMessage(),
                                   e.getCode()));
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's disconnect() method";

            notifyHandlerException(ChannelPipelineException(e.what()));
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's"
                      " disconnect() method";

            notifyHandlerException(ChannelPipelineException(
                                       "unknown exception from handle disconnect"));
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
    else {
        LOG_ERROR << "has no handler to handle to close";
        future->setFailure(ChannelPipelineException("has no handler to handle to close"));
        return future;
    }
}

const ChannelFuturePtr& ChannelHandlerContext::close(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.outboundHandler->close(ctx, future);
        }
        catch (const Exception& e) {
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's close() method";

            notifyHandlerException(
                ChannelPipelineException(e.getMessage(), e.getCode()));
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's close() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's"
                      " close() method";

            notifyHandlerException(
                ChannelPipelineException("unknown exception from handle close"));
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
    else {
        LOG_ERROR << "has no handler to handle to flush";
        future->setFailure(ChannelPipelineException("has no handler to handle to flush"));
        return future;
    }
}

const ChannelFuturePtr& ChannelHandlerContext::flush(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    if (ctx.eventloop->inLoopThread()) {
        try {
            ctx.outboundHandler->flush(ctx, future);
        }
        catch (const Exception& e) {
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's flush() method";

            notifyHandlerException(ChannelPipelineException(e.getMessage(),
                                   e.getCode()));
            clearOutboundChannelBuffer(ctx);
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's flush() method";

            notifyHandlerException(ChannelPipelineException(e.what()));
            clearOutboundChannelBuffer(ctx);
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's"
                      " flush() method";

            notifyHandlerException(ChannelPipelineException(
                                       "unknown exception from handle flush"));
            clearOutboundChannelBuffer(ctx);
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

void ChannelHandlerContext::notifyHandlerException(const ChannelPipelineException& e) {
    fireExceptionCaught(*this, e);
    //pipeline.notifyHandlerException(e);
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

ChannelInboundBufferHandlerContext* ChannelHandlerContext::nextInboundBufferHandlerContext(
    ChannelHandlerContext* ctx) {
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

ChannelInboundBufferHandlerContext* ChannelHandlerContext::nextInboundBufferHandlerContext() {
    return nextInboundBufferHandlerContext(nextInboundContext);
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

ChannelOutboundBufferHandlerContext* ChannelHandlerContext::nextOutboundBufferHandlerContext() {
    if (!canHandleOutbound) {
        ChannelHandlerContext* preCtx = prev;

        while (preCtx) {
            if (preCtx->canHandleOutbound) {
                return nextOutboundBufferHandlerContext(preCtx);
            }

            preCtx = preCtx->prev;
        }

        return NULL;
    }

    return nextOutboundBufferHandlerContext(nextOutboundContext);
}

ChannelHandlerContext* ChannelHandlerContext::getNextOutboundContext() {
    if (nextOutboundContext) {
        return nextOutboundContext;
    }

    ChannelHandlerContext* preCtx = prev;

    while (preCtx) {
        if (preCtx->canHandleOutbound) {
            return preCtx;
        }

        preCtx = preCtx->prev;
    }

    return NULL;
}

ChannelHandlerContext* ChannelHandlerContext::getNextInboundContext() {
    if (nextInboundContext) {
        return nextInboundContext;
    }

    ChannelHandlerContext* nextCtx = next;

    while (nextCtx) {
        if (nextCtx->canHandleInbound) {
            return nextCtx;
        }

        nextCtx = nextCtx->next;
    }

    return NULL;
}

void ChannelHandlerContext::clearOutboundChannelBuffer(ChannelHandlerContext& ctx) {
    boost::intrusive_ptr<ChannelOutboundBufferHandler>
    outboundBufferHandler =
        boost::dynamic_pointer_cast<ChannelOutboundBufferHandler>(ctx.outboundHandler);

    if (outboundBufferHandler) {
        const ChannelBufferPtr& buf = outboundBufferHandler->getOutboundChannelBuffer();

        if (!buf->readable()) {
            buf->discardReadBytes();
        }
    }
}

void ChannelHandlerContext::clearInboundChannelBuffer(ChannelHandlerContext& ctx) {
    boost::intrusive_ptr<ChannelInboundBufferHandler>
    inboundBufferHandler =
        boost::dynamic_pointer_cast<ChannelInboundBufferHandler>(ctx.inboundHandler);

    if (inboundBufferHandler) {
        const ChannelBufferPtr& buf = inboundBufferHandler->getInboundChannelBuffer();

        if (!buf->readable()) {
            buf->discardReadBytes();
        }
    }
}

}
}
