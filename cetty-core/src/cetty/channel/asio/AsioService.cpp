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
    ioService.stop();
}

void AsioService::post(const Functor& handler) {
    ioService.post(handler);
}

TimeoutPtr AsioService::runAt(const boost::posix_time::ptime& timestamp,
                              const Functor& timerCallback) {
    if (timerCallback) {
        AsioDeadlineTimeoutPtr timeout
            = new AsioDeadlineTimeout(ioService, timestamp);

        timeout->getTimer().async_wait(boost::bind(
                                           &AsioService::timerExpiresCallback,
                                           this,
                                           boost::asio::placeholders::error,
                                           timerCallback,
                                           boost::cref(timeout)));

        timeout->setState(AsioDeadlineTimeout::TIMER_ACTIVE);
        timers.push_back(timeout);
        return boost::static_pointer_cast<Timeout>(timeout);
    }
    else {
        return TimeoutPtr();
    }
}

TimeoutPtr AsioService::runAfter(int64_t millisecond,
                                 const Functor& timerCallback) {
    if (timerCallback) {
        AsioDeadlineTimeoutPtr timeout
            = new AsioDeadlineTimeout(ioService, millisecond);

        timeout->getTimer().async_wait(boost::bind(
                                           &AsioService::timerExpiresCallback,
                                           this,
                                           boost::asio::placeholders::error,
                                           timerCallback,
                                           boost::cref(timeout)));

        timeout->setState(AsioDeadlineTimeout::TIMER_ACTIVE);
        timers.push_back(timeout);
        return boost::static_pointer_cast<Timeout>(timeout);
    }
    else {
        return TimeoutPtr();
    }
}

TimeoutPtr AsioService::runEvery(int64_t millisecond,
                                 const Functor& timerCallback) {
    if (timerCallback) {
        AsioDeadlineTimeoutPtr timeout
            = new AsioDeadlineTimeout(ioService, millisecond);

        timeout->getTimer().async_wait(boost::bind(
                                           &AsioService::repeatTimerExpiresCallback,
                                           this,
                                           boost::asio::placeholders::error,
                                           timerCallback,
                                           millisecond,
                                           boost::cref(timeout)));
        timeout->setState(AsioDeadlineTimeout::TIMER_ACTIVE);
        timers.push_back(timeout);
        return boost::static_pointer_cast<Timeout>(timeout);
    }
    else {
        return TimeoutPtr();
    }
}

void AsioService::timerExpiresCallback(const boost::system::error_code& code,
                                       const Functor& timerCallback,
                                       const AsioDeadlineTimeoutPtr& timeout) {
    if (code != boost::asio::error::operation_aborted) {
        timeout->setState(AsioDeadlineTimeout::TIMER_EXPIRED);
        timerCallback();
    }
    else {
        timeout->setState(AsioDeadlineTimeout::TIMER_CANCELLED);
    }

    timers.remove(timeout);
}

void AsioService::repeatTimerExpiresCallback(const boost::system::error_code& code,
        const Functor& timerCallback,
        int64_t millisecond,
        const AsioDeadlineTimeoutPtr& timeout) {
    if (code != boost::asio::error::operation_aborted) {
        timeout->setState(AsioDeadlineTimeout::TIMER_EXPIRED);
        timerCallback();

        timers.remove(timeout);

        //FIXME
        AsioDeadlineTimeoutPtr newTimeout
            = new AsioDeadlineTimeout(ioService, millisecond);

        boost::system::error_code code;
        boost::asio::deadline_timer& timer = newTimeout->getTimer();
        timer.expires_from_now(
            boost::posix_time::milliseconds(millisecond),
            code);

        if (!code) {
            timer.async_wait(boost::bind(&AsioService::repeatTimerExpiresCallback,
                                         this,
                                         boost::asio::placeholders::error,
                                         timerCallback,
                                         millisecond,
                                         newTimeout));
            newTimeout->setState(AsioDeadlineTimeout::TIMER_ACTIVE);
            timers.push_back(newTimeout);
        }
        else {
            //timers.remove(timeout);
            //LOG_ERROR << "";
        }
    }
    else {
        timeout->setState(AsioDeadlineTimeout::TIMER_CANCELLED);
        timers.remove(timeout);
    }
}

}
}
}
