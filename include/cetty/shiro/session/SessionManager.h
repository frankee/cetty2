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
#include <vector>
#include <boost/function.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <cetty/shiro/session/SessionPtr.h>

namespace cetty {
namespace shiro {
namespace session {

using namespace boost::posix_time;

/**
 *   int SessionState
 */
typedef boost::function2<void, const SessionPtr&, int> SessionChangeCallback;

class SessionDAO;
class SessionContext;
class SessionValidationScheduler;

/**
 * A SessionManager manages the creation, maintenance, and clean-up of all application
 * Sessions.
 */
class SessionManager {
public:
    static const int DEFAULT_GLOBAL_SESSION_TIMEOUT;
    static const int DEFAULT_SESSION_VALIDATION_INTERVAL;

    typedef boost::function1<void, const SessionPtr&> SessionCallback;
    typedef std::vector<SessionChangeCallback> SessionChangeCallbacks;

public:
    SessionManager();
    virtual ~SessionManager();

    /**
     * Starts a new session based on the specified contextual initialization data, which can be used by the underlying
     * implementation to determine how exactly to create the internal Session instance.
     * <p/>
     * This method is mainly used in framework development, as the implementation will often relay the argument
     * to an underlying {@link SessionFactory} which could use the context to construct the internal Session
     * instance in a specific manner.  This allows pluggable {@link org.apache.shiro.session.Session Session} creation
     * logic by simply injecting a {@code SessionFactory} into the {@code SessionManager} instance.
     *
     * @param context the contextual initialization data that can be used by the implementation or underlying
     *                {@link SessionFactory} when instantiating the internal {@code Session} instance.
     * @return the newly created session.
     * @see SessionFactory#createSession(SessionContext)
     * @since 1.0
     */
    void start(const std::string& host, const SessionCallback& callback);

    void getSession(const std::string& id, const SessionCallback& callback);

    // from cache.
    SessionPtr getSession(const std::string& id);

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
    bool isDeleteInvalidSessions() const;

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
    void setDeleteInvalidSessions(bool deleteInvalidSessions);

    void addSessionListeners(const SessionChangeCallback& callback);

    const std::vector<SessionChangeCallback>& getSessionListeners() const;

    void clearSessionListeners();

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
    int getGlobalSessionTimeout() const;

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
    void setGlobalSessionTimeout(int globalSessionTimeout);

    bool isSessionValidationSchedulerEnabled() const;

    void setSessionValidationSchedulerEnabled(bool sessionValidationSchedulerEnabled);

    void setSessionValidationScheduler(SessionValidationScheduler* sessionValidationScheduler);

    SessionValidationScheduler* getSessionValidationScheduler() const;

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
    void setSessionValidationInterval(int sessionValidationInterval);

    int getSessionValidationInterval() const;

    void setSessionDAO(SessionDAO* sessionDAO);
    SessionDAO* getSessionDAO();

    void validateSessions();

    void enableSessionValidation();
    void disableSessionValidation();

protected:
    virtual void beforeSessionValidationDisabled() { /*NOOP*/ }
    virtual void afterSessionValidationEnabled() { /*NOOP*/ }

    /**
     * Template method that allows subclasses to react to a new session being created.
     * <p/>
     * This method is invoked <em>before</em> any session listeners are notified.
     *
     * @param session the session that was just {@link #createSession created}.
     * @param context the {@link SessionContext SessionContext} that was used to start the session.
     */
    virtual void onStart(const SessionPtr& session) { /*NOOP*/ }

    virtual void onStop(const SessionPtr& session);

    virtual void afterStopped(const SessionPtr& session);

    virtual void onExpiration(const SessionPtr& session);

    virtual void afterExpired(const SessionPtr& session);

    virtual void onChange(const SessionPtr& session);

    virtual void onInvalidation(const SessionPtr& session, int state);

private:
    void applyGlobalSessionTimeout(const SessionPtr& session);

    void remove(const SessionPtr& session);
    void stop(const SessionPtr& session);
    void expire(const SessionPtr& session);

    int validate(const SessionPtr& session);

    void enableSessionValidationIfNecessary();

    void getActiveSessions(std::vector<SessionPtr>* actives);

    SessionValidationScheduler* createSessionValidationScheduler();

    /**
      * Notifies any interested {@link SessionListener}s that a Session has started.  This method is invoked
      * <em>after</em> the {@link #onStart onStart} method is called.
      *
      * @param session the session that has just started that will be delivered to any
      *                {@link #setSessionListeners(java.util.Collection) registered} session listeners.
      * @see SessionListener#onStart(Session)
      */
    void notifyStart(const SessionPtr& session);
    void notifyStop(const SessionPtr& session);
    void notifyExpiration(const SessionPtr& session);

    void createSessionCallback(int result,
                               const SessionPtr& session,
                               const SessionCallback& callback);

    void readSessionCallback(int result,
                             const SessionPtr& session,
                             const SessionCallback& callback);

private:
    bool deleteInvalidSessions;
    bool sessionValidationSchedulerEnabled;

    int globalSessionTimeout;
    int sessionValidationInterval;

    SessionDAO* sessionDAO;
    SessionValidationScheduler* validationScheduler;

    std::map<std::string, SessionPtr> sessions;
    std::vector<SessionChangeCallback> listeners;
};

inline
bool SessionManager::isDeleteInvalidSessions() const {
    return this->deleteInvalidSessions;
}

inline
const std::vector<SessionChangeCallback>& SessionManager::getSessionListeners() const {
    return this->listeners;
}

inline
void SessionManager::clearSessionListeners() {
    listeners.clear();
}

inline
void SessionManager::setDeleteInvalidSessions(bool deleteInvalidSessions) {
    this->deleteInvalidSessions = deleteInvalidSessions;
}

inline
int SessionManager::getGlobalSessionTimeout() const {
    return this->globalSessionTimeout;
}

inline
void SessionManager::setGlobalSessionTimeout(int globalSessionTimeout) {
    this->globalSessionTimeout = globalSessionTimeout;
}

inline
bool SessionManager::isSessionValidationSchedulerEnabled() const {
    return sessionValidationSchedulerEnabled;
}

inline
void SessionManager::setSessionValidationSchedulerEnabled(bool sessionValidationSchedulerEnabled) {
    this->sessionValidationSchedulerEnabled = sessionValidationSchedulerEnabled;
}

inline
void SessionManager::setSessionValidationScheduler(SessionValidationScheduler* sessionValidationScheduler) {
    this->validationScheduler = sessionValidationScheduler;
}

inline
SessionValidationScheduler* SessionManager::getSessionValidationScheduler() const {
    return validationScheduler;
}

inline
void SessionManager::setSessionValidationInterval(int sessionValidationInterval) {
    this->sessionValidationInterval = sessionValidationInterval;
}

inline
int SessionManager::getSessionValidationInterval() const {
    return sessionValidationInterval;
}

inline
void SessionManager::setSessionDAO(SessionDAO* sessionDAO) {
    this->sessionDAO = sessionDAO;
}

inline
SessionDAO* SessionManager::getSessionDAO() {
    return this->sessionDAO;
}

}
}
}

#endif //#if !defined(CETTY_SHIRO_SESSION_SESSIONMANAGER_H)
