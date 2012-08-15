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

#include <cetty/logging/LogLevel.h>

namespace cetty {
namespace logging {

const LogLevel LogLevel::FATAL(5);
const LogLevel LogLevel::ERROR(4);
const LogLevel LogLevel::WARN(3);
const LogLevel LogLevel::INFO(2);
const LogLevel LogLevel::DEBUG(1);
const LogLevel LogLevel::TRACE(0);

static const std::string LOG_LEVLE_STRS[] = 
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
    "UNKNOWN"
};

const std::string& LogLevel::toString() const {
    return LOG_LEVLE_STRS[this->level];
}

}
}
