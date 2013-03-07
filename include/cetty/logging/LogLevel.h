#if !defined(CETTY_LOGGING_LOGLEVEL_H)
#define CETTY_LOGGING_LOGLEVEL_H

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

#include <vector>
#include <string>
#include <cetty/util/Enum.h>

#if defined(WIN32)

#if defined(ERROR)
#undef ERROR
#endif

#if defined(TRACE)
#undef TRACE
#endif

#endif

namespace cetty {
namespace logging {

class LogLevel : public cetty::util::Enum<LogLevel> {
public:
    static const LogLevel FATAL;
    static const LogLevel ERROR;
    static const LogLevel WARN;
    static const LogLevel INFO;
    static const LogLevel DEBUG;
    static const LogLevel TRACE;

public:
    const std::string& toString() const;

    int toInt() const {
        return v;
    }

    static LogLevel parseFrom(const std::string& level);

private:
    LogLevel(int value) : cetty::util::Enum<LogLevel>(value) {}
};

}
}

#endif //#if !defined(CETTY_LOGGING_LOGLEVEL_H)

// Local Variables:
// mode: c++
// End:
