#if !defined(CETTY_LOGGING_ABSTRACTINTERNALLOGGER_H)
#define CETTY_LOGGING_ABSTRACTINTERNALLOGGER_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 *
 */

#include <cetty/logging/InternalLogLevel.h>
#include <cetty/logging/InternalLogger.h>

namespace cetty {
namespace logging {

/**
 * A skeletal implementation of {@link InternalLogger}.  This class implements
 * all methods that have a {@link InternalLogLevel} parameter by default to call
 * specific logger methods such as {@link #info(String)} or {@link #isInfoEnabled()}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class AbstractInternalLogger : public InternalLogger {
public:
    virtual ~AbstractInternalLogger() {}

    virtual bool isDebugEnabled() const {
        return isEnabled(InternalLogLevel::DEBUG);
    }

    virtual bool isInfoEnabled() const {
        return isEnabled(InternalLogLevel::INFO);
    }

    virtual bool isWarnEnabled() const {
        return isEnabled(InternalLogLevel::WARN);
    }

    virtual bool isErrorEnabled() const {
        return isEnabled(InternalLogLevel::ERROR);
    }

    virtual void debug(const std::string& msg, const Exception& cause,
                       const char* file = NULL, int line = 0) {
        log(InternalLogLevel::DEBUG, msg, cause, file, line);
    }

    virtual void info(const std::string& msg, const Exception& cause,
                      const char* file = NULL, int line = 0) {
        log(InternalLogLevel::INFO, msg, cause, file, line);
    }

    virtual void warn(const std::string& msg, const Exception& cause,
                      const char* file = NULL, int line = 0) {
        log(InternalLogLevel::WARN, msg, cause, file, line);
    }

    virtual void error(const std::string& msg, const Exception& cause,
                       const char* file = NULL, int line = 0) {
        log(InternalLogLevel::ERROR, msg, cause, file, line);
    }

    virtual void debug(const std::string& msg, const char* file = NULL, int line = 0) {
        log(InternalLogLevel::DEBUG, msg, file, line);
    }

    virtual void info(const std::string& msg, const char* file = NULL, int line = 0) {
        log(InternalLogLevel::INFO, msg, file, line);
    }

    virtual void warn(const std::string& msg, const char* file = NULL, int line = 0) {
        log(InternalLogLevel::WARN, msg, file, line);
    }

    virtual void error(const std::string& msg, const char* file = NULL, int line = 0) {
        log(InternalLogLevel::ERROR, msg, file, line);
    }

    virtual void log(const InternalLogLevel& level, const std::string& msg,
                     const Exception& cause, const char* file = NULL, int line = 0) {

        log(level, msg, file, line);
    }

    virtual void log(const InternalLogLevel& level, const std::string& msg,
                     const char* file = NULL, int line = 0) {
        if (isEnabled(level)) {
            printMessage(msg, file, line);
        }
    }

protected:
    virtual void printMessage(const std::string& msg, const char* file, int line)  = 0;

protected:
    /**
     * Creates a new instance.
     */
    AbstractInternalLogger() {}
};

}
}

#endif //#if !defined(CETTY_LOGGING_ABSTRACTINTERNALLOGGER_H)

// Local Variables:
// mode: c++
// End:

