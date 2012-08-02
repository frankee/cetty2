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

#include <cetty/util/internal/asio/AsioDeadlineTimer.h>
#include <cetty/util/internal/asio/AsioDeadlineTimeout.h>
#include <cetty/util/TimeUnit.h>
#include <cetty/channel/socket/asio/AsioService.h>

namespace cetty {
namespace util {
namespace internal {
namespace asio {

using namespace cetty::util;

AsioDeadlineTimer::AsioDeadlineTimer(const EventLoopPtr& eventLoop)
    : ioService(boost::dynamic_pointer_cast<AsioService>(eventLoop)) {
    BOOST_ASSERT(ioService && "constract eventLoop only can be AsioService");
}

AsioDeadlineTimer::~AsioDeadlineTimer() {
    stop();
}

const TimeoutPtr& AsioDeadlineTimer::newTimeout(const TimerTask& task, boost::int64_t delay) {
    clearInactivedTimeout();

    timeouts.push_back(new AsioDeadlineTimeout(this, ioService, task, delay));
    return timeouts.back();
}

const TimeoutPtr& AsioDeadlineTimer::newTimeout(const TimerTask& task, boost::int64_t delay, const TimeUnit& unit) {
    clearInactivedTimeout();

    timeouts.push_back(new AsioDeadlineTimeout(this, ioService, task, unit.toMillis(delay)));
    return timeouts.back();
}

void AsioDeadlineTimer::stop() {
    TimeoutList::iterator itr;

    for (itr = timeouts.begin(); itr != timeouts.end(); ++itr) {
        const TimeoutPtr& timeout = *itr;

        if (timeout && timeout->isActived()) {
            timeout->cancel();
        }
    }
}

static bool isInactived(const TimeoutPtr& timeout) {
    return !(timeout && timeout->isActived());
}

void AsioDeadlineTimer::clearInactivedTimeout() {
    timeouts.remove_if(&isInactived);
}

}
}
}
}
