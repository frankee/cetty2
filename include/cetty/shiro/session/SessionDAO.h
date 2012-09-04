#if !defined(CETTY_SHIRO_SESSION_SESSIONDAO_H)
#define CETTY_SHIRO_SESSION_SESSIONDAO_H
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
#include <cetty/shiro/session/SessionPtr.h>
#include <cetty/shiro/session/SessionIdGenerator.h>
#include <cetty/shiro/session/Session.h>

namespace cetty {
namespace shiro {
namespace session {

/**
 * Data Access Object design pattern specification to enable {@link Session} access to an
 * EIS (Enterprise Information System).  It provides your four typical CRUD methods:
 * {@link #create}, {@link #readSession}, {@link #update(Session)},
 * and {@link #delete(Session)}.
 * <p/>
 * The remaining {@link #getActiveSessions()} method exists as a support mechanism to pre-emptively orphaned sessions,
 * typically by {@link org.apache.shiro.session.mgt.ValidatingSessionManager ValidatingSessionManager}s), and should
 * be as efficient as possible, especially if there are thousands of active sessions.  Large scale/high performance
 * implementations will often return a subset of the total active sessions and perform validation a little more
 * frequently, rather than return a massive set and infrequently validate.
 */
class SessionDAO {
public:
    virtual ~SessionDAO(){}

    /**
     * Retrieves the Session object from the underlying EIS identified by <tt>sessionId</tt> by delegating to
     * the {@link #doReadSession(Serializable)} method.  If {@code null} is returned from that method, an
     * {@link UnknownSessionException} will be thrown.
     *
     * @param sessionId the id of the session to retrieve from the EIS.
     * @return the session identified by <tt>sessionId</tt> in the EIS.
     */
    SessionPtr readSession(const std::string &sessionId){
        SessionPtr s = doReadSession(sessionId);
        return s;
    }


    /**
     * Creates the session by delegating EIS creation to subclasses via the {@link #doCreate} method, and then
     * asserting that the returned sessionId is not null.
     *
     * @param session Session object to create in the EIS and associate with an ID.
     */
    const std::string create(SessionPtr &session) {
        std::string sessionId = doCreate(session);
        return sessionId;
    }
    virtual void getActiveSessions(std::vector<SessionPtr> *actives) = 0;
    virtual void update(SessionPtr &session) = 0;
    virtual void remove(SessionPtr &session) = 0;

protected:
    /**
     * Generates a new ID to be applied to the specified {@code session} instance.  This method is usually called
     * from within a subclass's {@link #doCreate} implementation where they assign the returned id to the session
     * instance and then create a record with this ID in the EIS data store.
     * <p/>
     * Subclass implementations backed by EIS data stores that auto-generate IDs during record creation, such as
     * relational databases, don't need to use this method or the {@link #getSessionIdGenerator() sessionIdGenerator}
     * attribute - they can simply return the data store's generated ID from the {@link #doCreate} implementation
     * if desired.
     * <p/>
     * This implementation uses the {@link #setSessionIdGenerator configured} {@link SessionIdGenerator} to create
     * the ID.
     *
     * @param session the new session instance for which an ID will be generated and then assigned
     * @return the generated ID to assign
     */
    std::string generateSessionId(SessionPtr &session) {
        return SessionIdGenerator::generateId(session);
    }

    /**
     * Utility method available to subclasses that wish to
     * assign a generated session ID to the session instance directly.  This method is not used by the
     * implementation directly, but it is provided so subclasses don't
     * need to know the {@code Session} implementation if they don't need to.
     * <p/>
     * This default implementation casts the argument to a {@link Session}, Shiro's default EIS implementation.
     *
     * @param session   the session instance to which the sessionId will be applied
     * @param sessionId the id to assign to the specified session instance.
     */
    void assignSessionId(SessionPtr &session, const std::string &sessionId) {
        session->setId(sessionId);
    }

    /**
     * Subclass hook to actually persist the given <tt>Session</tt> instance to the underlying EIS.
     *
     * @param session the Session instance to persist to the EIS.
     * @return the id of the session created in the EIS (i.e. this is almost always a primary key and should be the
     *         value returned from {@link Session#getId() Session.getId()}.
     */
    virtual std::string doCreate(SessionPtr &session) = 0;

    /**
     * Subclass implementation hook that retrieves the Session object from the underlying EIS or {@code null} if a
     * session with that ID could not be found.
     *
     * @param sessionId the id of the <tt>Session</tt> to retrieve.
     * @return the Session in the EIS identified by <tt>sessionId</tt> or {@code null} if a
     *         session with that ID could not be found.
     */
    virtual SessionPtr doReadSession(const std::string &sessionId) = 0;

};
}
}
}

#endif // CETTY_SHIRO_SESSION_SESSIONDAO_H