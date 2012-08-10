#if !defined(CETTY_SHIRO_SESSION_SESSIONMANAGER_H)
#define CETTY_SHIRO_SESSION_SESSIONMANAGER_H
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
 * Default business-tier implementation of a {@link ValidatingSessionManager}.  All session CRUD operations are
 * delegated to an internal {@link SessionDAO}.
 *
 * @since 0.1
 */
class SessionManager : public AbstractValidatingSessionManager{
protected:
    SessionDAO *sessionDAO;  //todo - move SessionDAO up to AbstractValidatingSessionManager?

    CacheManager *cacheManager;

private:
    bool deleteInvalidSessions;

public:
    SessionManager()
      :deleteInvalidSessions(true),
       sessionDAO(NULL),
       cacheManager(NULL){}

    void setSessionDAO(SessionDAO *sessionDAO) {
        this->sessionDAO = sessionDAO;
        applyCacheManagerToSessionDAO();
    }

    SessionDAO *getSessionDAO() {
        return this->sessionDAO;
    }

    /**
     * Returns {@code true} if sessions should be automatically deleted after they are discovered to be invalid,
     * {@code false} if invalid sessions will be manually deleted by some process external to Shiro's control.  The
     * default is {@code true} to ensure no orphans exist in the underlying data store.
     * <h4>Usage</h4>
     * It is ok to set this to {@code false} <b><em>ONLY</em></b> if you have some other process that you manage yourself
     * that periodically deletes invalid sessions from the backing data store over time, such as via a Quartz or Cron
     * job.  If you do not do this, the invalid sessions will become 'orphans' and fill up the data store over time.
     * <p/>
     * This property is provided because some systems need the ability to perform querying/reporting against sessions in
     * the data store, even after they have stopped or expired.  Setting this attribute to {@code false} will allow
     * such querying, but with the caveat that the application developer/configurer deletes the sessions themselves by
     * some other means (cron, quartz, etc).
     *
     * @return {@code true} if sessions should be automatically deleted after they are discovered to be invalid,
     *         {@code false} if invalid sessions will be manually deleted by some process external to Shiro's control.
     * @since 1.0
     */
    bool isDeleteInvalidSessions() {
        return deleteInvalidSessions;
    }

    /**
     * Sets whether or not sessions should be automatically deleted after they are discovered to be invalid.  Default
     * value is {@code true} to ensure no orphans will exist in the underlying data store.
     * <h4>WARNING</h4>
     * Only set this value to {@code false} if you are manually going to delete sessions yourself by some process
     * (quartz, cron, etc) external to Shiro's control.  See the
     * {@link #isDeleteInvalidSessions() isDeleteInvalidSessions()} JavaDoc for more.
     *
     * @param deleteInvalidSessions whether or not sessions should be automatically deleted after they are discovered
     *                              to be invalid.
     * @since 1.0
     */
    void setDeleteInvalidSessions(bool deleteInvalidSessions) {
        this->deleteInvalidSessions = deleteInvalidSessions;
    }

    void setCacheManager(CacheManager *cacheManager) {
        this->cacheManager = cacheManager;
        applyCacheManagerToSessionDAO();
    }

    /**
     * Sets the internal {@code CacheManager} on the {@code SessionDAO} if it implements the
     * {@link org.apache.shiro.cache.CacheManagerAware CacheManagerAware} interface.
     * <p/>
     * This method is called after setting a cacheManager via the
     * {@link #setCacheManager(org.apache.shiro.cache.CacheManager) setCacheManager} method <em>em</em> when
     * setting a {@code SessionDAO} via the {@link #setSessionDAO} method to allow it to be propagated
     * in either case.
     *
     * @since 1.0
     */
    void applyCacheManagerToSessionDAO() {
        if (this->cacheManager != NULL && this->sessionDAO != NULL) {
            (this->sessionDAO)->setCacheManager(this->cacheManager);
        }
    }

    DelegatingSession* createExposedSession(Session *session, SessionContext context) {
        DelegatingSession *ds = new DelegatingSession(*this, SessionKey(session->getId()));
        return ds;
    }

    Session *createExposedSession(Session *session, SessionKey key) {
        DelegatingSession *ds = new DelegatingSession(*this, key.getSessionId());
        return ds;
    }

protected:
    Session *doCreateSession(SessionContext context) {
        Session *s = newSessionInstance(context);
        create(s);
        return s;
    }

    Session *newSessionInstance(SessionContext context) {
        return SessionFactory::createSession(context);
    }

    /**
     * Persists the given session instance to an underlying EIS (Enterprise Information System).  This implementation
     * delegates and calls
     * <code>this.{@link SessionDAO sessionDAO}.{@link SessionDAO#create(org.apache.shiro.session.Session) create}(session);<code>
     *
     * @param session the Session instance to persist to the underlying EIS.
     */
    void create(Session *session) {
        sessionDAO->create(session);
    }

    void onStop(Session *session) {
        time_t stopTs = session->getStopTimestamp();
            session->setLastAccessTime(stopTs);
        onChange(session);
    }

    void afterStopped(Session *session) {
        if (isDeleteInvalidSessions()) {
            delete(session);
        }
    }

    void onExpiration(Session *session) {
        session->setExpired(true);
        onChange(session);
    }

    void afterExpired(Session *session) {
        if (isDeleteInvalidSessions()) {
            delete(session);
        }
    }

    void onChange(Session *session) {
        sessionDAO->update(session);
    }

    Session *retrieveSession(const SessionKey &sessionKey){
        std::string sessionId = getSessionId(sessionKey);
        if (sessionId == "") return NULL;
        Session *s = retrieveSessionFromDataSource(sessionId);
        return s;
    }

    const std::string &getSessionId(const SessionKey &sessionKey) {
        return sessionKey.getSessionId();
    }

    Session *retrieveSessionFromDataSource(const SessionKey &sessionKey) {
        return sessionDAO->readSession(sessionKey.getSessionId());
    }

    void remove(Session *session) {
        sessionDAO->remove(session);
    }

    std::vector<Session *> *getActiveSessions() {
        return sessionDAO->getActiveSessions();
    }

    Session *getSession(SessionKey key){
        Session *session = lookupSession(key);
        return session != NULL ? createExposedSession(session, key) : NULL;
    }

    Session *start(const SessionContext &context) {
       Session *session = createSession(context);
       if(session != NULL){
           applyGlobalSessionTimeout(session);
           onStart(session, context);
           notifyStart(session);
           //Don't expose the EIS-tier Session object to the client-tier:
           return createExposedSession(session, context);
       }
       return session;
   }

};
}
}
}

#endif //#if !defined(CETTY_SHIRO_SESSION_SESSIONMANAGER_H)
