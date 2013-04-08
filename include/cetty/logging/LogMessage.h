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

#ifdef __DEPRECATED
// Make GCC quiet.
# undef __DEPRECATED
# include <strstream>
# define __DEPRECATED
#else
# include <strstream>
#endif

#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <cetty/logging/LogLevel.h>
#include <cetty/util/Process.h>
#include <cetty/util/CurrentThread.h>

namespace cetty {
namespace logging {

using namespace cetty::util;

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
    LogMessage(const LogLevel& level,
               const char* source,
               int line);

    LogMessage(const LogLevel& level,
               const char* source,
               int line,
               const char* func);

    bool finish();

public:
    Stream& stream() { return stream_; }
    const char* buffer() const { return buffer_; }

    const LogLevel& level() const { return level_; }
    const ThreadId& threadId() const { return tid_; }
    const boost::posix_time::ptime& timeStamp() const { return timestamp_; }

    const char* sourceFile() const { return source_; }
    const char* function() const { return function_; }
    int souceLine() const { return line_; }

private:
    //static int64_t messageCounter[];

private:
    int line_;
    const char* source_;
    const char* function_;

    ThreadId tid_;
    Process::PID pid_;

    LogLevel level_;
    boost::posix_time::ptime timestamp_;

    char buffer_[MAX_LOG_MESSAGE_LEN];
    Stream stream_;
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
