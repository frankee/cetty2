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

#include <cetty/util/TimeUnit.h>

namespace cetty {
namespace util {

const TimeUnit TimeUnit::SECONDS = 0;
const TimeUnit TimeUnit::MILLISECONDS = 1;
const TimeUnit TimeUnit::MICROSECONDS = 2; // 10e-6
const TimeUnit TimeUnit::NANOSECONDS = 3;

boost::int64_t TimeUnit::toNanos(boost::int64_t duration) const {
    if (m_value == SECONDS.m_value) {
        return duration * 1000000000;
    }
    else if (m_value == MILLISECONDS.m_value) {
        return duration * 1000000;
    }
    else if (m_value == MICROSECONDS.m_value) {
        return duration * 1000;
    }
    else if (m_value == NANOSECONDS.m_value) {
        return duration;
    }

    return duration;
}

boost::int64_t TimeUnit::toMicros(boost::int64_t duration) const {
    if (m_value == SECONDS.m_value) {
        return duration * 1000000;
    }
    else if (m_value == MILLISECONDS.m_value) {
        return duration * 1000;
    }
    else if (m_value == MICROSECONDS.m_value) {
        return duration;
    }
    else if (m_value == NANOSECONDS.m_value) {
        return duration / 1000;
    }

    return duration;
}

boost::int64_t TimeUnit::toMillis(boost::int64_t duration) const {
    if (m_value == SECONDS.m_value) {
        return duration * 1000;
    }
    else if (m_value == MILLISECONDS.m_value) {
        return duration;
    }
    else if (m_value == MICROSECONDS.m_value) {
        return duration / 1000;
    }
    else if (m_value == NANOSECONDS.m_value) {
        return duration / 1000000;
    }

    return duration;
}

boost::int64_t TimeUnit::toSeconds(boost::int64_t duration) const {
    if (m_value == SECONDS.m_value) {
        return duration;
    }
    else if (m_value == MILLISECONDS.m_value) {
        return duration / 1000;
    }
    else if (m_value == MICROSECONDS.m_value) {
        return duration / 1000000;
    }
    else if (m_value == NANOSECONDS.m_value) {
        return duration / 1000000000;
    }

    return duration;
}

std::string TimeUnit::toString() const {
    switch (m_value) {
    case 0:
        return "SECONDS";

    case 1:
        return "MILLISECONDS";

    case 2:
        return "MICROSECONDS";

    case 3:
        return "MICROSECONDS";

    default:
        return "UNKNOW";
    }
}

}
}


