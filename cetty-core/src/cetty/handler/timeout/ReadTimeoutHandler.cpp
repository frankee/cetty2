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

#include <cetty/handler/timeout/ReadTimeoutHandler.h>

#include <boost/thread/thread_time.hpp>
#include <boost/bind.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/util/TimeUnit.h>
#include <cetty/util/Timeout.h>
#include <cetty/util/TimerFactory.h>

namespace cetty {
namespace handler {
namespace timeout {

using namespace cetty::channel;
using namespace cetty::util;

const ReadTimeoutException ReadTimeoutHandler::EXCEPTION("ReadTimeout");

ReadTimeoutHandler::ReadTimeoutHandler(int timeoutSeconds) {
    if (timeoutSeconds <= 0) {
        timeoutMillis = 0;
    }
    else {
        timeoutMillis = TimeUnit::SECONDS.toMillis(timeoutSeconds);
    }
}

ReadTimeoutHandler::ReadTimeoutHandler(boost::int64_t timeout,
                                       const TimeUnit& unit) {
    if (timeout <= 0) {
        timeoutMillis = 0;
    }
    else {
        timeoutMillis = unit.toMillis(timeout);

        if (0 == timeoutMillis) { timeoutMillis = 1; }
    }
}

void ReadTimeoutHandler::releaseExternalResources() {
    if (timer) {
        timer->stop();
    }
}

void ReadTimeoutHandler::beforeAdd(ChannelHandlerContext& ctx) {
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

void ReadTimeoutHandler::beforeRemove(ChannelHandlerContext& ctx) {
    destroy();
}

void ReadTimeoutHandler::channelOpen(ChannelHandlerContext& ctx,
                                     const ChannelStateEvent& e) {
    // This method will be invoked only if this handler was added
    // before channelOpen event is fired.  If a user adds this handler
    // after the channelOpen event, initialize() will be called by beforeAdd().
    initialize(ctx);
    ctx.sendUpstream(e);
}

void ReadTimeoutHandler::channelClosed(ChannelHandlerContext& ctx,
                                       const ChannelStateEvent& e) {
    destroy();
    ctx.sendUpstream(e);
}

void ReadTimeoutHandler::messageReceived(ChannelHandlerContext& ctx,
        const MessageEvent& e) {
    updateLastReadTime();
    ctx.sendUpstream(e);
}

cetty::channel::ChannelHandlerPtr ReadTimeoutHandler::clone() {
    return ChannelHandlerPtr(
               new ReadTimeoutHandler(timeoutMillis, TimeUnit::MILLISECONDS));
}

void ReadTimeoutHandler::handleReadTimeout(Timeout& timeout, ChannelHandlerContext& ctx) {
    if (timeout.isCancelled()) {
        return;
    }

    if (!ctx.getChannel()->isOpen()) {
        return;
    }

    time_type currentTime = boost::get_system_time();
    time_duration_type duration = currentTime - lastReadTime;
    boost::int64_t nextDelay = timeoutMillis - duration.total_milliseconds();

    if (nextDelay <= 0) {
        // Read timed out - set a new timeout and notify the callback.
        this->timeout =
            timer->newTimeout(boost::bind(&ReadTimeoutHandler::handleReadTimeout,
                                          this,
                                          _1,
                                          boost::ref(ctx)),
                              timeoutMillis);

        try {
            readTimedOut(ctx);
        }
        catch (const Exception& t) {
            Channels::fireExceptionCaught(ctx, t);
        }
        catch (const std::exception& t) {
            Channels::fireExceptionCaught(ctx, Exception(t.what()));
        }
        catch (...) {
            Channels::fireExceptionCaught(ctx, Exception("Unknow exception."));
        }
    }
    else {
        // Read occurred before the timeout - set a new timeout with shorter delay.
        this->timeout =
            timer->newTimeout(boost::bind(&ReadTimeoutHandler::handleReadTimeout,
                                          this,
                                          _1,
                                          boost::ref(ctx)), nextDelay);
    }
}

void ReadTimeoutHandler::readTimedOut(ChannelHandlerContext& ctx) {
    Channels::fireExceptionCaught(ctx, EXCEPTION);
}

void ReadTimeoutHandler::initialize(ChannelHandlerContext& ctx) {
    if (timeoutMillis == 0) { return; }

    updateLastReadTime();

    if (!timer) {
        timer = TimerFactory::getFactory().getTimer(ctx.getChannel());
    }

    timeout = timer->newTimeout(boost::bind(&ReadTimeoutHandler::handleReadTimeout,
                                            this,
                                            _1,
                                            boost::ref(ctx)),
                                timeoutMillis);
}

void ReadTimeoutHandler::updateLastReadTime() {
    lastReadTime = boost::get_system_time();
}

void ReadTimeoutHandler::destroy() {
    if (this->timeout) {
        this->timeout->cancel();
        this->timeout.reset();
    }
}

}
}
}