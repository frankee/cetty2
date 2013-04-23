#if !defined(CETTY_LOGGING_LOGSINK_H)
#define CETTY_LOGGING_LOGSINK_H

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

#include <boost/optional.hpp>
#include <cetty/logging/LogLevel.h>
#include <cetty/logging/LogMessage.h>
#include <cetty/logging/LogSinkPtr.h>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace logging {

class LogSink : public cetty::util::ReferenceCounter<LogSink> {
public:
    virtual~ LogSink() {}

    void sink(const LogMessage& msg) {
        if (enabled(msg)) {
            doSink(msg);

            if (immediateFlush_) {
                doFlush();
            }
        }
    }

    void flush() {
        doFlush();
    }

    /**
     * Returns this appenders threshold LogLevel. See the {@link
     * #setThreshold} method for the meaning of this option.
     */
    //const LogLevel& thresholdLevel() const;

    /**
     * Set the threshold LogLevel. All log events with lower LogLevel
     * than the threshold LogLevel are ignored by the appender.
     * 
     * In configuration files this option is specified by setting the
     * value of the <b>Threshold</b> option to a LogLevel
     * string, such as "DEBUG", "INFO" and so on.
     */
    void setThresholdLevel(const LogLevel& level);

    /**
     * When it is set true, output stream will be flushed after
     * each appended event.
     */
    bool immediateFlush() const;
    void setImmediateFlush(bool immediateFlush);

protected:
    LogSink(const std::string& name)
        : immediateFlush_(false),
          name_(name) {
    }

    virtual void doSink(const LogMessage& msg) = 0;
    virtual void doFlush() = 0;

private:
    /**
     * Check whether the message LogLevel is below the appender's
     * threshold. If there is no threshold set, then the return value is
     * always <code>true</code>.
     */
    bool enabled(const LogMessage& msg) const;

private:
    bool immediateFlush_;

    std::string name_;
    boost::optional<LogLevel> level_;
};

// inline
// const LogLevel& LogSink::thresholdLevel() const {
//     return *level_;
// }

inline
void LogSink::setThresholdLevel(const LogLevel& level) {
    level_ = level;
}

inline
bool LogSink::immediateFlush() const {
    return immediateFlush_;
}

inline
void LogSink::setImmediateFlush(bool immediateFlush) {
    immediateFlush_ = immediateFlush;
}

inline
bool LogSink::enabled(const LogMessage& msg) const {
    return !level_ ? true : msg.level() >= *level_;
}

}
}

#endif //#if !defined(CETTY_LOGGING_LOGSINK_H)

// Local Variables:
// mode: c++
// End:
