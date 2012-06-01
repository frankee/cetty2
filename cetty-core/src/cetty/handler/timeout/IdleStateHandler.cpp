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

#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/WriteCompletionEvent.h>

#include <cetty/handler/timeout/DefaultIdleStateEvent.h>

#include <cetty/util/TimeUnit.h>
#include <cetty/util/Timeout.h>
#include <cetty/util/Exception.h>
#include <cetty/util/TimerFactory.h>

namespace cetty {
namespace handler {
namespace timeout {

using namespace cetty::channel;
using namespace cetty::util;

IdleStateHandler::IdleStateHandler(int readerIdleTimeSeconds,
                                   int writerIdleTimeSeconds,
                                   int allIdleTimeSeconds)
    : readerIdleTimeMillis(TimeUnit::SECONDS.toMillis(readerIdleTimeSeconds)),
      writerIdleTimeMillis(TimeUnit::SECONDS.toMillis(writerIdleTimeSeconds)),
      allIdleTimeMillis(TimeUnit::SECONDS.toMillis(allIdleTimeSeconds)) {
}

IdleStateHandler::IdleStateHandler(boost::int64_t readerIdleTime,
                                   boost::int64_t writerIdleTime,
                                   boost::int64_t allIdleTime,
                                   const TimeUnit& unit) {
    if (readerIdleTime <= 0) {
        readerIdleTimeMillis = 0;
    }
    else {
        readerIdleTimeMillis = unit.toMillis(readerIdleTime);

        if (!readerIdleTimeMillis) { readerIdleTimeMillis = 1; }
    }

    if (writerIdleTime <= 0) {
        writerIdleTimeMillis = 0;
    }
    else {
        writerIdleTimeMillis = unit.toMillis(writerIdleTime);

        if (!writerIdleTimeMillis) { writerIdleTimeMillis = 1; }
    }

    if (allIdleTime <= 0) {
        allIdleTimeMillis = 0;
    }
    else {
        allIdleTimeMillis = unit.toMillis(allIdleTime);

        if (!allIdleTime) { allIdleTime = 1; }
    }
}

void IdleStateHandler::releaseExternalResources() {
    if (timer) {
        timer->stop();
    }
}

void IdleStateHandler::beforeAdd(ChannelHandlerContext& ctx) {
    if (ctx.getPipeline()->isAttached()) {
        // channelOpen event has been fired already, which means
        // this.channelOpen() will not be invoked.
        // We have to initialize here instead.
        initialize(ctx);
    }
    else {
        // channelOpen event has not been fired yet.
        // this.channelOpen() will be invoked and initialization will occur there.
    }
}

void IdleStateHandler::beforeRemove(ChannelHandlerContext& ctx) {
    destroy();
}

void IdleStateHandler::channelOpen(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    // This method will be invoked only if this handler was added
    // before channelOpen event is fired.  If a user adds this handler
    // after the channelOpen event, initialize() will be called by beforeAdd().
    initialize(ctx);
    ctx.sendUpstream(e);
}

void IdleStateHandler::channelClosed(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    destroy();
    ctx.sendUpstream(e);
}

void IdleStateHandler::messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
    lastReadTime = boost::get_system_time();
    ctx.sendUpstream(e);
}

void IdleStateHandler::writeCompleted(ChannelHandlerContext& ctx, const WriteCompletionEvent& e) {
    if (e.getWrittenAmount() > 0) {
        lastWriteTime = boost::get_system_time();
    }

    ctx.sendUpstream(e);
}

ChannelHandlerPtr IdleStateHandler::clone() {
    return ChannelHandlerPtr(
               new IdleStateHandler(readerIdleTimeMillis,
                                    writerIdleTimeMillis,
                                    allIdleTimeMillis,
                                    TimeUnit::MILLISECONDS));
}

void IdleStateHandler::channelIdle(ChannelHandlerContext& ctx,
                                   const IdleState& state,
                                   const time_type& lastActivityTime) {
    ctx.sendUpstream(
        DefaultIdleStateEvent(ctx.getChannel(), state, lastActivityTime));
}

void IdleStateHandler::initialize(ChannelHandlerContext& ctx) {
    lastReadTime = lastWriteTime = boost::get_system_time();

    if (!timer) {
        timer = TimerFactory::getFactory().getTimer(ctx.getChannel());
    }

    if (!readerTimeoutTask) {
        readerTimeoutTask = new ReaderIdleTimeoutTask(ctx, *this);
    }

    if (!writerTimeoutTask) {
        writerTimeoutTask = new WriterIdleTimeoutTask(ctx, *this);
    }

    if (!allTimeoutTask) {
        allTimeoutTask = new AllIdleTimeoutTask(ctx, *this);
    }

    if (readerIdleTimeMillis > 0) {
        readerIdleTimeout =
            timer->newTimeout(*readerTimeoutTask, readerIdleTimeMillis);
    }

    if (writerIdleTimeMillis > 0) {
        writerIdleTimeout =
            timer->newTimeout(*writerTimeoutTask, writerIdleTimeMillis);
    }

    if (allIdleTimeMillis > 0) {
        allIdleTimeout =
            timer->newTimeout(*allTimeoutTask, allIdleTimeMillis);
    }
}

void IdleStateHandler::destroy() {
    if (timer) {
        timer = NULL;
    }

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

    if (readerTimeoutTask) {
        delete readerTimeoutTask;
        readerTimeoutTask = NULL;
    }

    if (writerTimeoutTask) {
        delete writerTimeoutTask;
        writerTimeoutTask = NULL;
    }

    if (allTimeoutTask) {
        delete allTimeoutTask;
        allTimeoutTask = NULL;
    }
}

void IdleStateHandler::ReaderIdleTimeoutTask::run(Timeout& timeout) {
    if (timeout.isCancelled() || !ctx.getChannel()->isOpen()) {
        return;
    }

    time_type currentTime = boost::get_system_time();
    time_type lastReadTime = handler.lastReadTime;

    time_duration_type duration = currentTime - lastReadTime;
    boost::int64_t nextDelay = handler.readerIdleTimeMillis - duration.total_milliseconds();

    if (nextDelay <= 0) {
        // Reader is idle - set a new timeout and notify the callback.
        handler.readerIdleTimeout =
            handler.timer->newTimeout(*this, handler.readerIdleTimeMillis);

        try {
            handler.channelIdle(ctx, IdleState::READER_IDLE, lastReadTime);
        }
        catch (const Exception& t) {
            Channels::fireExceptionCaught(ctx, t);
        }
        catch (const std::exception& t) {
            Channels::fireExceptionCaught(ctx, Exception(t.what()));
        }
        catch (...) {
            Channels::fireExceptionCaught(ctx, Exception("Unknow Exception"));
        }
    }
    else {
        // Read occurred before the timeout - set a new timeout with shorter delay.
        handler.readerIdleTimeout = handler.timer->newTimeout(*this, nextDelay);
    }
}


void IdleStateHandler::WriterIdleTimeoutTask::run(Timeout& timeout) {
    if (timeout.isCancelled() || !ctx.getChannel()->isOpen()) {
        return;
    }

    time_type currentTime = boost::get_system_time();
    time_type lastWriteTime = handler.lastWriteTime;

    time_duration_type duration = currentTime - lastWriteTime;
    boost::int64_t nextDelay = handler.writerIdleTimeMillis - duration.total_milliseconds();

    if (nextDelay <= 0) {
        // Writer is idle - set a new timeout and notify the callback.
        handler.writerIdleTimeout =
            handler.timer->newTimeout(*this, handler.writerIdleTimeMillis);

        try {
            handler.channelIdle(ctx, IdleState::WRITER_IDLE, lastWriteTime);
        }
        catch (const Exception& t) {
            Channels::fireExceptionCaught(ctx, t);
        }
        catch (const std::exception& t) {
            Channels::fireExceptionCaught(ctx, Exception(t.what()));
        }
        catch (...) {
            Channels::fireExceptionCaught(ctx, Exception("Unknow Exception"));
        }
    }
    else {
        // Write occurred before the timeout - set a new timeout with shorter delay.
        handler.writerIdleTimeout = handler.timer->newTimeout(*this, nextDelay);
    }
}

void IdleStateHandler::AllIdleTimeoutTask::run(Timeout& timeout) {
    if (timeout.isCancelled() || !ctx.getChannel()->isOpen()) {
        return;
    }

    time_type currentTime = boost::get_system_time();
    time_type lastIoTime = std::max(handler.lastWriteTime, handler.lastReadTime);

    time_duration_type duration = currentTime - lastIoTime;
    boost::int64_t nextDelay = handler.allIdleTimeMillis - duration.total_milliseconds();

    if (nextDelay <= 0) {
        // Both reader and writer are idle - set a new timeout and
        // notify the callback.
        handler.allIdleTimeout =
            handler.timer->newTimeout(*this, handler.allIdleTimeMillis);

        try {
            handler.channelIdle(ctx, IdleState::ALL_IDLE, lastIoTime);
        }
        catch (const Exception& t) {
            Channels::fireExceptionCaught(ctx, t);
        }
        catch (const std::exception& t) {
            Channels::fireExceptionCaught(ctx, Exception(t.what()));
        }
        catch (...) {
            Channels::fireExceptionCaught(ctx, Exception("Unknow Exception"));
        }
    }
    else {
        // Either read or write occurred before the timeout - set a new
        // timeout with shorter delay.
        handler.allIdleTimeout = handler.timer->newTimeout(*this, nextDelay);
    }
}

}
}
}