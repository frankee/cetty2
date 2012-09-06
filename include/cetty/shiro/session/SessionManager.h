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
#include <boost/date_time/posix_time/ptime.hpp>

#include <cetty/shiro/session/SessionDAO.h>
#include <cetty/shiro/session/SessionListener.h>
#include <cetty/shiro/session/SessionValidationScheduler.h>
#include <cetty/shiro/session/MemorySessionDAO.h>
#include <cetty/shiro/session/SessionContext.h>
#include <cetty/shiro/session/SessionFactory.h>

namespace cetty {
namespace shiro {
namespace session {

using namespace boost::posix_time;

/**
 * A SessionManager manages the creation, maintenance, and clean-up of all application
 * Sessions.
 */
class SessionManager{
public:
    /**
     * Default main session timeout value, equal to {@code 30} minutes.
     */
    static const int MILLIS_PER_SECOND ;
    static const int MILLIS_PER_MINUTE;
    static const int MILLIS_PER_HOUR;
    static const int DEFAULT_GLOBAL_SESSION_TIMEOUT;
    static const int DEFAULT_SESSION_VALIDATION_INTERVAL;

    SessionManager();
    ~SessionManager();

    void setSessionDAO(SessionDAO *sessionDAO) { this->sessionDAO = sessionDAO; }
    SessionDAO *getSessionDAO() { return this->sessionDAO; }

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
     */
    bool isDeleteInvalidSessions() const { return deleteInvalidSessions; }

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
     */
    void setDeleteInvalidSessions(bool deleteInvalidSessions) {
        this->deleteInvalidSessions = deleteInvalidSessions;
    }

    void setSessionListeners(const std::vector<SessionChangeCallback> &listeners) {
        if(!listeners.empty())
            this->listeners.assign(listeners.begin(), listeners.end());
    }

    const std::vector<SessionChangeCallback> &getSessionListeners() const{
        return this->listeners;
    }

    ptime getStartTimestamp(const std::string &sessionId);

    ptime getLastAccessTime(const std::string &sessionId);

    /** return -1 represent no session found*/
    int getTimeout(const std::string &sessionId);

    void setTimeout(const std::string &id, int maxIdleTimeInMillis);

    void touch(const std::string &id);

    std::string getHost(const std::string &id);

    void getAttributeKeys(const std::string &id, std::vector<std::string> *keys);

    std::string& getAttribute(const std::string &id, const std::string &key);

    void setAttribute(const std::string &id, const std::string &key, const std::string &value);

    void removeAttribute(const std::string &id, const std::string &key);

    bool isLogin(const std::string &id);
    void setLogin(const std::string &id, bool login);

    bool isValid(const std::string &id) { return checkValid(id); }
    bool checkValid(const std::string &id) { return !lookupRequiredSession(id); }

    void stop(const std::string &id);

    /**
     * Returns the system-wide default time in milliseconds that any session may remain idle before expiring. This
     * value is the main default for all sessions and may be overridden on a <em>per-session</em> basis by calling
     * {@code Subject.getSession().}{@link Session#setTimeout setTimeout(long)} if so desired.
     * <ul>
     * <li>A negative return value means sessions never expire.</li>
     * <li>A non-negative return value (0 or greater) means session timeout will occur as expected.</li>
     * </ul>
     * <p/>
     * Unless overridden via the {@link #setGlobalSessionTimeout} method, the default value is
     * {@link #DEFAULT_GLOBAL_SESSION_TIMEOUT}.
     *
     * @return the time in milliseconds that any session may remain idle before expiring.
     */
    int getGlobalSessionTimeout() const { return this->globalSessionTimeout; }

    /**
     * Sets the system-wide default time in milliseconds that any session may remain idle before expiring. This
     * value is the main default for all sessions and may be overridden on a <em>per-session</em> basis by calling
     * {@code Subject.getSession().}{@link Session#setTimeout setTimeout(long)} if so desired.
     * <p/>
     * <ul>
     * <li>A negative return value means sessions never expire.</li>
     * <li>A non-negative return value (0 or greater) means session timeout will occur as expected.</li>
     * </ul>
     * <p/>
     * Unless overridden by calling this method, the default value is {@link #DEFAULT_GLOBAL_SESSION_TIMEOUT}.
     *
     * @param globalSessionTimeout the time in milliseconds that any session may remain idel before expiring.
     */
    void setGlobalSessionTimeout(int globalSessionTimeout) {
        this->globalSessionTimeout = globalSessionTimeout;
    }

    bool isSessionValidationSchedulerEnabled() const {
        return sessionValidationSchedulerEnabled;
    }

    void setSessionValidationSchedulerEnabled(bool sessionValidationSchedulerEnabled) {
        this->sessionValidationSchedulerEnabled = sessionValidationSchedulerEnabled;
    }

    void setSessionValidationScheduler(SessionValidationScheduler *sessionValidationScheduler) {
        this->sessionValidationScheduler = sessionValidationScheduler;
    }

    SessionValidationScheduler *getSessionValidationScheduler() const{
        return sessionValidationScheduler;
    }

    void enableSessionValidationIfNecessary() {
        SessionValidationScheduler *scheduler = getSessionValidationScheduler();
        if (isSessionValidationSchedulerEnabled() && (scheduler == NULL || !scheduler->isEnabled())) {
            enableSessionValidation();
        }
    }

    /**
     * If using the underlying default <tt>SessionValidationScheduler</tt> (that is, the
     * {@link #setSessionValidationScheduler(SessionValidationScheduler) setSessionValidationScheduler} method is
     * never called) , this method allows one to specify how
     * frequently session should be validated (to check for orphans).  The default value is
     * {@link #DEFAULT_SESSION_VALIDATION_INTERVAL}.
     * <p/>
     * If you override the default scheduler, it is assumed that overriding instance 'knows' how often to
     * validate sessions, and this attribute will be ignored.
     * <p/>
     * Unless this method is called, the default value is {@link #DEFAULT_SESSION_VALIDATION_INTERVAL}.
     *
     * @param sessionValidationInterval the time in milliseconds between checking for valid sessions to reap orphans.
     */
    void setSessionValidationInterval(int sessionValidationInterval) {
        this->sessionValidationInterval = sessionValidationInterval;
    }

    int getSessionValidationInterval() const {
        return sessionValidationInterval;
    }

    /**
     * Subclass template hook in case per-session timeout is not based on
     * {@link oSession#getTimeout()}.
     * <p/>
     * <p>This implementation merely returns {@link Session#getTimeout()}</p>
     *
     * @param session the session for which to determine session timeout.
     * @return the time in milliseconds the specified session may remain idle before expiring.
     */
    int getTimeout(const SessionPtr &session) {
        if(!session) return -1;
        return session->getTimeout();
    }

    void validateSessions();

    SessionPtr start();

    SessionValidationScheduler *createSessionValidationScheduler(){
        SessionValidationScheduler *svs = new SessionValidationScheduler(this);
        return svs;
    }

    SessionPtr getSession(const std::string &id){
        SessionPtr session = doGetSession(id);
        return session;
    }

protected:
    SessionPtr doCreateSession() {
        SessionPtr s = new Session();
        create(s);
        return s;
    }

    SessionPtr doGetSession(const std::string &id){
        enableSessionValidationIfNecessary();
        SessionPtr s = retrieveSession(id);
        if (s) validate(s);
        return s;
    }

    Session::SessionState validate(SessionPtr &session);

    void beforeSessionValidationDisabled() {}

    void afterSessionValidationEnabled() {}
    void afterStopped(SessionPtr &session) {
        if (isDeleteInvalidSessions())
            remove(session);
    }

    void afterExpired(SessionPtr &session) {
        if (isDeleteInvalidSessions())
            remove(session);
    }

    void onStop(SessionPtr &session) {
        ptime stopTs = session->getStopTimestamp();
        session->setLastAccessTime(stopTs);
        onChange(session);
    }

    // void onStop(SessionPtr session, const std::string &id) { onStop(session); }

    void onExpiration(SessionPtr session) {
        session->setExpired(true);
        onChange(session);
    }

    void onChange(SessionPtr &session) {
        sessionDAO->update(session);
    }

    void onExpiration(SessionPtr &session, const std::string &id) {
        onExpiration(session);
        notifyExpiration(session);
        afterExpired(session);
   }

    /**
     * Template method that allows subclasses to react to a new session being created.
     * <p/>
     * This method is invoked <em>before</em> any session listeners are notified.
     *
     * @param session the session that was just {@link #createSession created}.
     * @param context the {@link SessionContext SessionContext} that was used to start the session.
     */
    void onStart(SessionPtr session) {}
    void onInvalidation(SessionPtr &session, Session::SessionState state);

    /**
     * Persists the given session instance to an underlying EIS (Enterprise Information System).  This implementation
     * delegates and calls
     * <code>this.{@link SessionDAO sessionDAO}.{@link SessionDAO#create(Session) create}(session);<code>
     *
     * @param session the Session instance to persist to the underlying EIS.
     */
    void create(SessionPtr &session) { sessionDAO->create(session); }

    /**
     * Looks up a session from the underlying data store based on the specified session key.
     *
     * @param key the session key to use to look up the target session.
     * @return the session identified by {@code sessionId}.
     * @throws UnknownSessionException if there is no session identified by {@code sessionId}.
     */

    SessionPtr retrieveSession(const std::string &id){ return sessionDAO->readSession(id); }

    void remove(SessionPtr &session) { sessionDAO->remove(session); }

    void getActiveSessions(std::vector<SessionPtr> *actives) { return sessionDAO->getActiveSessions(actives); }

    void applyGlobalSessionTimeout(SessionPtr session) {
        session->setTimeout(getGlobalSessionTimeout());
        onChange(session);
    }



    /**
     * Notifies any interested {@link SessionListener}s that a Session has started.  This method is invoked
     * <em>after</em> the {@link #onStart onStart} method is called.
     *
     * @param session the session that has just started that will be delivered to any
     *                {@link #setSessionListeners(java.util.Collection) registered} session listeners.
     * @see SessionListener#onStart(Session)
     */
    void notifyStart(const SessionPtr &session);
    void notifyStop(const SessionPtr &session);
    void notifyExpiration(const SessionPtr &session);

    /**
     * Creates a new {@code Session Session} instance based on the specified (possibly {@code null})
     * initialization data.  Implementing classes must manage the persistent state of the returned session such that it
     * could later be acquired via the {@link #getSession(SessionKey)} method.
     *
     * @param context the initialization data that can be used by the implementation or underlying
     *                {@link SessionFactory} when instantiating the internal {@code Session} instance.
     * @return the new {@code Session} instance.
     * @throws org.apache.shiro.authz.HostUnauthorizedException
     *                                if the system access control policy restricts access based
     *                                on client location/IP and the specified hostAddress hasn't been enabled.
     * @throws AuthorizationException if the system access control policy does not allow the currently executing
     *                                caller to start sessions.
     */
    SessionPtr createSession() {
        enableSessionValidationIfNecessary();
        return doCreateSession();
    }



    void enableSessionValidation();
    void disableSessionValidation();

private:
    SessionPtr lookupRequiredSession(const std::string &id){
        return doGetSession(id);
    }

private:
    bool deleteInvalidSessions;
    bool sessionValidationSchedulerEnabled;



    int globalSessionTimeout;
    int sessionValidationInterval;



    SessionDAO *sessionDAO;
    SessionValidationScheduler *sessionValidationScheduler;

    std::vector<SessionChangeCallback> listeners;
};
}
}
}

#endif //#if !defined(CETTY_SHIRO_SESSION_SESSIONMANAGER_H)
