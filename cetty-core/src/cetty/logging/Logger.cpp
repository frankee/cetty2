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

#include <cetty/logging/Logger.h>

#include <cstdio>
#include <cstdlib>

#include <cetty/util/StringUtil.h>
#include <cetty/logging/LogConsoleSink.h>

namespace cetty {
namespace logging {

LogLevel Logger::level_ = LogLevel::DEBUG;
LogPatternFormatter* Logger::formatter = NULL;
std::vector<LogSinkPtr> Logger::sinks_;

Logger::Logger(SourceFile file, int line)
    : message_(LogLevel::INFO, file.data(), line) {
}

Logger::Logger(SourceFile file, int line, LogLevel level)
    : message_(level, file.data(), line) {
}

Logger::Logger(SourceFile file, int line, const char* func, LogLevel level)
    : message_(level, file.data(), line, func) {
}

Logger::Logger(SourceFile file, int line, bool toAbort)
    : message_(toAbort ? LogLevel::FATAL : LogLevel::ERROR, file.data(), line) {
}

Logger::~Logger() {
    if (sinks_.empty()) {
        sinks_.push_back(new LogConsoleSink);
    }

    if (message_.finish()) {
        bool needFlush = message_.level() == LogLevel::FATAL;
        std::size_t j = sinks_.size();
        for (std::size_t i = 0; i < j; ++i) {
            sinks_[i]->sink(message_);
            
            if (needFlush) {
                sinks_[i]->flush();
            }
        }
    }
}

void Logger::addLogSink(const LogSinkPtr& sink) {
    if (sink) {
        sinks_.push_back(sink);
    }
}

}
}
