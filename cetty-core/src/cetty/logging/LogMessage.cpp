
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

#include <cetty/logging/LogMessage.h>
#include <cetty/logging/Logger.h>

namespace cetty {
namespace logging {

LogMessage::LogMessage(const LogLevel& level, const char* source, int line)
    : level(level),
      source(source),
      line(line),
      func(""),
      stream((char*)buffer, MAX_LOG_MESSAGE_LEN) {

    memset(buffer, 0, sizeof(buffer));

    timestamp = boost::posix_time::microsec_clock::universal_time();
    tid = boost::this_thread::get_id();

    Logger::getFormatter().formatFirst(*this);
}

LogMessage::LogMessage(const LogLevel& level, const char* source, int line, const char* func)
    : level(level),
      source(source),
      line(line),
      func(func),
      stream((char*)buffer, MAX_LOG_MESSAGE_LEN) {

    memset(buffer, 0, sizeof(buffer));

    timestamp = boost::posix_time::microsec_clock::universal_time();
    tid = boost::this_thread::get_id();

    Logger::getFormatter().formatFirst(*this);
}

void LogMessage::finish() {
    Logger::getFormatter().formatLast(*this);
    stream << "\n";
}

}
}