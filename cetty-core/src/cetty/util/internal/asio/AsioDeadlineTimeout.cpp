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

#include <cetty/util/internal/asio/AsioDeadlineTimeout.h>
#include <cetty/util/internal/asio/AsioDeadlineTimer.h>
#include <cetty/channel/socket/asio/AsioService.h>

namespace cetty {
namespace util {
namespace internal {
namespace asio {

using namespace cetty::util;
using namespace cetty::channel::socket::asio;

AsioDeadlineTimeout::AsioDeadlineTimeout(const TimerPtr& timer,
        const AsioServicePtr& ioService,
        const TimerTask& task,
        boost::int64_t delay)
    : state(TIMER_ACTIVE), timer(timer), deadlineTimer(ioService->service()) {

    BOOST_ASSERT(task);
    deadlineTimer.expires_from_now(boost::posix_time::milliseconds(delay));
    deadlineTimer.async_wait(boost::bind(&AsioDeadlineTimeout::timerExpiresCallback,
                                         this,
                                         boost::asio::placeholders::error,
                                         boost::cref(task)));
}

const TimerPtr& AsioDeadlineTimeout::getTimer() const {
    return timer;
}

AsioDeadlineTimeout::~AsioDeadlineTimeout() {

}

bool AsioDeadlineTimeout::isExpired() const {
    return (state == TIMER_EXPIRED);
}

bool AsioDeadlineTimeout::isCancelled() const {
    return (state == TIMER_CANCELLED);
}

bool AsioDeadlineTimeout::isActived() const {
    return (state == TIMER_ACTIVE);
}

void AsioDeadlineTimeout::cancel() {
    deadlineTimer.cancel();
}

int AsioDeadlineTimeout::expiresFromNow() const {
    //         deadline_timer::duration_type duration =
    //             deadline_timer::traits_type::subtract(deadline_timer.expires_at(),
    //                                                   deadline_timer::traits_type::now());
    //         return deadline_timer::traits_type::to_posix_duration(duration).total_milliseconds();
    //     }

    return 0;
}

void AsioDeadlineTimeout::timerExpiresCallback(const boost::system::error_code& error, const TimerTask& task) {
    if (boost::asio::error::operation_aborted == error) {
        state = TIMER_CANCELLED;
    }
    else {
        state = TIMER_EXPIRED;

        if (task) {
            task(*this);
        }
        else {
            //log here.
        }
    }
}

}
}
}
}
