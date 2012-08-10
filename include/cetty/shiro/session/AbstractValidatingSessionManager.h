#if !defined(CETTY_SHIRO_SESSION_ABSTRACTVALIDATINGSESSIONMANAGER_H)
#define CETTY_SHIRO_SESSION_ABSTRACTVALIDATINGSESSIONMANAGER_H

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

namespace cetty { namespace shiro { namespace session {
    class SessionValidationScheduler;
}}}

namespace cetty {
namespace shiro {
namespace session {

/**
 * Default business-tier implementation of the {@link ValidatingSessionManager} interface.
 *
 * @since 0.1
 */
class AbstractValidatingSessionManager : public AbstractNativeSessionManager{
    /**
     * The default interval at which sessions will be validated (1 hour);
     * This can be overridden by calling {@link #setSessionValidationInterval(long)}
     */
public:
    static const long DEFAULT_SESSION_VALIDATION_INTERVAL = MILLIS_PER_HOUR;

protected:
    bool sessionValidationSchedulerEnabled;

    /**
     * Scheduler used to validate sessions on a regular basis.
     */
    SessionValidationScheduler *sessionValidationScheduler;

    long sessionValidationInterval;

public:
    AbstractValidatingSessionManager()
      : sessionValidationSchedulerEnabled(true),
        sessionValidationInterval(DEFAULT_SESSION_VALIDATION_INTERVAL),
        sessionValidationScheduler(NULL){}

    bool isSessionValidationSchedulerEnabled() {
        return sessionValidationSchedulerEnabled;
    }

    void setSessionValidationSchedulerEnabled(bool sessionValidationSchedulerEnabled) {
        this->sessionValidationSchedulerEnabled = sessionValidationSchedulerEnabled;
    }

    void setSessionValidationScheduler(const SessionValidationScheduler &sessionValidationScheduler) {
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
    void setSessionValidationInterval(long sessionValidationInterval) {
        this->sessionValidationInterval = sessionValidationInterval;
    }

    long getSessionValidationInterval() {
        return sessionValidationInterval;
    }

    /**
        * Subclass template hook in case per-session timeout is not based on
        * {@link org.apache.shiro.session.Session#getTimeout()}.
        * <p/>
        * <p>This implementation merely returns {@link org.apache.shiro.session.Session#getTimeout()}</p>
        *
        * @param session the session for which to determine session timeout.
        * @return the time in milliseconds the specified session may remain idle before expiring.
        */
        long getTimeout(Session *session) {
           return session->getTimeout();
       }

protected:
    Session *doGetSession(const SessionKey key){
        enableSessionValidationIfNecessary();

        Session *s = retrieveSession(key);
        if (s != NULL) {
            validate(s, key);
        }
        return s;
    }

    /**
     * Looks up a session from the underlying data store based on the specified session key.
     *
     * @param key the session key to use to look up the target session.
     * @return the session identified by {@code sessionId}.
     * @throws UnknownSessionException if there is no session identified by {@code sessionId}.
     */
    virtual Session *retrieveSession(SessionKey key);

    Session *createSession(SessionContext context) {
        enableSessionValidationIfNecessary();
        return doCreateSession(context);
    }

    virtual Session *doCreateSession(SessionContext initData);

    void validate(Session *session, SessionKey key);

    void onExpiration(Session *s, SessionKey key) {
        onExpiration(s);
        notifyExpiration(s);
        afterExpired(s);
    }

   void onExpiration(Session *session) {
        onChange(session);
    }

    void afterExpired(Session *session) {}

    void onInvalidation(Session *s, /*InvalidSessionException ise,*/ SessionKey key);

    void doValidate(Session *session) { session->validate(); }

    SessionValidationScheduler *createSessionValidationScheduler();

    void enableSessionValidation();

    void afterSessionValidationEnabled() {}

    void disableSessionValidation();

    void beforeSessionValidationDisabled() {}

public:
    void destroy() {
        disableSessionValidation();
    }

    /**
     * @see ValidatingSessionManager#validateSessions()
     */
    void validateSessions();

protected:
    virtual std::map<std::string, Session> getActiveSessions();
};
}
}
}
#endif // #if !defined(CETTY_SHIRO_SESSION_ABSTRACTVALIDATINGSESSIONMANAGER_H)
