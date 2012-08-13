#if !defined(CETTY_SHIRO_SESSION_SESSIONCONTEXT_H)
#define CETTY_SHIRO_SESSION_SESSIONCONTEXT_H
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

namespace cetty {
namespace shiro {
namespace session {

/**
 * Default implementation of the {@link SessionContext} interface which provides getters and setters that
 * wrap interaction with the underlying backing context map.
 *
 * @since 1.0
 */
class SessionContext {
private:
    static const std::string HOST;
    static const std::string SESSION_ID;

public:
    SessionContext() {}

    SessionContext(const std::map<std::string, std::string> &map) {}

    std::string getHost() {
        return getValue(HOST);
    }

    void setHost(const std::string &host) {
        if (host.size() > 0) {
            put(HOST, host);
        }
    }

    std::string getSessionId() {
        return getValue(SESSION_ID);
    }

    void setSessionId(std::string sessionId) {
        nullSafePut(SESSION_ID, sessionId);
    }

private:
    std::map<std::string, std::string> context;
};

}
}
}
#endif // #if !defined(CETTY_SHIRO_SESSION_SESSIONCONTEXT_H)
