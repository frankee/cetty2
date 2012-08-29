#if !defined(CETTY_SHIRO_SESSION_SESSIONIDGENERATOR_H)
#define CETTY_SHIRO_SESSION_SESSIONIDGENERATOR_H
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <string>

#include <cetty/shiro/session/SessionPtr.h>

namespace cetty {
namespace shiro {
namespace session {

/**
 * Generates session IDs by using a host name, nanoseconds passed from 1970-1-1 00:00:00
 * and process id
 */
class SessionIdGenerator {
public:
    /**
     * Returns the String value of the session id: host + current time + id
     *
     * @param session the {@link Session} instance to which the ID will be applied.
     * @return the String value of the session id
     */
    static std::string generateId(SessionPtr session);

};

}
}
}
#endif // #if !defined(CETTY_SHIRO_SESSION_SESSIONIDGENERATOR_H)
