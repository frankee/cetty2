
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

using namespace cetty::util;

LogMessage::LogMessage(const LogLevel& level,
                       const char* source,
                       int line)
    : line_(line),
      source_(source),
      function_(""),
      level_(level),
      stream_(static_cast<char*>(buffer_), MAX_LOG_MESSAGE_LEN) {

    if (level < Logger::logLevel()) {
        return;
    }

    memset(buffer_, 0, sizeof(buffer_));

    timestamp_ = boost::posix_time::microsec_clock::universal_time();
    tid_ = CurrentThread::id();

    Logger::getFormatter().formatFirst(*this);
}

LogMessage::LogMessage(const LogLevel& level, const char* source, int line, const char* func)
    : line_(line),
      source_(source),
      function_(func),
      level_(level),
      stream_(static_cast<char*>(buffer_), MAX_LOG_MESSAGE_LEN) {

    if (level < Logger::logLevel()) {
        return;
    }

    memset(buffer_, 0, sizeof(buffer_));

    timestamp_ = boost::posix_time::microsec_clock::universal_time();
    tid_ = CurrentThread::id();

    Logger::getFormatter().formatFirst(*this);
}

bool LogMessage::finish() {
    if (level_ < Logger::logLevel()) {
        return false;
    }

    Logger::getFormatter().formatLast(*this);
    stream_ << "\n";

    return true;
}

}
}
