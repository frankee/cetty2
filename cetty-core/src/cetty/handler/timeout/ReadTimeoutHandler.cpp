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

#include <cetty/channel/Timeout.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelHandlerContext.h>

namespace cetty {
namespace handler {
namespace timeout {

using namespace cetty::channel;
using namespace cetty::util;

const ReadTimeoutException ReadTimeoutHandler::EXCEPTION("ReadTimeout");

ReadTimeoutHandler::ReadTimeoutHandler(int timeoutSeconds)
    : state(0), closed(true), timeoutMillis(timeoutSeconds * 1000) {
    if (timeoutSeconds <= 0) {
        timeoutMillis = 0;
    }
}

ReadTimeoutHandler::ReadTimeoutHandler(const Duration& timeout)
    : state(0), closed(true), timeoutMillis(timeout.total_milliseconds()) {
    if (timeoutMillis <= 0) {
        timeoutMillis = 0;
    }
}

void ReadTimeoutHandler::initialize(ChannelHandlerContext& ctx) {
    // Avoid the case where destroy() is called before scheduling timeouts.
    switch (state) {
    case 1:
    case 2:
        return;
    }

    state = 1;

    if (!timeoutCallback) {
        timeoutCallback = boost::bind(
                              &ReadTimeoutHandler::handleReadTimeout,
                              this,
                              boost::ref(ctx));
    }

    lastReadTime = boost::get_system_time();

    if (timeoutMillis > 0) {
        timeout =
            ctx.getEventLoop()->runAfter(timeoutMillis, timeoutCallback);
    }
}

void ReadTimeoutHandler::destroy() {
    state = 2;

    if (timeout) {
        timeout->cancel();
        timeout.reset();
    }
}

void ReadTimeoutHandler::handleReadTimeout(ChannelHandlerContext& ctx) {
    if (timeout->isCancelled()) {
        return;
    }

    if (!ctx.getChannel()->isOpen()) {
        return;
    }

    Time currentTime = boost::get_system_time();
    Duration duration = currentTime - lastReadTime;
    boost::int64_t nextDelay = timeoutMillis - duration.total_milliseconds();

    if (nextDelay <= 0) {
        // Read timed out - set a new timeout and notify the callback.
        this->timeout =
            ctx.getEventLoop()->runAfter(timeoutMillis, timeoutCallback);

        try {
            readTimedOut(ctx);
        }
        catch (const std::exception& t) {
            ctx.fireExceptionCaught(ChannelException(t.what()));
        }
        catch (...) {
            ctx.fireExceptionCaught(ChannelException("Unknow exception."));
        }
    }
    else {
        // Read occurred before the timeout - set a new timeout with shorter delay.
        this->timeout =
            ctx.getEventLoop()->runAfter(nextDelay, timeoutCallback);
    }
}

void ReadTimeoutHandler::readTimedOut(ChannelHandlerContext& ctx) {
    if (!closed) {
        ctx.fireExceptionCaught(EXCEPTION);
        ctx.close();
        closed = true;
    }
}

void ReadTimeoutHandler::beforeAdd(ChannelHandlerContext& ctx) {
    if (ctx.getChannel()->isActive()) {
        // channelActvie() event has been fired already, which means this.channelActive() will
        // not be invoked. We have to initialize here instead.
        initialize(ctx);
    }
    else {
        // channelActive() event has not been fired yet.  this.channelActive() will be invoked
        // and initialization will occur there.
    }
}

ChannelHandlerPtr ReadTimeoutHandler::clone() {
    return ChannelHandlerPtr(
               new ReadTimeoutHandler(
                   boost::posix_time::milliseconds(timeoutMillis)));
}

void ReadTimeoutHandler::afterAdd(ChannelHandlerContext& ctx) {
    // NOOP
}

void ReadTimeoutHandler::beforeRemove(ChannelHandlerContext& ctx) {
    destroy();
}

void ReadTimeoutHandler::afterRemove(ChannelHandlerContext& ctx) {
    // NOOP
}

void ReadTimeoutHandler::channelOpen(ChannelHandlerContext& ctx) {
    // Initialize early if channel is active already.
    if (ctx.getChannel()->isActive()) {
        initialize(ctx);
    }

    AbstractChannelInboundHandler::channelOpen(ctx);
}

void ReadTimeoutHandler::channelActive(ChannelHandlerContext& ctx) {
    // This method will be invoked only if this handler was added
    // before channelActive() event is fired.  If a user adds this handler
    // after the channelActive() event, initialize() will be called by beforeAdd().
    initialize(ctx);
    AbstractChannelInboundHandler::channelActive(ctx);
}

void ReadTimeoutHandler::channelInactive(ChannelHandlerContext& ctx) {
    destroy();
    AbstractChannelInboundHandler::channelInactive(ctx);
}

void ReadTimeoutHandler::messageUpdated(ChannelHandlerContext& ctx) {
    lastReadTime = boost::get_system_time();
    ctx.fireMessageUpdated();
}

std::string ReadTimeoutHandler::toString() const {
    return "ReadTimeoutHandler";
}

}
}
}
