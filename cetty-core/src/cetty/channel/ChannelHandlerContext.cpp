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
      hasNextUserEventHandler_(true),
      hasNextMessageUpdatedHandler_(true),
      hasNextChannelOpenHandler_(true),
      hasNextChannelActiveHandler_(true),
      hasNextChannelInactiveHandler_(true),
      hasNextChannelExceptionHandler_(true),
      hasPrevBindHandler_(true),
      hasPrevConnectHandler_(true),
      hasPrevDisconnectHandler_(true),
      hasPrevCloseHandler_(true),
      hasPrevFlushHandler_(true),
      nextUserEventHandler_(),
      nextMessageUpdatedHandler_(),
      nextChannelOpenHandler_(),
      nextChannelActiveHandler_(),
      nextChannelInactiveHandler_(),
      nextChannelExceptionHandler_(),
      prevBindHandler_(),
      prevConnectHandler_(),
      prevDisconnectHandler_(),
      prevCloseHandler_(),
      prevFlushHandler_(),
      pipeline_(),
      eventLoop_() {
}

ChannelHandlerContext::ChannelHandlerContext(const std::string& name,
        const EventLoopPtr& eventLoop)
    : name_(name),
      next_(),
      prev_(),
      hasNextUserEventHandler_(true),
      hasNextMessageUpdatedHandler_(true),
      hasNextChannelOpenHandler_(true),
      hasNextChannelActiveHandler_(true),
      hasNextChannelInactiveHandler_(true),
      hasNextChannelExceptionHandler_(true),
      hasPrevBindHandler_(true),
      hasPrevConnectHandler_(true),
      hasPrevDisconnectHandler_(true),
      hasPrevCloseHandler_(true),
      hasPrevFlushHandler_(true),
      nextUserEventHandler_(),
      nextMessageUpdatedHandler_(),
      nextChannelOpenHandler_(),
      nextChannelActiveHandler_(),
      nextChannelInactiveHandler_(),
      nextChannelExceptionHandler_(),
      prevBindHandler_(),
      prevConnectHandler_(),
      prevDisconnectHandler_(),
      prevCloseHandler_(),
      prevFlushHandler_(),
      pipeline_(),
      eventLoop_(eventLoop) {
}

void ChannelHandlerContext::fireChannelOpen() {
    if (nextChannelOpenHandler_) {
        fireChannelOpen(*nextChannelOpenHandler_);
        return;
    }

    if (hasNextChannelOpenHandler_) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->channelOpenCallback_) {
                nextChannelOpenHandler_ = next;
                fireChannelOpen(*next);
                break;
            }

            next = next->next_;
        }

        hasNextChannelOpenHandler_ = (next != NULL);
    }
}

void ChannelHandlerContext::fireChannelOpen(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelOpenCallback_) {
                ctx.channelOpenCallback_(ctx);
            }
            else {
                ChannelHandlerContext* next = ctx.nextChannelOpenHandler_;

                if (next) {
                    next->channelOpenCallback_(*next);
                }
                else {
                    ctx.fireChannelOpen();
                }
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
    if (nextChannelInactiveHandler_) {
        fireChannelInactive(*nextChannelInactiveHandler_);
        return;
    }

    if (hasNextChannelInactiveHandler_) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->channelInactiveCallback_) {
                nextChannelInactiveHandler_ = next;
                fireChannelInactive(*next);
                break;
            }

            next = next->next_;
        }

        hasNextChannelInactiveHandler_ = (next != NULL);
    }
}

void ChannelHandlerContext::fireChannelInactive(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelInactiveCallback_) {
                ctx.channelInactiveCallback_(ctx);
            }
            else {
                ChannelHandlerContext* next = ctx.nextChannelInactiveHandler_;

                if (next) {
                    next->channelInactiveCallback_(*next);
                }
                else {
                    ctx.fireChannelInactive();
                }
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
    if (nextChannelActiveHandler_) {
        fireChannelActive(*nextChannelActiveHandler_);
        return;
    }

    if (hasNextChannelActiveHandler_) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->channelActiveCallback_) {
                nextChannelActiveHandler_ = next;
                fireChannelActive(*next);
                break;
            }

            next = next->next_;
        }

        hasNextChannelActiveHandler_ = (next != NULL);
    }
}

void ChannelHandlerContext::fireChannelActive(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelActiveCallback_) {
                ctx.channelActiveCallback_(ctx);
            }
            else {
                ChannelHandlerContext* next = ctx.nextChannelActiveHandler_;

                if (next) {
                    next->channelActiveCallback_(*next);
                }
                else {
                    ctx.fireChannelActive();
                }
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
    if (nextChannelExceptionHandler_) {
        fireExceptionCaught(*nextChannelExceptionHandler_, cause);
        return;
    }

    if (hasNextChannelExceptionHandler_) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->exceptionCallback_) {
                nextChannelExceptionHandler_ = next;
                fireExceptionCaught(*next, cause);
                break;
            }

            next = next->next_;
        }

        if (!next) {
            hasNextChannelActiveHandler_ = false;

            LOG_WARN << "An exceptionCaught() event was fired, and it reached at the end of the "
                     "pipeline.  It usually means the last inbound handler in the pipeline did not "
                     "handle the exception.";
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
                ChannelHandlerContext* next = ctx.nextChannelExceptionHandler_;

                if (next) {
                    next->exceptionCallback_(*next, cause);
                }
                else {
                    ctx.fireExceptionCaught(cause);
                }
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
    if (nextUserEventHandler_) {
        fireUserEventTriggered(*nextUserEventHandler_, evt);
        return;
    }

    if (hasNextUserEventHandler_) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->userEventCallback_) {
                nextUserEventHandler_ = next;
                fireUserEventTriggered(*next, evt);
                break;
            }

            next = next->next_;
        }

        if (!next) {
            hasNextUserEventHandler_ = false;

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
                ChannelHandlerContext* next = ctx.nextUserEventHandler_;

                if (next) {
                    next->userEventCallback_(*next, evt);
                }
                else {
                    ctx.fireUserEventTriggered(evt);
                }
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
    if (nextMessageUpdatedHandler_) {
        fireMessageUpdated(*nextMessageUpdatedHandler_);
        return;
    }

    if (hasNextMessageUpdatedHandler_) {
        ChannelHandlerContext* next = next_;

        while (next) {
            if (next->channelMessageUpdatedCallback_) {
                nextMessageUpdatedHandler_ = next;
                fireMessageUpdated(*next);
                break;
            }

            next = next->next_;
        }

        hasNextMessageUpdatedHandler_ = (next != NULL);
    }
}

void ChannelHandlerContext::fireMessageUpdated(ChannelHandlerContext& ctx) {
    if (ctx.eventLoop_->inLoopThread()) {
        try {
            if (ctx.channelMessageUpdatedCallback_) {
                ctx.channelMessageUpdatedCallback_(ctx);
            }
            else {
                ChannelHandlerContext* next = ctx.nextMessageUpdatedHandler_;

                if (next) {
                    next->channelMessageUpdatedCallback_(*next);
                }
                else {
                    ctx.fireMessageUpdated();
                }
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

const ChannelFuturePtr& ChannelHandlerContext::bind(const InetAddress& localAddress,
        const ChannelFuturePtr& future) {

    if (prevBindHandler_) {
        return bind(*prevBindHandler_, localAddress, future);
    }

    if (hasPrevBindHandler_) {
        ChannelHandlerContext* prev = prev_;

        while (prev) {
            if (prev->bindFunctor_) {
                prevBindHandler_ = prev;
                return bind(*prev, localAddress, future);
            }

            prev = prev->prev_;
        }

        if (!prev) {
            hasPrevBindHandler_ = false;
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
                ChannelHandlerContext* prev = ctx.prevBindHandler_;

                if (prev) {
                    prev->bindFunctor_(*prev, localAddress, future);
                }
                else {
                    ctx.bind(localAddress, future);
                }
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

const ChannelFuturePtr& ChannelHandlerContext::connect(const InetAddress& remoteAddress,
        const InetAddress& localAddress,
        const ChannelFuturePtr& future) {
    if (prevConnectHandler_) {
        return connect(*prevConnectHandler_, remoteAddress, localAddress, future);
    }

    if (hasPrevConnectHandler_) {
        ChannelHandlerContext* prev = prev_;

        while (prev) {
            if (prev->connectFunctor_) {
                prevConnectHandler_ = prev;
                return connect(*prev, remoteAddress, localAddress, future);
            }

            prev = prev->prev_;
        }

        if (!prev) {
            hasPrevConnectHandler_ = false;
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
                ctx.connectFunctor_(ctx, remoteAddress, localAddress, future);
            }
            else {
                ChannelHandlerContext* prev = ctx.prevConnectHandler_;

                if (prev) {
                    prev->connectFunctor_(*prev, remoteAddress, localAddress, future);
                }
                else {
                    ctx.connect(remoteAddress, localAddress, future);
                }
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
    if (prevDisconnectHandler_) {
        return disconnect(*prevDisconnectHandler_, future);
    }

    if (hasPrevDisconnectHandler_) {
        ChannelHandlerContext* prev = prev_;

        while (prev) {
            if (prev->disconnectFunctor_) {
                prevDisconnectHandler_ = prev;
                return disconnect(*prev, future);
            }

            prev = prev->prev_;
        }

        if (!prev) {
            hasPrevDisconnectHandler_ = false;
            LOG_ERROR << "has no handler to handle to disconnect";
        }
    }

    future->setFailure(ChannelPipelineException(
                           "has no handler to handle to disconnect"));

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
                ChannelHandlerContext* prev = ctx.prevDisconnectHandler_;

                if (prev) {
                    prev->disconnectFunctor_(*prev,future);
                }
                else {
                    ctx.disconnect(future);
                }
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

const ChannelFuturePtr& ChannelHandlerContext::close() {
    return close(newFuture());
}

const ChannelFuturePtr& ChannelHandlerContext::close(const ChannelFuturePtr& future) {
    if (prevCloseHandler_) {
        return close(*prevCloseHandler_, future);
    }

    if (hasPrevCloseHandler_) {
        ChannelHandlerContext* prev = prev_;

        while (prev) {
            if (prev->closeFunctor_) {
                prevCloseHandler_ = prev;
                return close(*prev, future);
            }

            prev = prev->prev_;
        }

        if (!prev) {
            hasPrevCloseHandler_ = false;
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
                ChannelHandlerContext* prev = ctx.prevCloseHandler_;

                if (prev) {
                    prev->closeFunctor_(*prev, future);
                }
                else {
                    ctx.close(future);
                }
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

const ChannelFuturePtr& ChannelHandlerContext::flush() {
    return flush(newFuture());
}

const ChannelFuturePtr& ChannelHandlerContext::flush(const ChannelFuturePtr& future) {
    if (prevFlushHandler_) {
        return flush(*prevFlushHandler_, future);
    }

    if (hasPrevFlushHandler_) {
        ChannelHandlerContext* prev = prev_;

        while (prev) {
            if (prev->flushFunctor_) {
                prevFlushHandler_ = prev;
                return flush(*prev, future);
            }

            prev = prev->prev_;
        }

        if (!prev) {
            hasPrevFlushHandler_ = false;
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
                ChannelHandlerContext* prev = ctx.prevFlushHandler_;

                if (prev) {
                    prev->flushFunctor_(*prev, future);
                }
                else {
                    ctx.flush(future);
                }
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
    else {
        LOG_ERROR << "the channel in context has been destroied, may not happend.";
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
        LOG_ERROR << "the channel in context has been destroied, may not happend.";
        return NullChannel::instance()->newSucceededFuture();
    }
}

ChannelFuturePtr ChannelHandlerContext::newFailedFuture(const ChannelException& cause) {
    if (!channel_.expired()) {
        return channel_.lock()->newFailedFuture(cause);
    }
    else {
        LOG_ERROR << "the channel in context has been destroied, may not happend.";
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

void ChannelHandlerContext::initialize(ChannelPipeline* pipeline) {
    pipeline_ = pipeline;
    channel_ = pipeline_->channel();

    if (!eventLoop_) {
        eventLoop_ = pipeline_->eventLoop();
    }
}

}
}
