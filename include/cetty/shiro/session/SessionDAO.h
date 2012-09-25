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

#include <boost/function.hpp>
#include <cetty/shiro/session/Session.h>
#include <cetty/shiro/session/SessionPtr.h>

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
    typedef boost::function2<void, int, const SessionPtr&> SessionCallback;

public:
    virtual ~SessionDAO() {}

    /**
     * Retrieves the Session object from the underlying EIS identified by <tt>sessionId</tt> by delegating to
     * the {@link #doReadSession(Serializable)} method.  If {@code null} is returned from that method, an
     * {@link UnknownSessionException} will be thrown.
     *
     * @param sessionId the id of the session to retrieve from the EIS.
     * @return the session identified by <tt>sessionId</tt> in the EIS.
     */
    virtual void readSession(const std::string &sessionId, const SessionCallback& callback) = 0;
    
    /**
     * Inserts a new Session record into the underling EIS (e.g. Relational database, file system, persistent cache,
     * etc, depending on the DAO implementation).
     * <p/>
     * After this method is invoked, the {@link org.apache.shiro.session.Session#getId()}
     * method executed on the argument must return a valid session identifier.  That is, the following should
     * always be true:
     * <pre>
     * Serializable id = create( session );
     * id.equals( session.getId() ) == true</pre>
     * <p/>
     * Implementations are free to throw any exceptions that might occur due to
     * integrity violation constraints or other EIS related errors.
     *
     * @param session the {@link org.apache.shiro.session.Session} object to create in the EIS.
     * @return the EIS id (e.g. primary key) of the created {@code Session} object.
     */
    virtual void create(const SessionPtr &session, const SessionCallback& callback) = 0;
    
    virtual void update(const SessionPtr &session, const SessionCallback& callback) = 0;
    virtual void remove(const SessionPtr &session, const SessionCallback& callback) = 0;

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
    std::string generateSessionId(const SessionPtr &session);

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
    void assignSessionId(const SessionPtr &session, const std::string &sessionId);
};

}
}
}

#endif // CETTY_SHIRO_SESSION_SESSIONDAO_H
