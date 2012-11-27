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
        ChannelHandlerContext* prev,
        ChannelHandlerContext* next)
    : next(next),
      prev(prev),
      nextInboundContext_(NULL),
      nextOutboundContext_(NULL),
      name(name),
      pipeline(pipeline),
      eventLoop() {
    init();
}

ChannelHandlerContext::ChannelHandlerContext(const std::string& name,
        const EventLoopPtr& eventLoop,
        ChannelPipeline& pipeline,
        ChannelHandlerContext* prev,
        ChannelHandlerContext* next)
    : next(next),
      prev(prev),
      nextInboundContext_(NULL),
      nextOutboundContext_(NULL),
      name(name),
      pipeline(pipeline),
      eventLoop(eventLoop) {
    init();
}

const ChannelPtr& ChannelHandlerContext::channel() const {
    return pipeline->channel();
}

void ChannelHandlerContext::fireChannelOpen() {
    ChannelHandlerContext* next = nextInboundContext_;
    do {
        if (next->channelOpenCallback_) {
            fireChannelOpen(*next);
            break;
        }

        next = next->nextInboundContext_;
    }
    while (next);
}

void ChannelHandlerContext::fireChannelOpen(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelOpenCallback_) {
                ctx.channelOpenCallback_(ctx);
            }
            else {
                fireChannelOpen();
            }
        }
        catch (const Exception& e) {
            LOG_ERROR << "An exception (" << e.getDisplayText()
                      << ") was thrown by a user handler's channelOpen() method";

            notifyHandlerException(
                ChannelPipelineException(e.getMessage(), e.getCode()));
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's channelOpen() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
        }
        catch (...) {
            LOG_ERROR << "An unknown exception was thrown by a user handler's"
                      " channelOpen() method";

            notifyHandlerException(
                ChannelPipelineException("unknown exception from handle channelOpen"));
        }
    }
    else {
        ctx.eventLoop_->post(
            boost::bind(&ChannelHandlerContext::fireChannelOpen,
                        this,
                        boost::ref(ctx)));
    }
}

void ChannelHandlerContext::fireChannelInactive() {
    ChannelHandlerContext* next = nextInboundContext_;

    if (next) {
        fireChannelInactive(*next);
    }
}

void ChannelHandlerContext::fireChannelInactive(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            ctx.channelInactiveCallback_(ctx);
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
        ctx.eventLoop_->post(
            boost::bind(&ChannelHandlerContext::fireChannelInactive,
                        this,
                        boost::ref(ctx)));
    }
}

void ChannelHandlerContext::fireChannelActive() {
    ChannelHandlerContext* next = nextInboundContext_;

    if (next) {
        fireChannelActive(*next);
    }
}

void ChannelHandlerContext::fireChannelActive(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            ctx.channelActiveCallback_(ctx);
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
        ctx.eventLoop_->post(
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
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            ctx.exceptionCallback_(ctx, cause);
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
        LOG_INFO << "fireExceptionCaught not in the same thread, will using eventLoop->post.";

        ctx.eventLoop_->post(
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

void ChannelHandlerContext::fireUserEventTriggered(ChannelHandlerContext& ctx,
    const boost::any& evt) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            ctx.userEventCallback_(ctx, evt);
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
        ctx.eventLoop_->post(
            boost::bind(&ChannelHandlerContext::fireUserEventTriggered,
                        this,
                        boost::ref(ctx),
                        boost::cref(evt)));
    }
}

void ChannelHandlerContext::fireMessageUpdated() {
    ChannelHandlerContext* next = nextInboundContext_;

    if (next) {
        fireMessageUpdated(*next);
    }
}

void ChannelHandlerContext::fireMessageUpdated(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            ctx.channelMessageUpdatedCallback_(ctx);
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
        ctx.eventLoop_->post(
            boost::bind(&ChannelHandlerContext::fireMessageUpdated,
                        this,
                        boost::ref(ctx)));
    }
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

    if (ctx.eventLoop_->inLoopThread()) {
        try {
            ctx.bindFunctor_(ctx, localAddress, future);
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
        ctx.eventLoop_->post(
            boost::bind(&ChannelHandlerContext::bind,
                        this,
                        boost::ref(ctx),
                        localAddress,
                        future));
    }

    return future;
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

    if (ctx.eventLoop_->inLoopThread()) {
        try {
            ctx.connectFunctor_(ctx, remoteAddress, localAddress, future);
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
        ctx.eventLoop_->post(
            boost::bind(&ChannelHandlerContext::connect,
                        this,
                        boost::ref(ctx),
                        remoteAddress,
                        localAddress,
                        future));
    }

    return future;
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

    if (ctx.eventLoop_->inLoopThread()) {
        try {
            ctx.disconnectFunctor_(ctx, future);
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
        ctx.eventLoop_->post(
            boost::bind(&ChannelHandlerContext::disconnect,
                        this,
                        boost::ref(ctx),
                        future));
    }

    return future;
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
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            ctx.closeFunctor_(ctx, future);
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
        ctx.eventLoop_->post(
            boost::bind(&ChannelHandlerContext::close,
                        this,
                        boost::ref(ctx),
                        future));
    }

    return future;
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
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            ctx.flushFunctor_(ctx, future);
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
        ctx.eventLoop->post(
            boost::bind(&ChannelHandlerContext::flush,
                                    this,
                                    boost::ref(ctx),
                                    future));
    }

    return future;
}

ChannelFuturePtr ChannelHandlerContext::newFuture() {
    return channel()->newFuture();
}

ChannelFuturePtr ChannelHandlerContext::newSucceededFuture() {
    return channel()->newSucceededFuture();
}

ChannelFuturePtr ChannelHandlerContext::newFailedFuture(const ChannelException& cause) {
    return channel()->newFailedFuture(cause);
}

void ChannelHandlerContext::notifyHandlerException(const ChannelPipelineException& e) {
    fireExceptionCaught(*this, e);
    //pipeline.notifyHandlerException(e);
}

void ChannelHandlerContext::init() {
    
}

void ChannelHandlerContext::attach() {
    if (!eventLoop) {
        this->eventLoop = pipeline.eventLoop();
    }
}

void ChannelHandlerContext::detach() {
    if (eventLoop == pipeline.eventLoop()) {
        eventLoop.reset();
    }
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
    if (nextInboundContext_) {
        return nextInboundContext_;
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
