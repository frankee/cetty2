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

#include <boost/bind.hpp>
#include <boost/thread/thread_time.hpp>

#include <cetty/channel/Timeout.h>
#include <cetty/channel/Channel.h>

namespace cetty {
namespace handler {
namespace timeout {

using namespace cetty::channel;
using namespace cetty::util;

const ReadTimeoutException ReadTimeoutHandler::EXCEPTION("ReadTimeout");

ReadTimeoutHandler::ReadTimeoutHandler(int timeoutSeconds)
    : closed_(true),
state_(0),
    timeoutMillis_(timeoutSeconds * 1000) {
    if (timeoutSeconds <= 0) {
        timeoutMillis_ = 0;
    }
}

ReadTimeoutHandler::ReadTimeoutHandler(const Duration& timeout)
    : closed_(true),
state_(0),
    timeoutMillis_(timeout.total_milliseconds()) {
    if (timeoutMillis_ <= 0) {
        timeoutMillis_ = 0;
    }
}

void ReadTimeoutHandler::registerTo(Context& ctx) {
    ctx.setBeforeAddCallback(boost::bind(&ReadTimeoutHandler::beforeAdd,
        this,
        _1));
    ctx.setBeforeRemoveCallback(boost::bind(&ReadTimeoutHandler::beforeRemove,
        this,
        _1));
    ctx.setChannelActiveCallback(boost::bind(&ReadTimeoutHandler::channelActive,
        this,
        _1));
    ctx.setChannelInactiveCallback(boost::bind(&ReadTimeoutHandler::channelInactive,
        this,
        _1));
    ctx.setChannelMessageUpdatedCallback(boost::bind(&ReadTimeoutHandler::messageUpdated,
        this,
        _1));
}

void ReadTimeoutHandler::initialize(ChannelHandlerContext& ctx) {
    // Avoid the case where destroy() is called before scheduling timeouts.
    switch (state_) {
    case 1:
    case 2:
        return;
    }

    state_ = 1;

    if (!timerCallback_) {
        timerCallback_ = boost::bind(
                              &ReadTimeoutHandler::handleReadTimeout,
                              this,
                              boost::ref(ctx));
    }

    lastReadTime_ = boost::get_system_time();

    if (timeoutMillis_ > 0) {
        timeout_ =
            ctx.eventLoop()->runAfter(timeoutMillis_, timerCallback_);
    }
}

void ReadTimeoutHandler::destroy() {
    state_ = 2;

    if (timeout_) {
        timeout_->cancel();
        timeout_.reset();
    }
}

void ReadTimeoutHandler::handleReadTimeout(ChannelHandlerContext& ctx) {
    if (timeout_->isCancelled()) {
        return;
    }

    if (!ctx.channel()->isOpen()) {
        return;
    }

    Time currentTime = boost::get_system_time();
    Duration duration = currentTime - lastReadTime_;
    boost::int64_t nextDelay = timeoutMillis_ - duration.total_milliseconds();

    if (nextDelay <= 0) {
        // Read timed out - set a new timeout and notify the callback.
        this->timeout_ =
            ctx.eventLoop()->runAfter(timeoutMillis_, timerCallback_);

        try {
            readTimedOut(ctx);
        }
        catch (const std::exception& t) {
            ctx.fireExceptionCaught(ChannelException(t.what()));
        }
        catch (...) {
            ctx.fireExceptionCaught(ChannelException("Unknown exception."));
        }
    }
    else {
        // Read occurred before the timeout - set a new timeout with shorter delay.
        this->timeout_ =
            ctx.eventLoop()->runAfter(nextDelay, timerCallback_);
    }
}

void ReadTimeoutHandler::readTimedOut(ChannelHandlerContext& ctx) {
    if (!closed_) {
        ctx.fireExceptionCaught(EXCEPTION);
        ctx.close();
        closed_ = true;
    }
}

void ReadTimeoutHandler::beforeAdd(ChannelHandlerContext& ctx) {
    if (ctx.channel()->isActive()) {
        // channelActvie() event has been fired already, which means this.channelActive() will
        // not be invoked. We have to initialize here instead.
        initialize(ctx);
    }
    else {
        // channelActive() event has not been fired yet.  this.channelActive() will be invoked
        // and initialization will occur there.
    }
}

void ReadTimeoutHandler::beforeRemove(ChannelHandlerContext& ctx) {
    destroy();
}

void ReadTimeoutHandler::channelActive(ChannelHandlerContext& ctx) {
    // This method will be invoked only if this handler was added
    // before channelActive() event is fired.  If a user adds this handler
    // after the channelActive() event, initialize() will be called by beforeAdd().
    initialize(ctx);

    ctx.fireChannelActive();
}

void ReadTimeoutHandler::channelInactive(ChannelHandlerContext& ctx) {
    destroy();

    ctx.fireChannelInactive();
}

void ReadTimeoutHandler::messageUpdated(ChannelHandlerContext& ctx) {
    lastReadTime_ = boost::get_system_time();

    ctx.fireMessageUpdated();
}

}
}
}
