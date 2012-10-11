#if !defined(CETTY_SHIRO_SESSION_SESSIONMANAGERCONFIG_H)
#define CETTY_SHIRO_SESSION_SESSIONMANAGERCONFIG_H

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

#include <cetty/config/ConfigObject.h>

namespace cetty {
namespace shiro {
namespace session {

using namespace cetty::config;

class SessionManagerConfig : public ConfigObject {
public:
    bool deleteInvalidSessions;
    bool sessionValidationSchedulerEnabled;

    int globalSessionTimeout;
    int sessionValidationInterval;

    SessionManagerConfig();
    virtual ConfigObject* create() const { return new SessionManagerConfig; }
};

}
}
}

#endif //#if !defined(CETTY_SHIRO_SESSION_SESSIONMANAGERCONFIG_H)

// Local Variables:
// mode: c++
// End:
