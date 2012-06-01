#if !defined(CETTY_LOGGING_INTERNALLOGLEVEL_H)
#define CETTY_LOGGING_INTERNALLOGLEVEL_H

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
#include <cetty/util/Enum.h>

#if defined(WIN32) && defined(ERROR)
#undef ERROR
#endif

// Local Variables:
// mode: c++
// End:


namespace cetty {
namespace logging {

/**
 * The log level that {@link InternalLogger} can log at.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */
class InternalLogLevel : public cetty::util::Enum<InternalLogLevel> {
public:

    /**
     * 'DEBUG' log level.
     */
    static const InternalLogLevel DEBUG;

    /**
     * 'INFO' log level.
     */
    static const InternalLogLevel INFO;

    /**
     * 'WARN' log level.
     */
    static const InternalLogLevel WARN;

    /**
     * 'ERROR' log level.
     */
    static const InternalLogLevel ERROR;

    /**
     * Constructs a string describing this enum.
     * This method returns the string "DEBUG" for DEBUG, "INFO" for INFO,
     * "WARN" for WARN and "ERROR" for ERROR.
     *
     * @return The specified string
     */
    std::string toString() const;

private:
    InternalLogLevel(int value) : cetty::util::Enum<InternalLogLevel>(value) {}
};

}
}

#endif //#if !defined(CETTY_LOGGING_INTERNALLOGLEVEL_H)

// Local Variables:
// mode: c++
// End:

