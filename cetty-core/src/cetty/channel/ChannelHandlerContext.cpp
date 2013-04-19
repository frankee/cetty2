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
#include <cetty/channel/NullChannel.h>
#include <cetty/channel/InetAddress.h>
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
      eventLoop_(),
      pipeline_() {
    resetNeighbourContexts();
}

ChannelHandlerContext::ChannelHandlerContext(const std::string& name,
        const EventLoopPtr& eventLoop)
    : name_(name),
      next_(),
      prev_(),
      eventLoop_(eventLoop),
      pipeline_() {
    resetNeighbourContexts();
}

void ChannelHandlerContext::fireChannelOpen() {
    bool& hasNext = hasNeighbourEventHandler_[EVT_CHANNEL_OPEN];
    ChannelHandlerContext*& nextCtx = neighbourEventHandler_[EVT_CHANNEL_OPEN];

    if (nextCtx) {
        fireChannelOpen(*nextCtx);
        return;
    }

    if (hasNext) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->channelOpenCallback_) {
                nextCtx = next;
                fireChannelOpen(*nextCtx);
                break;
            }

            next = next->next_;
        }

        hasNext = (next != NULL);
    }
}

void ChannelHandlerContext::fireChannelOpen(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelOpenCallback_) {
                ctx.channelOpenCallback_(ctx);
            }
            else {
                ChannelHandlerContext* next =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_OPEN];

                if (next) {
                    next->channelOpenCallback_(*next);
                }
                else {
                    ctx.fireChannelOpen();
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's channelOpen() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
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
    bool& hasNext = hasNeighbourEventHandler_[EVT_CHANNEL_INACTIVE];
    ChannelHandlerContext*& nextCtx = neighbourEventHandler_[EVT_CHANNEL_INACTIVE];

    if (nextCtx) {
        fireChannelInactive(*nextCtx);
        return;
    }

    if (hasNext) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->channelInactiveCallback_) {
                nextCtx = next;
                fireChannelInactive(*next);
                break;
            }

            next = next->next_;
        }

        hasNext = (next != NULL);
    }
}

void ChannelHandlerContext::fireChannelInactive(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelInactiveCallback_) {
                ctx.channelInactiveCallback_(ctx);
            }
            else {
                ChannelHandlerContext* next =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_INACTIVE];

                if (next) {
                    next->channelInactiveCallback_(*next);
                }
                else {
                    ctx.fireChannelInactive();
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's channelInactive() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
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
    bool& hasNext = hasNeighbourEventHandler_[EVT_CHANNEL_ACTIVE];
    ChannelHandlerContext*& nextCtx = neighbourEventHandler_[EVT_CHANNEL_ACTIVE];

    if (nextCtx) {
        fireChannelActive(*nextCtx);
        return;
    }

    if (hasNext) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->channelActiveCallback_) {
                nextCtx = next;
                fireChannelActive(*next);
                break;
            }

            next = next->next_;
        }

        hasNext = (next != NULL);
    }
}

void ChannelHandlerContext::fireChannelActive(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelActiveCallback_) {
                ctx.channelActiveCallback_(ctx);
            }
            else {
                ChannelHandlerContext* next =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_ACTIVE];

                if (next) {
                    next->channelActiveCallback_(*next);
                }
                else {
                    ctx.fireChannelActive();
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's channelActive() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
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
    bool& hasNext = hasNeighbourEventHandler_[EVT_CHANNEL_EXCEPTION];
    ChannelHandlerContext*& nextCtx = neighbourEventHandler_[EVT_CHANNEL_EXCEPTION];

    if (nextCtx) {
        fireExceptionCaught(*nextCtx, cause);
        return;
    }

    if (hasNext) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->exceptionCallback_) {
                nextCtx = next;
                fireExceptionCaught(*next, cause);
                break;
            }

            next = next->next_;
        }

        if (!next) {
            hasNext = false;

            LOG_WARN << "An exception(" << cause.displayText()
                     << ") event was fired, and it reached at the end of the pipeline."
                     " It usually means the last inbound handler in the pipeline did not "
                     "handle the exception, will close the channel.";

            channel_.lock()->close(newVoidFuture());
        }
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
                ChannelHandlerContext* next =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_EXCEPTION];

                if (next) {
                    next->exceptionCallback_(*next, cause);
                }
                else {
                    ctx.fireExceptionCaught(cause);
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's "
                      "exceptionCaught() method while handling the following exception:"
                      << cause.displayText();
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
    bool& hasNext = hasNeighbourEventHandler_[EVT_CHANNEL_USR_EVENT];
    ChannelHandlerContext*& nextCtx = neighbourEventHandler_[EVT_CHANNEL_USR_EVENT];

    if (nextCtx) {
        fireUserEventTriggered(*nextCtx, evt);
        return;
    }

    if (hasNext) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->userEventCallback_) {
                nextCtx = next;
                fireUserEventTriggered(*next, evt);
                break;
            }

            next = next->next_;
        }

        if (!next) {
            hasNext = false;

            LOG_WARN << "An user event was fired, and it reached at the end of the "
                     "pipeline.  It usually means the last inbound handler in the pipeline did not "
                     "handle the user event.";
        }
    }
}

void ChannelHandlerContext::fireUserEventTriggered(ChannelHandlerContext& ctx,
        const boost::any& evt) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.userEventCallback_) {
                ctx.userEventCallback_(ctx, evt);
            }
            else {
                ChannelHandlerContext* next =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_USR_EVENT];

                if (next) {
                    next->userEventCallback_(*next, evt);
                }
                else {
                    ctx.fireUserEventTriggered(evt);
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's userEventTriggered() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
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
    bool& hasNext = hasNeighbourEventHandler_[EVT_CHANNEL_MESSAGE_UPDATED];
    ChannelHandlerContext*& nextCtx = neighbourEventHandler_[EVT_CHANNEL_MESSAGE_UPDATED];

    if (nextCtx) {
        fireMessageUpdated(*nextCtx);
        return;
    }

    if (hasNext) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->channelMessageUpdatedCallback_) {
                nextCtx = next;
                fireMessageUpdated(*next);
                break;
            }

            next = next->next_;
        }

        hasNext = (next != NULL);
    }
}

void ChannelHandlerContext::fireMessageUpdated(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelMessageUpdatedCallback_) {
                ctx.channelMessageUpdatedCallback_(ctx);
            }
            else {
                ChannelHandlerContext* next =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_MESSAGE_UPDATED];

                if (next) {
                    next->channelMessageUpdatedCallback_(*next);
                }
                else {
                    ctx.fireMessageUpdated();
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's messageUpdated() method";

            notifyHandlerException(ChannelPipelineException(e.what()));
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

void ChannelHandlerContext::fireChannelReadSuspended() {
    bool& hasNext = hasNeighbourEventHandler_[EVT_CHANNEL_READ_SUSPENDED];
    ChannelHandlerContext*& nextCtx = neighbourEventHandler_[EVT_CHANNEL_READ_SUSPENDED];

    if (nextCtx) {
        fireChannelReadSuspended(*nextCtx);
        return;
    }

    if (hasNext) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->channelReadSuspendedCallback_) {
                nextCtx = next;
                fireChannelReadSuspended(*next);
                break;
            }

            next = next->next_;
        }

        hasNext = (next != NULL);
    }
}

void ChannelHandlerContext::fireChannelReadSuspended(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelReadSuspendedCallback_) {
                ctx.channelReadSuspendedCallback_(ctx);
            }
            else {
                ChannelHandlerContext* next =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_READ_SUSPENDED];

                if (next) {
                    next->channelReadSuspendedCallback_(*next);
                }
                else {
                    ctx.fireChannelReadSuspended();
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                << ") was thrown by a user handler's ChannelReadSuspended() method";

            notifyHandlerException(ChannelPipelineException(e.what()));
        }
    }
    else {
        ctx.eventLoop_->post(
            boost::bind(&ChannelHandlerContext::fireChannelReadSuspended,
            this,
            boost::ref(ctx)));
    }
}

const ChannelFuturePtr& ChannelHandlerContext::bind(const InetAddress& localAddress,
        const ChannelFuturePtr& future) {
            bool& hasPre = hasNeighbourEventHandler_[EVT_CHANNEL_BIND];
            ChannelHandlerContext*& preCtx = neighbourEventHandler_[EVT_CHANNEL_BIND];
    if (preCtx) {
        return bind(*preCtx, localAddress, future);
    }

    if (hasPre) {
        ChannelHandlerContext* prev = prev_;

        while (prev) {
            if (prev->bindFunctor_) {
                preCtx = prev;
                return bind(*prev, localAddress, future);
            }

            prev = prev->prev_;
        }

        if (!prev) {
            hasPre = false;
            LOG_ERROR << "has no handler to handle to bind";
        }
    }

    future->setFailure(ChannelPipelineException("has no handler to handle to bind"));
    return future;
}

const ChannelFuturePtr& ChannelHandlerContext::bind(ChannelHandlerContext& ctx,
        const InetAddress& localAddress,
        const ChannelFuturePtr& future) {
    if (!localAddress) {
        future->setFailure(ChannelException("localAddress is invalid."));
        return future;
    }

    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.bindFunctor_) {
                ctx.bindFunctor_(ctx, localAddress, future);
            }
            else {
                ChannelHandlerContext* prev =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_BIND];

                if (prev) {
                    prev->bindFunctor_(*prev, localAddress, future);
                }
                else {
                    ctx.bind(localAddress, future);
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's bind() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
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

const ChannelFuturePtr& ChannelHandlerContext::connect(const InetAddress& remoteAddress,
        const InetAddress& localAddress,
        const ChannelFuturePtr& future) {
            bool& hasPre = hasNeighbourEventHandler_[EVT_CHANNEL_CONNECT];
            ChannelHandlerContext*& preCtx = neighbourEventHandler_[EVT_CHANNEL_CONNECT];
    if (preCtx) {
        return connect(*preCtx, remoteAddress, localAddress, future);
    }

    if (hasPre) {
        ChannelHandlerContext* prev = prev_;

        while (prev) {
            if (prev->connectFunctor_) {
                preCtx = prev;
                return connect(*prev, remoteAddress, localAddress, future);
            }

            prev = prev->prev_;
        }

        if (!prev) {
            hasPre = false;
            LOG_ERROR << "has no handler to handle to connect";
        }
    }

    future->setFailure(ChannelPipelineException("has no handler to handle to connect"));
    return future;
}

const ChannelFuturePtr& ChannelHandlerContext::connect(ChannelHandlerContext& ctx,
        const InetAddress& remoteAddress,
        const InetAddress& localAddress,
        const ChannelFuturePtr& future) {

    if (!remoteAddress) {
        future->setFailure(ChannelException("remoteAddress is invalid."));
        return future;
    }

    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.connectFunctor_) {
                ctx.connectFunctor_(ctx,
                    remoteAddress,
                    localAddress,
                    future);
            }
            else {
                ChannelHandlerContext* prev =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_CONNECT];

                if (prev) {
                    prev->connectFunctor_(*prev,
                        remoteAddress,
                        localAddress,
                        future);
                }
                else {
                    ctx.connect(remoteAddress,
                        localAddress,
                        future);
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's connect() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
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

const ChannelFuturePtr& ChannelHandlerContext::disconnect(
    const ChannelFuturePtr& future) {
    bool& hasPre = hasNeighbourEventHandler_[EVT_CHANNEL_DISCONNECT];
    ChannelHandlerContext*& preCtx = neighbourEventHandler_[EVT_CHANNEL_DISCONNECT];
    if (preCtx) {
        return disconnect(*preCtx, future);
    }

    if (hasPre) {
        ChannelHandlerContext* prev = prev_;

        while (prev) {
            if (prev->disconnectFunctor_) {
                preCtx = prev;
                return disconnect(*prev, future);
            }

            prev = prev->prev_;
        }

        if (!prev) {
            hasPre = false;
            LOG_ERROR << "has no handler to handle to disconnect";
        }
    }

    future->setFailure(ChannelPipelineException(
                           "has no handler to handle to disconnect"));

    return future;
}

const ChannelFuturePtr& ChannelHandlerContext::disconnect(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {

    // TODO: for connect less, like UDP.
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
                ChannelHandlerContext* prev =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_DISCONNECT];

                if (prev) {
                    prev->disconnectFunctor_(*prev,future);
                }
                else {
                    ctx.disconnect(future);
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's disconnect() method";

            notifyHandlerException(ChannelPipelineException(e.what()));
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

const ChannelFuturePtr& ChannelHandlerContext::close() {
    return close(newFuture());
}

const ChannelFuturePtr& ChannelHandlerContext::close(const ChannelFuturePtr& future) {
    bool& hasPre = hasNeighbourEventHandler_[EVT_CHANNEL_CLOSE];
    ChannelHandlerContext*& preCtx = neighbourEventHandler_[EVT_CHANNEL_CLOSE];

    if (preCtx) {
        return close(*preCtx, future);
    }

    if (hasPre) {
        ChannelHandlerContext* prev = prev_;

        while (prev) {
            if (prev->closeFunctor_) {
                preCtx = prev;
                return close(*prev, future);
            }

            prev = prev->prev_;
        }

        if (!prev) {
            hasPre = false;
            LOG_ERROR << "has no handler to handle to close";
        }
    }

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
                ChannelHandlerContext* prev =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_CLOSE];

                if (prev) {
                    prev->closeFunctor_(*prev, future);
                }
                else {
                    ctx.close(future);
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's close() method";

            notifyHandlerException(
                ChannelPipelineException(e.what()));
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

const ChannelFuturePtr& ChannelHandlerContext::flush() {
    return flush(newFuture());
}

const ChannelFuturePtr& ChannelHandlerContext::flush(const ChannelFuturePtr& future) {
    bool& hasPre = hasNeighbourEventHandler_[EVT_CHANNEL_FLUSH];
    ChannelHandlerContext*& preCtx = neighbourEventHandler_[EVT_CHANNEL_FLUSH];

    if (preCtx) {
        return flush(*preCtx, future);
    }

    if (hasPre) {
        ChannelHandlerContext* prev = prev_;

        while (prev) {
            if (prev->flushFunctor_) {
                preCtx = prev;
                return flush(*prev, future);
            }

            prev = prev->prev_;
        }

        if (!prev) {
            hasPre = false;
            LOG_ERROR << "has no handler to handle to flush";
        }
    }

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
                ChannelHandlerContext* prev =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_FLUSH];

                if (prev) {
                    prev->flushFunctor_(*prev, future);
                }
                else {
                    ctx.flush(future);
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                      << ") was thrown by a user handler's flush() method";

            notifyHandlerException(ChannelPipelineException(e.what()));
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

void ChannelHandlerContext::read() {
    bool& hasPre = hasNeighbourEventHandler_[EVT_CHANNEL_READ];
    ChannelHandlerContext*& preCtx = neighbourEventHandler_[EVT_CHANNEL_READ];

    if (preCtx) {
        return read(*preCtx);
    }

    if (hasPre) {
        ChannelHandlerContext* prev = prev_;

        while (prev) {
            if (prev->readFunctor_) {
                preCtx = prev;
                return read(*prev);
            }

            prev = prev->prev_;
        }

        if (!prev) {
            hasPre = false;
            LOG_ERROR << "has no handler to handle to flush";
        }
    }
}

void ChannelHandlerContext::read(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.readFunctor_) {
                ctx.readFunctor_(ctx);
            }
            else {
                ChannelHandlerContext* prev =
                    ctx.neighbourEventHandler_[EVT_CHANNEL_READ];

                if (prev) {
                    prev->readFunctor_(*prev);
                }
                else {
                    ctx.read();
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR << "An exception (" << e.what()
                << ") was thrown by a user handler's flush() method";

            notifyHandlerException(ChannelPipelineException(e.what()));
        }
    }
    else {
        ctx.eventLoop_->post(
            boost::bind(&ChannelHandlerContext::read,
            this,
            boost::ref(ctx)));
    }
}

ChannelFuturePtr ChannelHandlerContext::newFuture() {
    if (!channel_.expired()) {
        return new DefaultChannelFuture(channel_, false);
    }
    else {
        LOG_ERROR << "the channel in context has been destroyed, may not happened.";
        return new DefaultChannelFuture(NullChannel::instance(), false);
    }
}

ChannelFuturePtr ChannelHandlerContext::newVoidFuture() {
    return pipeline_->voidFuture();
}

ChannelFuturePtr ChannelHandlerContext::newSucceededFuture() {
    if (!channel_.expired()) {
        return channel_.lock()->newSucceededFuture();
    }
    else {
        LOG_ERROR << "the channel in context has been destroyed, may not happened.";
        return NullChannel::instance()->newSucceededFuture();
    }
}

ChannelFuturePtr ChannelHandlerContext::newFailedFuture(const ChannelException& cause) {
    if (!channel_.expired()) {
        return channel_.lock()->newFailedFuture(cause);
    }
    else {
        LOG_ERROR << "the channel in context has been destroyed, may not happened.";
        return NullChannel::instance()->newFailedFuture(cause);
    }
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

void ChannelHandlerContext::initialize(const ChannelPipeline& pipeline) {
    pipeline_ = &pipeline;
    channel_ = pipeline.channel();

    if (!eventLoop_) {
        eventLoop_ = pipeline.eventLoop();
    }
}

void ChannelHandlerContext::onPipelineChanged() {
    resetNeighbourContexts();
}

}
}
