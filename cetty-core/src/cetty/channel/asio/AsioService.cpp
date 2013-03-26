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

#include <cetty/channel/asio/AsioService.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace cetty::channel;

AsioService::AsioService(const EventLoopPoolPtr& pool)
    : EventLoop(pool),
      timerId_(0) {
}

void AsioService::stop() {
    EventLoop::stop();
    ioService_.stop();
}

void AsioService::post(const Handler& handler) {
    ioService_.post(handler);
}

TimeoutPtr AsioService::runAt(const boost::posix_time::ptime& timestamp,
                              const Handler& handler) {
    if (handler) {
        AsioDeadlineTimeoutPtr timeout
            = new AsioDeadlineTimeout(*this, timerId_++, timestamp);

        timeout->timer().async_wait(boost::bind(
                                        &AsioService::timerExpiresCallback,
                                        this,
                                        boost::asio::placeholders::error,
                                        handler,
                                        timeout));

        timeout->setState(AsioDeadlineTimeout::TIMER_ACTIVE);
        timers_.push_back(timeout);

        LOG_INFO << "Active the timer "
                 << timeout->threadId() << ":" << timeout->id()
                 << " , and expire at " << timestamp;

        return boost::static_pointer_cast<Timeout>(timeout);
    }
    else {
        LOG_WARN << "Timer handler is empty, do nothing.";
        return TimeoutPtr();
    }
}

TimeoutPtr AsioService::runAfter(int64_t millisecond,
                                 const Handler& handler) {
    if (handler) {
        AsioDeadlineTimeoutPtr timeout
            = new AsioDeadlineTimeout(*this, timerId_++, millisecond);

        timeout->timer().async_wait(boost::bind(
                                        &AsioService::timerExpiresCallback,
                                        this,
                                        boost::asio::placeholders::error,
                                        handler,
                                        timeout));

        timeout->setState(AsioDeadlineTimeout::TIMER_ACTIVE);
        timers_.push_back(timeout);

        LOG_INFO << "Active the timer "
                 << timeout->threadId() << ":" << timeout->id()
                 << " , and expire after " << millisecond << " ms";

        return boost::static_pointer_cast<Timeout>(timeout);
    }
    else {
        LOG_WARN << "Timer handler is empty, do nothing.";
        return TimeoutPtr();
    }
}

TimeoutPtr AsioService::runEvery(int64_t millisecond,
                                 const Handler& handler) {
    if (handler) {
        AsioDeadlineTimeoutPtr timeout
            = new AsioDeadlineTimeout(*this, timerId_++, millisecond);

        timeout->timer().async_wait(boost::bind(
                                        &AsioService::repeatTimerExpiresCallback,
                                        this,
                                        boost::asio::placeholders::error,
                                        handler,
                                        millisecond,
                                        timeout));
        timeout->setState(AsioDeadlineTimeout::TIMER_ACTIVE);
        timers_.push_back(timeout);

        LOG_INFO << "Active the repeated timer "
                 << timeout->threadId() << ":" << timeout->id()
                 << " , run every " << millisecond << " ms";

        return boost::static_pointer_cast<Timeout>(timeout);
    }
    else {
        LOG_WARN << "The repeated timer handler is empty, do nothing.";
        return TimeoutPtr();
    }
}

void AsioService::timerExpiresCallback(const boost::system::error_code& code,
                                       const Handler& handler,
                                       const AsioDeadlineTimeoutPtr& timeout) {
    if (code != boost::asio::error::operation_aborted) {
        timeout->setState(AsioDeadlineTimeout::TIMER_EXPIRED);

        LOG_INFO << "The timer "
                 << timeout->threadId() << ":" << timeout->id()
                 << " expired, and do handler";

        if (handler) {
            handler();
        }
    }
    else {
        LOG_INFO << "The timer "
                 << timeout->threadId() << ":" << timeout->id()
                 << " canceled";

        timeout->setState(AsioDeadlineTimeout::TIMER_CANCELLED);
    }

    timers_.remove(timeout);
}

void AsioService::repeatTimerExpiresCallback(const boost::system::error_code& code,
        const Handler& handler,
        int64_t millisecond,
        const AsioDeadlineTimeoutPtr& timeout) {
    if (code != boost::asio::error::operation_aborted) {
        timeout->setState(AsioDeadlineTimeout::TIMER_EXPIRED);

        LOG_INFO << "The repeated timer "
                 << timeout->threadId() << ":" << timeout->id()
                 << " expired once, and do handler";

        if (handler) {
            handler();
        }

        timers_.remove(timeout);

        //FIXME
        AsioDeadlineTimeoutPtr newTimeout
            = new AsioDeadlineTimeout(*this, timeout->id(), millisecond);

        boost::system::error_code code;
        boost::asio::deadline_timer& timer = newTimeout->timer();
        timer.expires_from_now(
            boost::posix_time::milliseconds(millisecond),
            code);

        if (!code) {
            timer.async_wait(boost::bind(&AsioService::repeatTimerExpiresCallback,
                                         this,
                                         boost::asio::placeholders::error,
                                         handler,
                                         millisecond,
                                         newTimeout));
            newTimeout->setState(AsioDeadlineTimeout::TIMER_ACTIVE);
            timers_.push_back(newTimeout);

            LOG_INFO << "Active the repeated timer "
                     << timeout->threadId() << ":" << timeout->id()
                     << " again, and expire after " << millisecond << " ms";
        }
        else {
            LOG_ERROR << "active the repeated timer "
                      << timeout->threadId() << ":" << timeout->id()
                      << " error, code :" << code.value()
                      << ", message: " << code.message();
        }
    }
    else {
        LOG_INFO << "The repeated timer "
                 << timeout->threadId() << ":" << timeout->id()
                 << " canceled";

        timeout->setState(AsioDeadlineTimeout::TIMER_CANCELLED);
        timers_.remove(timeout);
    }
}

}
}
}
