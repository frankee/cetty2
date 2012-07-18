#if !defined(CETTY_LOGGING_SIMPLELOGGERFACTORY_H)
#define CETTY_LOGGING_SIMPLELOGGERFACTORY_H

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

#include <cetty/logging/SimpleLogger.h>
#include <cetty/logging/InternalLoggerFactory.h>

namespace cetty {
namespace logging {

/**
 * Logger factory which creates an
 * <a href="http://commons.apache.org/logging/">Apache Commons Logging</a>
 * logger.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */
class SimpleLoggerFactory : public InternalLoggerFactory {
public:
    SimpleLoggerFactory() {}
    virtual ~SimpleLoggerFactory() {}

    virtual InternalLogger* newInstance(const std::string& name) {
        return new SimpleLogger(name);
    }
};

}
}

#endif //#if !defined(CETTY_LOGGING_SIMPLELOGGERFACTORY_H)

// Local Variables:
// mode: c++
// End:
