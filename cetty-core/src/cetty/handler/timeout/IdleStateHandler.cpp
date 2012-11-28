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
    : readerIdleTimeMillis(readerIdleTimeSeconds * 1000),
      readerIdleCount(),
      writerIdleTimeMillis(writerIdleTimeSeconds * 1000),
      writerIdleCount(),
      allIdleTimeMillis(allIdleTimeSeconds * 1000),
      allIdleCount(),
      state(0) {
}

IdleStateHandler::IdleStateHandler(
    const IdleStateHandler::Duration& readerIdleTime,
    const IdleStateHandler::Duration& writerIdleTime,
    const IdleStateHandler::Duration& allIdleTime)
    : readerIdleTimeMillis(readerIdleTime.total_milliseconds()),
      readerIdleCount(),
      writerIdleTimeMillis(writerIdleTime.total_milliseconds()),
      writerIdleCount(),
      allIdleTimeMillis(allIdleTime.total_milliseconds()),
      allIdleCount(),
      state(0) {
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

ChannelHandlerPtr IdleStateHandler::clone() {
    return ChannelHandlerPtr(
               new IdleStateHandler(
                   boost::posix_time::milliseconds(readerIdleTimeMillis),
                   boost::posix_time::milliseconds(writerIdleTimeMillis),
                   boost::posix_time::milliseconds(allIdleTimeMillis)));
}

void IdleStateHandler::handleIdle(ChannelHandlerContext& ctx,
                                  const IdleStateEvent& evt) {
    if (idleEventCallback) {
        idleEventCallback(ctx, evt);
    }

    ctx.fireUserEventTriggered(evt);
}

void IdleStateHandler::initialize(ChannelHandlerContext& ctx) {
    // Avoid the case where destroy() is called before scheduling timeouts.
    switch (state) {
    case 1:
    case 2:
        return;
    }

    state = 1;

    lastReadTime = lastWriteTime = boost::get_system_time();

    if (!readerIdleTimeCallback) {
        readerIdleTimeCallback = boost::bind(
                                     &IdleStateHandler::handleReaderIdleTimeout,
                                     this,
                                     boost::ref(ctx));
    }

    if (!writerIdleTimeCallback) {
        writerIdleTimeCallback = boost::bind(
                                     &IdleStateHandler::handleWriterIdleTimeout,
                                     this,
                                     boost::ref(ctx));
    }

    if (!allIdleTimeCallback) {
        allIdleTimeCallback = boost::bind(
                                  &IdleStateHandler::handleAllIdleTimeout,
                                  this,
                                  boost::ref(ctx));
    }

    const EventLoopPtr& eventLoop = ctx.eventLoop();

    if (readerIdleTimeMillis > 0) {
        readerIdleTimeout =
            eventLoop->runAfter(readerIdleTimeMillis, readerIdleTimeCallback);
    }

    if (writerIdleTimeMillis > 0) {
        writerIdleTimeout =
            eventLoop->runAfter(writerIdleTimeMillis, writerIdleTimeCallback);
    }

    if (allIdleTimeMillis > 0) {
        allIdleTimeout =
            eventLoop->runAfter(allIdleTimeMillis, allIdleTimeCallback);
    }
}

void IdleStateHandler::destroy() {
    if (readerIdleTimeout) {
        readerIdleTimeout->cancel();
        readerIdleTimeout.reset();
    }

    if (writerIdleTimeout) {
        writerIdleTimeout->cancel();
        writerIdleTimeout.reset();
    }

    if (allIdleTimeout) {
        allIdleTimeout->cancel();
        allIdleTimeout.reset();
    }
}

void IdleStateHandler::handleReaderIdleTimeout(ChannelHandlerContext& ctx) {
    if (readerIdleTimeout->isCancelled() || !ctx.channel()->isOpen()) {
        return;
    }

    Time currentTime = boost::get_system_time();

    Duration duration = currentTime - lastReadTime;
    boost::int64_t nextDelay = readerIdleTimeMillis - duration.total_milliseconds();

    if (nextDelay <= 0) {
        // Reader is idle - set a new timeout and notify the callback.
        readerIdleTimeout = ctx.eventLoop()->runAfter(readerIdleTimeMillis,
                            readerIdleTimeCallback);

        try {
            handleIdle(ctx,
                       IdleStateEvent(IdleState::READER_IDLE,
                                      readerIdleCount++,
                                      (currentTime - lastReadTime).total_milliseconds()));
        }
        catch (const std::exception& e) {
            ctx.fireExceptionCaught(ChannelException(e.what()));
        }
    }
    else {
        // Read occurred before the timeout - set a new timeout with shorter delay.
        readerIdleTimeout = ctx.eventLoop()->runAfter(nextDelay,
                            readerIdleTimeCallback);
    }
}

void IdleStateHandler::handleWriterIdleTimeout(ChannelHandlerContext& ctx) {
    if (readerIdleTimeout->isCancelled() || !ctx.channel()->isOpen()) {
        return;
    }

    Time currentTime = boost::get_system_time();
    Duration duration = currentTime - lastWriteTime;
    boost::int64_t nextDelay = writerIdleTimeMillis - duration.total_milliseconds();

    if (nextDelay <= 0) {
        // Writer is idle - set a new timeout and notify the callback.
        writerIdleTimeout = ctx.eventLoop()->runAfter(writerIdleTimeMillis,
                            writerIdleTimeCallback);

        try {
            handleIdle(ctx,
                       IdleStateEvent(IdleState::WRITER_IDLE,
                                      writerIdleCount ++,
                                      (currentTime - lastWriteTime).total_milliseconds()));
        }
        catch (const std::exception& e) {
            ctx.fireExceptionCaught(ChannelException(e.what()));
        }
    }
    else {
        // Write occurred before the timeout - set a new timeout with shorter delay.
        writerIdleTimeout = ctx.eventLoop()->runAfter(nextDelay,
                            writerIdleTimeCallback);
    }
}

void IdleStateHandler::handleAllIdleTimeout(ChannelHandlerContext& ctx) {
    if (readerIdleTimeout->isCancelled() || !ctx.channel()->isOpen()) {
        return;
    }

    Time currentTime = boost::get_system_time();
    Time lastIoTime = std::max(lastWriteTime, lastReadTime);

    Duration duration = currentTime - lastIoTime;
    boost::int64_t nextDelay = allIdleTimeMillis - duration.total_milliseconds();

    if (nextDelay <= 0) {
        // Both reader and writer are idle - set a new timeout and
        // notify the callback.
        allIdleTimeout =  ctx.eventLoop()->runAfter(allIdleTimeMillis,
                          allIdleTimeCallback);

        try {
            handleIdle(ctx,
                       IdleStateEvent(IdleState::ALL_IDLE,
                                      allIdleCount ++,
                                      (currentTime - lastIoTime).total_milliseconds()));
        }
        catch (const std::exception& e) {
            ctx.fireExceptionCaught(ChannelException(e.what()));
        }
    }
    else {
        // Either read or write occurred before the timeout - set a new
        // timeout with shorter delay.
        allIdleTimeout =  ctx.eventLoop()->runAfter(nextDelay,
                          allIdleTimeCallback);
    }
}

void IdleStateHandler::handleWriteCompleted(const ChannelFuturePtr& future) {
    lastWriteTime = boost::get_system_time();
    writerIdleCount = allIdleCount = 0;
}

std::string IdleStateHandler::toString() const {
    return "IdleStateHandler";
}

void IdleStateHandler::afterAdd(ChannelHandlerContext& ctx) {

}

void IdleStateHandler::afterRemove(ChannelHandlerContext& ctx) {

}

void IdleStateHandler::channelActive(ChannelHandlerContext& ctx) {
    // This method will be invoked only if this handler was added
    // before channelActive() event is fired.  If a user adds this handler
    // after the channelActive() event, initialize() will be called by beforeAdd().
    initialize(ctx);
    AbstractChannelHandler::channelActive(ctx);
}

void IdleStateHandler::channelInactive(ChannelHandlerContext& ctx) {
    destroy();
    AbstractChannelHandler::channelInactive(ctx);
}

void IdleStateHandler::messageUpdated(ChannelHandlerContext& ctx) {
    lastReadTime = boost::get_system_time();
    readerIdleCount = allIdleCount = 0;
    ctx.fireMessageUpdated();
}

void IdleStateHandler::flush(ChannelHandlerContext& ctx,
                             const ChannelFuturePtr& future) {
    future->addListener(
        boost::bind(&IdleStateHandler::handleWriteCompleted,
                    this,
                    boost::cref(future)));

    AbstractChannelHandler::flush(ctx, future);
}

}
}
}
