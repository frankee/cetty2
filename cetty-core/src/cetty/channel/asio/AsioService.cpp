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

namespace cetty {
namespace channel {
namespace asio {

using namespace cetty::channel;

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
            = new AsioDeadlineTimeout(ioService_, timestamp);

        timeout->timer().async_wait(boost::bind(
                                           &AsioService::timerExpiresCallback,
                                           this,
                                           boost::asio::placeholders::error,
                                           handler,
                                           boost::cref(timeout)));

        timeout->setState(AsioDeadlineTimeout::TIMER_ACTIVE);
        timers_.push_back(timeout);
        return boost::static_pointer_cast<Timeout>(timeout);
    }
    else {
        return TimeoutPtr();
    }
}

TimeoutPtr AsioService::runAfter(int64_t millisecond,
                                 const Handler& handler) {
    if (handler) {
        AsioDeadlineTimeoutPtr timeout
            = new AsioDeadlineTimeout(ioService_, millisecond);

        timeout->timer().async_wait(boost::bind(
                                           &AsioService::timerExpiresCallback,
                                           this,
                                           boost::asio::placeholders::error,
                                           handler,
                                           boost::cref(timeout)));

        timeout->setState(AsioDeadlineTimeout::TIMER_ACTIVE);
        timers_.push_back(timeout);
        return boost::static_pointer_cast<Timeout>(timeout);
    }
    else {
        return TimeoutPtr();
    }
}

TimeoutPtr AsioService::runEvery(int64_t millisecond,
                                 const Handler& handler) {
    if (handler) {
        AsioDeadlineTimeoutPtr timeout
            = new AsioDeadlineTimeout(ioService_, millisecond);

        timeout->timer().async_wait(boost::bind(
                                           &AsioService::repeatTimerExpiresCallback,
                                           this,
                                           boost::asio::placeholders::error,
                                           handler,
                                           millisecond,
                                           boost::cref(timeout)));
        timeout->setState(AsioDeadlineTimeout::TIMER_ACTIVE);
        timers_.push_back(timeout);
        return boost::static_pointer_cast<Timeout>(timeout);
    }
    else {
        return TimeoutPtr();
    }
}

void AsioService::timerExpiresCallback(const boost::system::error_code& code,
                                       const Handler& handler,
                                       const AsioDeadlineTimeoutPtr& timeout) {
    if (code != boost::asio::error::operation_aborted) {
        timeout->setState(AsioDeadlineTimeout::TIMER_EXPIRED);
        handler();
    }
    else {
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
        handler();

        timers_.remove(timeout);

        //FIXME
        AsioDeadlineTimeoutPtr newTimeout
            = new AsioDeadlineTimeout(ioService_, millisecond);

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
        }
        else {
            //timers.remove(timeout);
            //LOG_ERROR << "";
        }
    }
    else {
        timeout->setState(AsioDeadlineTimeout::TIMER_CANCELLED);
        timers_.remove(timeout);
    }
}

}
}
}
