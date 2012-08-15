#if !defined(CETTY_LOGGING_LOGMESSAGE_H)
#define CETTY_LOGGING_LOGMESSAGE_H

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

#include <ctime>
#include <strstream>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <cetty/logging/LogLevel.h>

namespace cetty {
namespace logging {

class LogMessage {
public:
    class Stream : public std::ostrstream {
    public:
        Stream(char* buf, int len)
            : std::ostrstream(buf, len) {
        }
    };

    static const std::size_t MAX_LOG_MESSAGE_LEN = 1024 * 8;

public:
    LogMessage(const LogLevel& level, const char* source, int line);

    LogMessage(const LogLevel& level, const char* source, int line, const char* func);

    void finish();

public:
    Stream& getStream() { return stream; }
    const char* getBuffer() const { return buffer; }

    const LogLevel& getLevel() const { return level; }
    const boost::posix_time::ptime& getTimeStamp() const { return timestamp; }
    const boost::thread::id& getThreadId() const { return tid; }

    const char* getSourceFile() const { return source; }
    const char* getFunction() const { return func; }
    int getSouceLine() const { return line; }

private:
    //static boost::int64_t messageCounter[];

private:
    LogLevel level;

    boost::posix_time::ptime timestamp;

    int pid;
    boost::thread::id tid;

    int line;
    const char* source;
    const char* func;

    char buffer[MAX_LOG_MESSAGE_LEN];
    Stream stream;
};

// This class is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect".
class LogMessageVoidify {
public:
    LogMessageVoidify() {}

    // This has to be an operator with a precedence lower than << but
    // higher than ?:
    void operator&(std::ostream&) { }
};

}
}

#endif //#if !defined(CETTY_LOGGING_LOGMESSAGE_H)

// Local Variables:
// mode: c++
// End:
