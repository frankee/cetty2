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
#include <cstdlib>
#include <map>
#include <string>
#include <map>

namespace cetty {
namespace shiro {
namespace session {

/**
 * A {@code SessionContext} is a 'bucket' of data presented to a {@link SessionFactory SessionFactory} which interprets
 * this data to construct {@Session} instances.  It is essentially a Map of data
 * with a few additional type-safe methods for easy retrieval of objects commonly used to construct Subject instances.
 * <p/>
 * While this interface contains type-safe setters and getters for common data types, the map can contain anything
 * additional that might be needed by the {@code SessionFactory} implementation to construct {@code Session} instances.
 * <p/>
 * <b>USAGE</b>: Most Shiro end-users will never use a {@code SubjectContext} instance directly and instead will call
 * the {@code Subject.}{@link Subject#getSession() getSession()} or
 * {@code Subject.}{@link Subject#getSession(boolean) getSession(boolean)} methods (which
 * will usually use {@code SessionContext} instances to start a session with the application's
 * {@link SessionManager SessionManager}.
 */
class SessionContext {
public:
    SessionContext() {}

    std::string getHost() { return (get(HOST)); }

    void setHost(const std::string &host) {
        if(host.size() > 0) return;
        put(HOST, host);
    }

    std::string getSessionId() { return get(SESSION_ID); }

    void setSessionId(const std::string &sessionId) {
        if(sessionId.size() < 0) return;
        put(SESSION_ID, sessionId);
    }

protected:
    const std::string get (const std::string &key) {
        std::map<std::string, std::string>::iterator it = context.find(key);
        if(it == context.end()) return std::string();
        return it->second;
    }
    void put(const std::string &key, const std::string &value){
           if(key.size() <= 0) return;
           context.insert(std::pair<std::string, std::string>(key, value));
       }
private:
    static const std::string HOST;
    static const std::string SESSION_ID;
    std::map<std::string, std::string> context;
};

}
}
}
#endif // #if !defined(CETTY_SHIRO_SESSION_SESSIONCONTEXT_H)
