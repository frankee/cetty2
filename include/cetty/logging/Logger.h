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
            : data(arr),
            size(N-1) {
                const char* slash = strrchr(data, '\\');

                if (slash) {
                    data = slash + 1;
                    size -= static_cast<int>(data - arr);
                }
        }

        SourceFile(const char* data, int size)
            : data(data), size(size) {
        }

        const char* data;
        int size;
    };

public:
    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, const char* func, LogLevel level);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogMessage::Stream& stream() { return message.getStream(); }

public:
    static const LogPatternFormatter& getFormatter() {
        return formatter;
    }

    static bool isEnabled(const LogLevel& level) {
        return level >= Logger::level;
    }

    static LogLevel logLevel() {
        return level;
    }
    static void logLevel(LogLevel level) {
        Logger::level = level;
    }

private:
    LogMessage message;

    static LogLevel level;
    static LogPatternFormatter formatter;
};

}
}

#endif //#if !defined(CETTY_LOGGING_LOGGER_H)

// Local Variables:
// mode: c++
// End:
