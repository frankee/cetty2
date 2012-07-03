#if !defined(CETTY_LOGGING_LOG4CPLUSLOGGER_H)
#define CETTY_LOGGING_LOG4CPLUSLOGGER_H

/**
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

#if defined(HAS_LOG4CPLUS)

#include <log4cplus/logger.h>
#include <cetty/logging/AbstractInternalLogger.h>

namespace cetty {
namespace logging {

class Log4cplusLogger : public AbstractInternalLogger {
public:
    Log4cplusLogger(const std::string& name, const log4cplus::Logger& logger)
        : name(name), level(InternalLogLevel::INFO), logger(logger) {
    }

    Log4cplusLogger(const std::string& name, const InternalLogLevel& level, const log4cplus::Logger& logger)
        : name(name), level(level), logger(logger) {
    }

    virtual bool isEnabled(const InternalLogLevel& level) const {
        return level >= this->level;
    }

    virtual void setLogLevel(const InternalLogLevel& level) {
        this->level = level;
    }

    virtual std::string toString() const {
        return name;
    }

protected:
    virtual void printMessage(const std::string& msg, const char* file, int line) {
        logger.log(level.value(), msg, file, line);
    }

private:
    std::string name;
    InternalLogLevel level;
    log4cplus::Logger logger;

};

}
}

#endif

#endif //#if !defined(CETTY_LOGGING_LOG4CPLUSLOGGER_H)
