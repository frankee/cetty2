#if !defined(CETTY_SHIRO_SESSION_ABSTRACTNATIVESESSIONMANAGER_H)
#define CETTY_SHIRO_SESSION_ABSTRACTNATIVESESSIONMANAGER_H
/*
 * Copyright 2008 Les Hazlewood
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cstdlib>

#include <cetty/shiro/session/DelegatingSession.h>
#include <cetty/shiro/session/SessionListener.h>

namespace cetty {
namespace shiro {
namespace session {
/**
 * Abstract implementation supporting the {@link NativeSessionManager NativeSessionManager} interface, supporting
 * {@link SessionListener SessionListener}s and application of the
 * {@link #getGlobalSessionTimeout() globalSessionTimeout}.
 *
 * @since 1.0
 */
class AbstractNativeSessionManager : public AbstractSessionManager{
public:
    AbstractNativeSessionManager() {}

    void setSessionListeners(const std::vector<SessionListener> &listeners) {
        if(!listeners.empty())
            this->listeners.assign(listeners.begin(), listeners.end());
    }

    const std::vector<SessionListener> &getSessionListeners() const{
        return this->listeners;
    }

protected:
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
    virtual Session *createSession(SessionContext context);

    void applyGlobalSessionTimeout(Session *session) {
        session->setTimeout(getGlobalSessionTimeout());
        onChange(session);
    }

    /**
     * Template method that allows subclasses to react to a new session being created.
     * <p/>
     * This method is invoked <em>before</em> any session listeners are notified.
     *
     * @param session the session that was just {@link #createSession created}.
     * @param context the {@link SessionContext SessionContext} that was used to start the session.
     */
    void onStart(Session *session, SessionContext context) {}

public:

protected:
    virtual Session* doGetSession(SessionKey key);

    /**
     * Notifies any interested {@link SessionListener}s that a Session has started.  This method is invoked
     * <em>after</em> the {@link #onStart onStart} method is called.
     *
     * @param session the session that has just started that will be delivered to any
     *                {@link #setSessionListeners(java.util.Collection) registered} session listeners.
     * @see SessionListener#onStart(org.apache.shiro.session.Session)
     */
    void notifyStart(const Session *session) {
        for (SessionListener listener : this->listeners) {
            listener.onStart(session);
        }
    }

    void notifyStop(const Session *session) {
        for (SessionListener listener : this->listeners) {
            listener.onStop(session);
        }
    }

    void notifyExpiration(const Session *session) {
        for (SessionListener listener : this->listeners) {
            listener.onExpiration(session);
        }
    }

public:
    time_t getStartTimestamp(const SessionKey &key) {
        Session *s = lookupRequiredSession(key);
        if(s == NULL) return 0L;
        return s->getStartTimestamp();
    }

    time_t getLastAccessTime(const SessionKey &key) {
        Session *s = lookupRequiredSession(key);
        if(s == NULL) return 0;
        return s->getLastAccessTime();
    }

    long getTimeout(const SessionKey &key){
        Session *s = lookupRequiredSession(key);
        if(s == NULL) return -1L;
        return s->getTimeout();
    }

    void setTimeout(const SessionKey &key, long maxIdleTimeInMillis){
        Session *s = lookupRequiredSession(key);
        if(s != NULL){
            s->setTimeout(maxIdleTimeInMillis);
            onChange(s);
        }
    }

    void touch(const SessionKey &key){
        Session *s = lookupRequiredSession(key);
        if(s != NULL){
            s->touch();
            onChange(s);
        }
    }

    std::string getHost(const SessionKey &key) {
        Session *s = lookupRequiredSession(key);
        if(s == NULL) return std::string();
        return s->getHost();
    }

    std::vector<std::string> *getAttributeKeys(const SessionKey &key) {
        Session *s = lookupRequiredSession(key);
        if(s == NULL) return NULL;
        return s->getAttributeKeys();
    }

    std::string getAttribute(SessionKey sessionKey, std::string key){
        Session *s = lookupRequiredSession(key);
        if(s == NULL) return std::string();
        return s->getAttribute(key);
    }

    void setAttribute(SessionKey sessionKey, std::string key, std::string value) {
        if (value == "") {
            removeAttribute(sessionKey, key);
        } else {
            Session *s = lookupRequiredSession(sessionKey);
            s->setAttribute(key, value);
            onChange(s);
        }
    }

    std::string removeAttribute(SessionKey sessionKey, std::string attributeKey) {
        Session *s = lookupRequiredSession(sessionKey);
        if(s == NULL) return std::string();
        std::string removed = s->removeAttribute(sessionKey.getSessionId());
        if (removed.size() > 0) {
            onChange(s);
        }
        return removed;
    }

    bool isValid(SessionKey key) {
        return checkValid(key);
    }

    void stop(SessionKey key){
        Session *s = lookupRequiredSession(key);

        s->stop();
        onStop(s, key);
        notifyStop(s);
        afterStopped(s);
    }
    bool checkValid(SessionKey key) {
        return (lookupRequiredSession(key) != NULL);
    }

    virtual ~AbstractNativeSessionManager(){};

protected:
    void onStop(Session *session, SessionKey key) {
        onStop(session);
    }

    void onStop(Session *session) {
        onChange(session);
    }

    void afterStopped(Session *session) {}
    void onChange(Session *s) {}

private:
    std::vector<SessionListener> listeners;

    Session *lookupSession(SessionKey key){
        if (key.getSessionId() == "") {
            return NULL;
        }
        return doGetSession(key);
    }

    Session *lookupRequiredSession(SessionKey key){
        Session *session = lookupSession(key);
        return session;
    }
};
}
}
}
#endif // #if !defined(CETTY_SHIRO_SESSION_ABSTRACTNATIVESESSIONMANAGER_H)
