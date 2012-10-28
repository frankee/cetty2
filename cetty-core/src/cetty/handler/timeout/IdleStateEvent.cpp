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

#include <cetty/handler/timeout/IdleStateEvent.h>

#include <cetty/util/StringUtil.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace handler {
namespace timeout {

IdleStateEvent::IdleStateEvent(IdleState state,
                               int count,
                               int64_t durationMillis)
    : state(state), count(count), durationMillis(durationMillis) {
    if (count < 0) {
        LOG_WARN << "count: " << count <<" (expected: >= 0)";
    }

    if (durationMillis < 0) {
        LOG_WARN << "durationMillis: "
                 << durationMillis
                 << " (expected: >= 0)";
    }
}

std::string IdleStateEvent::toString() const {
    return cetty::util::StringUtil::strprintf("%s(%d, %dms)",
            state.toString().c_str(),
            count,
            durationMillis);
}

}
}
}
