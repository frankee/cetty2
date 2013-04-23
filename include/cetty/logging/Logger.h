#if !defined(CETTY_LOGGING_LOGGER_H)
#define CETTY_LOGGING_LOGGER_H

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

#include <cetty/logging/LogSink.h>
#include <cetty/logging/LogLevel.h>
#include <cetty/logging/LogMessage.h>
#include <cetty/logging/LogPatternFormatter.h>

namespace cetty {
namespace logging {

class Logger {
public:
    // compile time calculation of basename of source file
    class SourceFile {
    public:
        template<int N>
        inline SourceFile(const char(&arr)[N])
            : data_(arr),
              size_(N-1) {
            const char* slash = strrchr(data_, '\\');

            if (slash) {
                data_ = slash + 1;
                size_ -= static_cast<int>(data_ - arr);
            }
        }

        SourceFile(const char* data, int size)
            : data_(data), size_(size) {
        }

        int size() const { return size_; }
        const char* data() const { return data_; }

    private:
        const char* data_;
        int size_;
    };

public:
    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, const char* func, LogLevel level);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogMessage::Stream& stream() { return message_.stream(); }

public:
    static const LogLevel& level();
    static void setLevel(const LogLevel& level);

    static bool isEnabled(const LogLevel& level);

    static const LogPatternFormatter& patternFormatter();
    static void setPatternFormatter(const std::string& format);

    static void addLogSink(const LogSinkPtr& sink);

private:
    LogMessage message_;

    static LogLevel level_;
    static LogPatternFormatter* formatter;
    static std::vector<LogSinkPtr> sinks_;
};

inline
const LogLevel& Logger::level() {
    return level_;
}

inline
void Logger::setLevel(const LogLevel& level) {
    Logger::level_ = level;
}

inline
bool Logger::isEnabled(const LogLevel& level) {
    return level >= Logger::level_;
}

inline
const LogPatternFormatter& Logger::patternFormatter() {
    if (!formatter) {
        formatter = new LogPatternFormatter();    
    }

    return *formatter;
}

inline
void Logger::setPatternFormatter(const std::string& format) {
    if (formatter) {
        delete formatter;
    }
    formatter = new LogPatternFormatter(format);
}

}
}

#endif //#if !defined(CETTY_LOGGING_LOGGER_H)

// Local Variables:
// mode: c++
// End:
