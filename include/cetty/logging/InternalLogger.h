#if !defined(CETTY_LOGGING_INTERNALLOGGER_H)
#define CETTY_LOGGING_INTERNALLOGGER_H

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
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <string>

namespace cetty {
namespace util {
class Exception;
}
}

namespace cetty {
namespace logging {

using namespace cetty::util;

class InternalLogLevel;

/**
 * <em>Internal-use-only</em> logger used by Cetty.  <strong>DO NOT</strong>
 * access this class outside of Cetty.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class InternalLogger {
public:
    virtual ~InternalLogger() {}

    /**
     * Returns <tt>true</tt> if a DEBUG level message is logged.
     */
    virtual bool isDebugEnabled() const = 0;

    /**
     * Returns <tt>true</tt> if an INFO level message is logged.
     */
    virtual bool isInfoEnabled() const = 0;

    /**
     * Returns <tt>true</tt> if a WARN level message is logged.
     */
    virtual bool isWarnEnabled() const = 0;

    /**
     * Returns <tt>true</tt> if an ERROR level message is logged.
     */
    virtual bool isErrorEnabled() const = 0;

    /**
     * Returns <tt>true</tt> if the specified log level message is logged.
     */
    virtual bool isEnabled(const InternalLogLevel& level) const = 0;

    /**
     * set the specified log level for logger.
     */
    virtual void setLogLevel(const InternalLogLevel& level) = 0;

    /**
     * Logs a DEBUG level message.
     */
    virtual void debug(const std::string& msg, const char* file = NULL, int line = 0) = 0;

    /**
     * Logs a DEBUG level message.
     */
    virtual void debug(const std::string& msg, const Exception& cause,
                       const char* file = NULL, int line = 0) = 0;

    /**
     * Logs an INFO level message.
     */
    virtual void info(const std::string& msg, const char* file = NULL, int line = 0) = 0;

    /**
     * Logs an INFO level message.
     */
    virtual void info(const std::string& msg, const Exception& cause,
                      const char* file = NULL, int line = 0) = 0;

    /**
     * Logs a WARN level message.
     */
    virtual void warn(const std::string& msg, const char* file = NULL, int line = 0) = 0;

    /**
     * Logs a WARN level message.
     */
    virtual void warn(const std::string& msg, const Exception& cause,
                      const char* file = NULL, int line = 0) = 0;

    /**
     * Logs an ERROR level message.
     */
    virtual void error(const std::string& msg, const char* file = NULL, int line = 0) = 0;

    /**
     * Logs an ERROR level message.
     */
    virtual void error(const std::string& msg, const Exception& cause,
                       const char* file = NULL, int line = 0) = 0;

    /**
     * Logs a message.
     */
    virtual void log(const InternalLogLevel& level, const std::string& msg,
                     const char* file = NULL, int line = 0) = 0;

    /**
     * Logs a message.
     */
    virtual void log(const InternalLogLevel& level, const std::string& msg, const Exception& cause,
                     const char* file = NULL, int line = 0) = 0;

    /**
     *
     */
    virtual std::string toString() const = 0;

};

}
}

#endif //#if !defined(CETTY_LOGGING_INTERNALLOGGER_H)

// Local Variables:
// mode: c++
// End:

