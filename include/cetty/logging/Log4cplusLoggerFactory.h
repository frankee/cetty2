#if !defined(CETTY_LOGGING_LOG4CPLUSLOGGERFACTORY_H)
#define CETTY_LOGGING_LOG4CPLUSLOGGERFACTORY_H

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

#include <cetty/logging/InternalLoggerFactory.h>

namespace cetty {
namespace logging {

class Log4cplusLoggerFactory : public InternalLoggerFactory {
public:
    Log4cplusLoggerFactory(const std::string& configureFile);
    virtual ~Log4cplusLoggerFactory();

    virtual InternalLogger* newInstance(const std::string& name);
};

}
}

#endif //#if !defined(CETTY_LOGGING_LOG4CPLUSLOGGERFACTORY_H)

// Local Variables:
// mode: c++
// End:
