#if !defined(CETTY_LOGGING_SIMPLELOGGER_H)
#define CETTY_LOGGING_SIMPLELOGGER_H

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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
#include <cstdio>
#include <cetty/logging/AbstractInternalLogger.h>

namespace cetty {
namespace logging {

class SimpleLogger : public AbstractInternalLogger {
public:
    SimpleLogger(const std::string& name)
        : name(name), level(InternalLogLevel::INFO) {
    }

    SimpleLogger(const std::string& name, const InternalLogLevel& level)
        : name(name), level(level) {

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
        std::printf("%s\n", msg.c_str());
    }

private:
    std::string name;
    InternalLogLevel level;
};

}
}

#endif //#if !defined(CETTY_LOGGING_SIMPLELOGGER_H)
