/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/handler/timeout/IdleStateHandler.h>

#include <boost/thread/thread_time.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/Timeout.h>
#include <cetty/channel/EventLoop.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelHandlerContext.h>

#include <cetty/handler/timeout/IdleStateEvent.h>

#include <cetty/util/Exception.h>

namespace cetty {
namespace handler {
namespace timeout {

using namespace cetty::channel;
using namespace cetty::util;

IdleStateHandler::IdleStateHandler(int readerIdleTimeSeconds,
                                   int writerIdleTimeSeconds,
                                   int allIdleTimeSeconds)
    : readerIdleTimeMillis_(readerIdleTimeSeconds * 1000),
      readerIdleCount_(),
      writerIdleTimeMillis_(writerIdleTimeSeconds * 1000),
      writerIdleCount_(),
      allIdleTimeMillis_(allIdleTimeSeconds * 1000),
      allIdleCount_(),
      state_(0) {
}

IdleStateHandler::IdleStateHandler(
    const IdleStateHandler::Duration& readerIdleTime,
    const IdleStateHandler::Duration& writerIdleTime,
    const IdleStateHandler::Duration& allIdleTime)
    : readerIdleTimeMillis_(readerIdleTime.total_milliseconds()),
      readerIdleCount_(),
      writerIdleTimeMillis_(writerIdleTime.total_milliseconds()),
      writerIdleCount_(),
      allIdleTimeMillis_(allIdleTime.total_milliseconds()),
      allIdleCount_(),
      state_(0) {
}

void IdleStateHandler::beforeAdd(ChannelHandlerContext& ctx) {
    if (ctx.pipeline().attached() && ctx.channel()->isActive()) {
        // channelActvie() event has been fired already, which means this.channelActive() will
        // not be invoked. We have to initialize here instead.
        initialize(ctx);
    }
    else {
        // channelActive() event has not been fired yet.  this.channelActive() will be invoked
        // and initialization will occur there.
    }
}

void IdleStateHandler::beforeRemove(ChannelHandlerContext& ctx) {
    destroy();
}

void IdleStateHandler::handleIdle(ChannelHandlerContext& ctx,
                                  const IdleStateEvent& evt) {
    if (idleEventCallback_) {
        idleEventCallback_(ctx, evt);
    }

    ctx.fireUserEventTriggered(evt);
}

void IdleStateHandler::initialize(ChannelHandlerContext& ctx) {
    // Avoid the case where destroy() is called before scheduling timeouts.
    switch (state_) {
    case 1:
    case 2:
        return;
    }

    state_ = 1;

    lastReadTime_ = lastWriteTime_ = boost::get_system_time();

    if (!readerIdleTimerCallback_) {
        readerIdleTimerCallback_ = boost::bind(
                                     &IdleStateHandler::handleReaderIdleTimeout,
                                     this,
                                     boost::ref(ctx));
    }

    if (!writerIdleTimerCallback_) {
        writerIdleTimerCallback_ = boost::bind(
                                     &IdleStateHandler::handleWriterIdleTimeout,
                                     this,
                                     boost::ref(ctx));
    }

    if (!allIdleTimeCallback_) {
        allIdleTimeCallback_ = boost::bind(
                                  &IdleStateHandler::handleAllIdleTimeout,
                                  this,
                                  boost::ref(ctx));
    }

    const EventLoopPtr& eventLoop = ctx.eventLoop();

    if (readerIdleTimeMillis_ > 0) {
        readerIdleTimeout_ =
            eventLoop->runAfter(readerIdleTimeMillis_, readerIdleTimerCallback_);
    }

    if (writerIdleTimeMillis_ > 0) {
        writerIdleTimeout_ =
            eventLoop->runAfter(writerIdleTimeMillis_, writerIdleTimerCallback_);
    }

    if (allIdleTimeMillis_ > 0) {
        allIdleTimeout_ =
            eventLoop->runAfter(allIdleTimeMillis_, allIdleTimeCallback_);
    }
}

void IdleStateHandler::destroy() {
    if (readerIdleTimeout_) {
        readerIdleTimeout_->cancel();
        readerIdleTimeout_.reset();
    }

    if (writerIdleTimeout_) {
        writerIdleTimeout_->cancel();
        writerIdleTimeout_.reset();
    }

    if (allIdleTimeout_) {
        allIdleTimeout_->cancel();
        allIdleTimeout_.reset();
    }
}

void IdleStateHandler::handleReaderIdleTimeout(ChannelHandlerContext& ctx) {
    if (readerIdleTimeout_->isCancelled() || !ctx.channel()->isOpen()) {
        return;
    }

    Time currentTime = boost::get_system_time();

    Duration duration = currentTime - lastReadTime_;
    boost::int64_t nextDelay = readerIdleTimeMillis_ - duration.total_milliseconds();

    if (nextDelay <= 0) {
        // Reader is idle - set a new timeout and notify the callback.
        readerIdleTimeout_ = ctx.eventLoop()->runAfter(readerIdleTimeMillis_,
                            readerIdleTimerCallback_);

        try {
            handleIdle(ctx,
                       IdleStateEvent(IdleState::READER_IDLE,
                                      readerIdleCount_++,
                                      (currentTime - lastReadTime_).total_milliseconds()));
        }
        catch (const std::exception& e) {
            ctx.fireExceptionCaught(ChannelException(e.what()));
        }
    }
    else {
        // Read occurred before the timeout - set a new timeout with shorter delay.
        readerIdleTimeout_ = ctx.eventLoop()->runAfter(nextDelay,
                            readerIdleTimerCallback_);
    }
}

void IdleStateHandler::handleWriterIdleTimeout(ChannelHandlerContext& ctx) {
    if (readerIdleTimeout_->isCancelled() || !ctx.channel()->isOpen()) {
        return;
    }

    Time currentTime = boost::get_system_time();
    Duration duration = currentTime - lastWriteTime_;
    boost::int64_t nextDelay = writerIdleTimeMillis_ - duration.total_milliseconds();

    if (nextDelay <= 0) {
        // Writer is idle - set a new timeout and notify the callback.
        writerIdleTimeout_ = ctx.eventLoop()->runAfter(writerIdleTimeMillis_,
                            writerIdleTimerCallback_);

        try {
            handleIdle(ctx,
                       IdleStateEvent(IdleState::WRITER_IDLE,
                                      writerIdleCount_ ++,
                                      (currentTime - lastWriteTime_).total_milliseconds()));
        }
        catch (const std::exception& e) {
            ctx.fireExceptionCaught(ChannelException(e.what()));
        }
    }
    else {
        // Write occurred before the timeout - set a new timeout with shorter delay.
        writerIdleTimeout_ = ctx.eventLoop()->runAfter(nextDelay,
                            writerIdleTimerCallback_);
    }
}

void IdleStateHandler::handleAllIdleTimeout(ChannelHandlerContext& ctx) {
    if (readerIdleTimeout_->isCancelled() || !ctx.channel()->isOpen()) {
        return;
    }

    Time currentTime = boost::get_system_time();
    Time lastIoTime = std::max(lastWriteTime_, lastReadTime_);

    Duration duration = currentTime - lastIoTime;
    boost::int64_t nextDelay = allIdleTimeMillis_ - duration.total_milliseconds();

    if (nextDelay <= 0) {
        // Both reader and writer are idle - set a new timeout and
        // notify the callback.
        allIdleTimeout_ =  ctx.eventLoop()->runAfter(allIdleTimeMillis_,
                          allIdleTimeCallback_);

        try {
            handleIdle(ctx,
                       IdleStateEvent(IdleState::ALL_IDLE,
                                      allIdleCount_ ++,
                                      (currentTime - lastIoTime).total_milliseconds()));
        }
        catch (const std::exception& e) {
            ctx.fireExceptionCaught(ChannelException(e.what()));
        }
    }
    else {
        // Either read or write occurred before the timeout - set a new
        // timeout with shorter delay.
        allIdleTimeout_ =  ctx.eventLoop()->runAfter(nextDelay,
                          allIdleTimeCallback_);
    }
}

void IdleStateHandler::handleWriteCompleted(const ChannelFuturePtr& future) {
    lastWriteTime_ = boost::get_system_time();
    writerIdleCount_ = allIdleCount_ = 0;
}

void IdleStateHandler::channelActive(ChannelHandlerContext& ctx) {
    // This method will be invoked only if this handler was added
    // before channelActive() event is fired.  If a user adds this handler
    // after the channelActive() event, initialize() will be called by beforeAdd().
    initialize(ctx);

    ctx.fireChannelActive();
}

void IdleStateHandler::channelInactive(ChannelHandlerContext& ctx) {
    destroy();

    ctx.fireChannelInactive();
}

void IdleStateHandler::messageUpdated(ChannelHandlerContext& ctx) {
    lastReadTime_ = boost::get_system_time();
    readerIdleCount_ = allIdleCount_ = 0;

    ctx.fireMessageUpdated();
}

void IdleStateHandler::flush(ChannelHandlerContext& ctx,
                             const ChannelFuturePtr& future) {
    future->addListener(
        boost::bind(&IdleStateHandler::handleWriteCompleted,
                    this,
                    boost::cref(future)));

    ctx.flush(future);
}

}
}
}
