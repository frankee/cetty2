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
#include <cetty/channel/DefaultChannelFuture.h>
#include <cetty/channel/ChannelPipelineException.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {

ChannelHandlerContext::ChannelHandlerContext(const std::string& name)
    : name_(name),
      next_(),
      prev_(),
      pipeline_(),
      eventLoop_() {
}

ChannelHandlerContext::ChannelHandlerContext(const std::string& name,
        const EventLoopPtr& eventLoop)
    : name_(name),
      next_(),
      prev_(),
      pipeline_(),
      eventLoop_(eventLoop) {
}

void ChannelHandlerContext::fireChannelOpen() {
    ChannelHandlerContext* next = next_;

    do {
        if (next->channelOpenCallback_) {
            fireChannelOpen(*next);
            break;
        }

        next = next->next_;
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
    ChannelHandlerContext* next = next_;

    do {
        if (next->channelInactiveCallback_) {
            fireChannelInactive(*next);
            break;
        }

        next = next->next_;
    }
    while (next);
}

void ChannelHandlerContext::fireChannelInactive(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelInactiveCallback_) {
                ctx.channelInactiveCallback_(ctx);
            }
            else {
                fireChannelInactive();
            }
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
    ChannelHandlerContext* next = next_;

    do {
        if (next->channelActiveCallback_) {
            fireChannelActive(*next);
            break;
        }

        next = next->next_;
    }
    while (next);
}

void ChannelHandlerContext::fireChannelActive(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelActiveCallback_) {
                ctx.channelActiveCallback_(ctx);
            }
            else {
                fireChannelActive();
            }
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
    ChannelHandlerContext* next = next_;

    do {
        if (next->exceptionCallback_) {
            fireExceptionCaught(*next, cause);
            break;
        }

        next = next->next_;
    }
    while (next);

    if (!next) {
        LOG_WARN << "An exceptionCaught() event was fired, and it reached at the end of the "
                 "pipeline.  It usually means the last inbound handler in the pipeline did not "
                 "handle the exception.";
    }
}

void ChannelHandlerContext::fireExceptionCaught(ChannelHandlerContext& ctx,
        const ChannelException& cause) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.exceptionCallback_) {
                ctx.exceptionCallback_(ctx, cause);
            }
            else {
                fireExceptionCaught(cause);
            }
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
    ChannelHandlerContext* next = next_;

    do {
        if (next->userEventCallback_) {
            fireUserEventTriggered(*next, evt);
            break;
        }

        next = next->next_;
    }
    while (next);
}

void ChannelHandlerContext::fireUserEventTriggered(ChannelHandlerContext& ctx,
        const boost::any& evt) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.userEventCallback_) {
                ctx.userEventCallback_(ctx, evt);
            }
            else {
                fireUserEventTriggered(evt);
            }
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
    ChannelHandlerContext* next = next_;

    do {
        if (next->channelMessageUpdatedCallback_) {
            fireMessageUpdated(*next);
            break;
        }

        next = next->next_;
    }
    while (next);
}

void ChannelHandlerContext::fireMessageUpdated(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelMessageUpdatedCallback_) {
                ctx.channelMessageUpdatedCallback_(ctx);
            }
            else {
                fireMessageUpdated();
            }
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
    ChannelHandlerContext* before = prev_;

    do {
        if (before->bindFunctor_) {
            return bind(*before, localAddress, future);
            break;
        }

        before = before->prev_;
    }
    while (before);

    LOG_ERROR << "has no handler to handle to bind";
    future->setFailure(ChannelPipelineException("has no handler to handle to bind"));
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
            if (ctx.bindFunctor_) {
                ctx.bindFunctor_(ctx, localAddress, future);
            }
            else {
                bind(localAddress, future);
            }
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
    ChannelHandlerContext* before = prev_;

    do {
        if (before->connectFunctor_) {
            return connect(*before, remoteAddress, localAddress, future);
            break;
        }

        before = before->prev_;
    }
    while (before);

    LOG_ERROR << "has no handler to handle to connect";
    future->setFailure(ChannelPipelineException("has no handler to handle to connect"));
    return future;
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
            if (ctx.connectFunctor_) {
                ctx.connectFunctor_(ctx, remoteAddress, localAddress, future);
            }
            else {
                connect(localAddress, remoteAddress, future);
            }
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
    ChannelHandlerContext* before = prev_;

    do {
        if (before->disconnectFunctor_) {
            return disconnect(*before, future);
            break;
        }

        before = before->prev_;
    }
    while (before);

    LOG_ERROR << "has no handler to handle to disconnect";
    future->setFailure(ChannelPipelineException("has no handler to handle to disconnect"));
    return future;
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
            if (ctx.disconnectFunctor_) {
                ctx.disconnectFunctor_(ctx, future);
            }
            else {
                disconnect(future);
            }
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
    ChannelHandlerContext* before = prev_;

    do {
        if (before->closeFunctor_) {
            return close(*before, future);
            break;
        }

        before = before->prev_;
    }
    while (before);

    LOG_ERROR << "has no handler to handle to close";
    future->setFailure(ChannelPipelineException("has no handler to handle to close"));
    return future;
}

const ChannelFuturePtr& ChannelHandlerContext::close(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.closeFunctor_) {
                ctx.closeFunctor_(ctx, future);
            }
            else {
                close(future);
            }
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
    ChannelHandlerContext* before = prev_;

    do {
        if (before->flushFunctor_) {
            return flush(*before, future);
            break;
        }

        before = before->prev_;
    }
    while (before);

    LOG_ERROR << "has no handler to handle to flush";
    future->setFailure(ChannelPipelineException("has no handler to handle to flush"));
    return future;
}

const ChannelFuturePtr& ChannelHandlerContext::flush(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.flushFunctor_) {
                ctx.flushFunctor_(ctx, future);
            }
            else {
                flush(future);
            }
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
        ctx.eventLoop_->post(
            boost::bind(&ChannelHandlerContext::flush,
                        this,
                        boost::ref(ctx),
                        future));
    }

    return future;
}

ChannelFuturePtr ChannelHandlerContext::newFuture() {
    if (!channel_.expired()) {
        return new DefaultChannelFuture(channel_, false);
    }
}

ChannelFuturePtr ChannelHandlerContext::newSucceededFuture() {
    if (!channel_.expired()) {
        return channel_.lock()->newSucceededFuture();
    }
}

ChannelFuturePtr ChannelHandlerContext::newFailedFuture(const ChannelException& cause) {
    return channel_.lock()->newFailedFuture(cause);
}

void ChannelHandlerContext::notifyHandlerException(const ChannelPipelineException& e) {
    fireExceptionCaught(*this, e);
    //pipeline.notifyHandlerException(e);
}

void ChannelHandlerContext::clearOutboundChannelBuffer(ChannelHandlerContext& ctx) {
    ChannelBufferContainer* container =
        outboundMessageContainer<ChannelBufferContainer>();

    if (container) {
        const ChannelBufferPtr& buf = container->getMessages();

        if (!buf->readable()) {
            buf->discardReadBytes();
        }
    }
}

void ChannelHandlerContext::clearInboundChannelBuffer(ChannelHandlerContext& ctx) {
    ChannelBufferContainer* container =
        inboundMessageContainer<ChannelBufferContainer>();

    if (container) {
        const ChannelBufferPtr& buf = container->getMessages();

        if (!buf->readable()) {
            buf->discardReadBytes();
        }
    }
}

void ChannelHandlerContext::initialize(ChannelPipeline* pipeline) {
    pipeline_ = pipeline;
    channel_ = pipeline_->channel();

    if (!eventLoop_) {
        eventLoop_ = pipeline_->eventLoop();
    }
}

}
}
