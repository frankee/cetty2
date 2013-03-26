
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

#include <cetty/channel/asio/AsioDeadlineTimeout.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/channel/asio/AsioService.h>

namespace cetty {
namespace channel {
namespace asio {

AsioDeadlineTimeout::AsioDeadlineTimeout(AsioService& service,
        int id,
        const boost::posix_time::ptime& timestamp)
    : id_(id),
      threadId_(service.threadId()),
      deadlineTimer_(service.service(), timestamp) {
}

AsioDeadlineTimeout::AsioDeadlineTimeout(AsioService& service,
        int id,
        int64_t delay)
    : id_(id),
      threadId_(service.threadId()),
      deadlineTimer_(service.service(),
                     boost::posix_time::milliseconds(delay)) {
}

AsioDeadlineTimeout::~AsioDeadlineTimeout() {
}

bool AsioDeadlineTimeout::isExpired() const {
    return state_ == TIMER_EXPIRED;
}

bool AsioDeadlineTimeout::isCancelled() const {
    return state_ == TIMER_CANCELLED;
}

bool AsioDeadlineTimeout::isActived() const {
    return state_ == TIMER_ACTIVE;
}

void AsioDeadlineTimeout::cancel() {
    LOG_INFO << "canceling timer " << threadId_ << ":" << id_;

    boost::system::error_code code;
    deadlineTimer_.cancel(code);

    if (code) {
        LOG_ERROR << "cancel timer has error, code: " << code.value()
                  << ", message: " << code.message();
    }
}

boost::int64_t AsioDeadlineTimeout::expiresFromNow() const {
    return deadlineTimer_.expires_from_now().total_milliseconds();
}

}
}
}
