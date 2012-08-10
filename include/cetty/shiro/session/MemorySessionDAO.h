#if !defined(CETTY_SHIRO_SESSION_MEMORYSESSIONDAO_H)
#define CETTY_SHIRO_SESSION_MEMORYSESSIONDAO_H
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

namespace cetty {
namespace shiro {
namespace session {

/**
 * Simple memory-based implementation of the SessionDAO that stores all of its sessions in an in-memory
 * {@link ConcurrentMap}.  <b>This implementation does not page to disk and is therefore unsuitable for applications
 * that could experience a large amount of sessions</b> and would therefore cause {@code OutOfMemoryException}s.  It is
 * <em>not</em> recommended for production use in most environments.
 * <h2>Memory Restrictions</h2>
 * If your application is expected to host many sessions beyond what can be stored in the
 * memory available to the JVM, it is highly recommended to use a different {@code SessionDAO} implementation which
 * uses a more expansive or permanent backing data store.
 * <p/>
 * In this case, it is recommended to instead use a custom
 * {@link CachingSessionDAO} implementation that communicates with a higher-capacity data store of your choice
 * (file system, database, etc).
 * <h2>Changes in 1.0</h2>
 * This implementation prior to 1.0 used to subclass the {@link CachingSessionDAO}, but this caused problems with many
 * cache implementations that would expunge entries due to TTL settings, resulting in Sessions that would be randomly
 * (and permanently) lost.  The Shiro 1.0 release refactored this implementation to be 100% memory-based (without
 * {@code Cache} usage to avoid this problem.
 *
 * @see CachingSessionDAO
 * @since 0.1
 */
class MemorySessionDAO : public SessionDAO {
private:
    std::map<std::string, Session*> sessions;



protected:
    std::string doCreate(Session *session) {
        std::string sessionId = generateSessionId(session);
        assignSessionId(session, sessionId);
        storeSession(sessionId, session);
        return sessionId;
    }

    Session *storeSession(const std::string &id, Session *session) {
        if (id == "") {
           return NULL;
        }
        return sessions.insert(std::pair<std::string, Session *>(id, session));
    }

    virtual Session *doReadSession(const std::string &sessionId) {
        std::map<std::string, Session*>::iterator ret = sessions.find(sessionId);
        if(ret == sessions.end()) return NULL;
        return ret->second;
    }


public:
    virtual std::vector<Session *> * getActiveSessions();
    virtual void update(Session *session){
        storeSession(session->getId(), session);
    }

    virtual void remove(Session *session) {
        if (session == NULL) {
            return;
        }
        std::string id = session->getId();
        if (id != "") {
            sessions.erase(id);
        }
    }

};
}
}
}
#endif // #if !defined(CETTY_SHIRO_SESSION_MEMORYSESSIONDAO_H)
