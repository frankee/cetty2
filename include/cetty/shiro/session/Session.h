#if !defined(CETTY_SHIRO_SESSION_SESSION_H)
#define CETTY_SHIRO_SESSION_SESSION_H
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

#include <vector>

#include <boost/function.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/any.hpp>

#include <cetty/util/ReferenceCounter.h>
#include <cetty/shiro/session/SessionPtr.h>

namespace cetty {
namespace shiro {
namespace session {

using namespace boost::posix_time;
using namespace cetty::util;

/**
 * A {@code Session} is a stateful data context associated with a single Subject (user, daemon process,
 * etc) who interacts with a software system over a period of time.
 * <p/>
 * A {@code Session} is intended to be managed by the business tier and accessible via other
 * tiers without being tied to any given client technology.
 */
class Session : public ReferenceCounter<Session, int> {
public:
    enum SessionState {
        START,
        ACTIVE,
        STOPPED,
        EXPIRED
    };

    typedef boost::function1<void, const SessionPtr&> UpdateCallback;
    typedef boost::function1<void, const SessionPtr&> ExpireCallback;
    typedef boost::function1<void, const SessionPtr&> StopCallback;

public:
    Session();
    Session(const std::string& host);
    Session(const std::string& host,
            const StopCallback& stopCallback,
            const UpdateCallback& updateCallback,
            const ExpireCallback& expireCallback);

    /**
     * Returns the unique identifier assigned by the system upon session creation.
     * <p/>
     * All return values from this method are expected to have proper {@code toString()},
     * {@code equals()}, and {@code hashCode()} implementations. Good candidates for such
     * an identifier are {@link java.util.UUID UUID}s, {@link java.lang.Integer Integer}s, and
     * {@link java.lang.String String}s.
     *
     * @return The unique identifier assigned to the session upon creation.
     */
    const std::string& getId() const;
    void setId(const std::string& id);

    /**
     * Returns the time the session was started; that is, the time the system created the instance.
     *
     * @return The time the system created the session.
     */
    const ptime& getStartTimestamp() const;
    void setStartTimestamp(const ptime& startTimestamp);

    /**
     * Returns the time the session was stopped, or <tt>null</tt> if the session is still active.
     * <p/>
     * A session may become stopped under a number of conditions:
     * <ul>
     * <li>If the user logs out of the system, their current session is terminated (released).</li>
     * <li>If the session expires</li>
     * <li>The application explicitly calls {@link #stop()}</li>
     * <li>If there is an internal system error and the session state can no longer accurately
     * reflect the user's behavior, such in the case of a system crash</li>
     * </ul>
     * <p/>
     * Once stopped, a session may no longer be used.  It is locked from all further activity.
     *
     * @return The time the session was stopped, or <tt>null</tt> if the session is still
     *         active.
     */
    const ptime& getStopTimestamp() const;
    void setStopTimestamp(const ptime& stopTimestamp);

    /**
     * Returns the last time the application received a request or method invocation from the user associated
     * with this session.  Application calls to this method do not affect this access time.
     *
     * @return The time the user last interacted with the system.
     * @see #touch()
     */
    const ptime& getLastAccessTime() const;
    void setLastAccessTime(ptime& lastAccessTime);

    /**
     * Returns true if this session has expired, false otherwise.  If the session has
     * expired, no further user interaction with the system may be done under this session.
     *
     * @return true if this session has expired, false otherwise.
     */
    bool isExpired() const;

    bool isStopped() const;

    /**
     * Returns the time in milliseconds that the session session may remain idle before expiring.
     * <ul>
     * <li>A negative return value means the session will never expire.</li>
     * <li>A non-negative return value (0 or greater) means the session expiration will occur if idle for that
     * length of time.</li>
     * </ul>
     * <b>*Note:</b> if you are used to the {@code HttpSession}'s {@code getMaxInactiveInterval()} method, the scale on
     * this method is different: Shiro Sessions use millisecond values for timeout whereas
     * {@code HttpSession.getMaxInactiveInterval} uses seconds.  Always use millisecond values with Shiro sessions.
     *
     * @return the time in milliseconds the session may remain idle before expiring.
     * @throws InvalidSessionException if the session has been stopped or expired prior to calling this method.
     * @since 0.2
     */
    int getTimeout() const;

    /**
     * Sets the time in milliseconds that the session may remain idle before expiring.
     * <ul>
     * <li>A negative value means the session will never expire.</li>
     * <li>A non-negative value (0 or greater) means the session expiration will occur if idle for that
     * length of time.</li>
     * </ul>
     * <p/>
     * <b>*Note:</b> if you are used to the {@code HttpSession}'s {@code getMaxInactiveInterval()} method, the scale on
     * this method is different: Shiro Sessions use millisecond values for timeout whereas
     * {@code HttpSession.getMaxInactiveInterval} uses seconds.  Always use millisecond values with Shiro sessions.
     *
     * @param maxIdleTimeInMillis the time in milliseconds that the session may remain idle before expiring.
     * @throws InvalidSessionException if the session has been stopped or expired prior to calling this method.
     * @since 0.2
     */
    void setTimeout(int timeout);

    /**
     * Returns the host name or IP string of the host that originated this session, or {@code null}
     * if the host is unknown.
     *
     * @return the host name or IP string of the host that originated this session, or {@code null}
     *         if the host address is unknown.
     */
    const std::string& getHost() const;
    void setHost(const std::string& host);

    bool isLogin() const;
    void setLogin(bool login);

    bool isRememberMe();
    void setRememberMe(bool rememberMe);

    const std::map<std::string, std::string>& getAttributes() const;

    /**
     * Explicitly updates the {@link #getLastAccessTime() lastAccessTime} of this session to the current time when
     * this method is invoked.  This method can be used to ensure a session does not time out.
     * <p/>
     * Most programmers won't use this method directly and will instead rely on the last access time to be updated
     * automatically as a result of an incoming web request or remote procedure call/method invocation.
     * <p/>
     * However, this method is particularly useful when supporting rich-client applications such as
     * Java Web Start appp, Java or Flash applets, etc.  Although rare, it is possible in a rich-client
     * environment that a user continuously interacts with the client-side application without a
     * server-side method call ever being invoked.  If this happens over a long enough period of
     * time, the user's server-side session could time-out.  Again, such cases are rare since most
     * rich-clients frequently require server-side method invocations.
     * <p/>
     * In this example though, the user's session might still be considered valid because
     * the user is actively &quot;using&quot; the application, just not communicating with the
     * server. But because no server-side method calls are invoked, there is no way for the server
     * to know if the user is sitting idle or not, so it must assume so to maintain session
     * integrity.  This {@code touch()} method could be invoked by the rich-client application code during those
     * times to ensure that the next time a server-side method is invoked, the invocation will not
     * throw an {@link ExpiredSessionException ExpiredSessionException}.  In short terms, it could be used periodically
     * to ensure a session does not time out.
     * <p/>
     * How often this rich-client &quot;maintenance&quot; might occur is entirely dependent upon
     * the application and would be based on variables such as session timeout configuration,
     * usage characteristics of the client application, network utilization and application server
     * performance.
     *
     * @throws InvalidSessionException if this session has stopped or expired prior to calling this method.
     */
    void touch();

    /**
     * Explicitly stops (invalidates) this session and releases all associated resources.
     * <p/>
     * If this session has already been authenticated (i.e. the {@code Subject} that
     * owns this session has logged-in), calling this method explicitly might have undesired side effects:
     * <p/>
     * It is common for a {@code Subject} implementation to retain authentication state in the
     * {@code Session}.  If the session
     * is explicitly stopped by application code by calling this method directly, it could clear out any
     * authentication state that might exist, thereby effectively &quot;unauthenticating&quot; the {@code Subject}.
     * <p/>
     * As such, you might consider {@link org.apache.shiro.subject.Subject#logout logging-out} the 'owning'
     * {@code Subject} instead of manually calling this method, as a log out is expected to stop the
     * corresponding session automatically, and also allows framework code to execute additional cleanup logic.
     *
     * @throws InvalidSessionException if this session has stopped or expired prior to calling this method.
     */
    void stop();

    void expire();

    /**
     * Returns the keys of all the attributes stored under this session.  If there are no
     * attributes, this returns an empty collection.
     *
     * @return the keys of all attributes stored under this session, or an empty collection if
     *         there are no session attributes.
     * @throws InvalidSessionException if this session has stopped or expired prior to calling this method.
     * @since 0.2
     */
    void getAttributeKeys(std::vector<std::string>* keys) {}

    /**
     * Returns the object bound to this session identified by the specified key.  If there is no
     * object bound under the key, {@code null} is returned.
     *
     * @param key the unique name of the object bound to this session
     * @return the object bound under the specified {@code key} name or {@code null} if there is
     *         no object bound under that name.
     * @throws InvalidSessionException if this session has stopped or expired prior to calling
     *                                 this method.
     */
    const std::string& getAttribute(const std::string& key) const;

    /**
     * Binds the specified {@code value} to this session, uniquely identified by the specifed
     * {@code key} name.  If there is already an object bound under the {@code key} name, that
     * existing object will be replaced by the new {@code value}.
     * <p/>
     * If the {@code value} parameter is null, it has the same effect as if
     * {@link #removeAttribute(Object) removeAttribute} was called.
     *
     * @param key   the name under which the {@code value} object will be bound in this session
     * @param value the object to bind in this session.
     * @throws InvalidSessionException if this session has stopped or expired prior to calling
     *                                 this method.
     */
    void setAttribute(const std::string& key, const std::string& value);

    /**
     * Removes (unbinds) the object bound to this session under the specified {@code key} name.
     *
     * @param key the name uniquely identifying the object to remove
     * @return the object removed or {@code null} if there was no object bound under the name
     *         {@code key}.
     * @throws InvalidSessionException if this session has stopped or expired prior to calling
     *                                 this method.
     */
    void removeAttribute(const std::string& key);

    void setStopCallback(StopCallback callback) {
        this->stopCallback = callback;
    }
    void setUpdateCallback(UpdateCallback callback){
        this->updateCallback = callback;
    }
    void setExpireCallback(ExpireCallback callback){
        this->expireCallback = callback;
    }

    SessionState validate();

    void toJson(std::string* json) const;
    void fromJson(const char* json);

private:
    /**
     * Determines if this session is expired.
     *
     * @return true if the specified session has expired, false otherwise.
     */
    bool isTimedOut() const;
    bool isValid() const;

    void doStop();
    void doExpire();

private:
    const static std::string SESSION_SELF;
    const static std::string NULL_TIME;

    bool expired;
    bool login;
    bool rememberMe;

    int timeout;

    std::string id;
    std::string host;
    ptime startTime;
    ptime stopTime;
    ptime lastAccessTime;

    std::map<std::string, std::string> attributes;

    StopCallback stopCallback;
    UpdateCallback updateCallback;
    ExpireCallback expireCallback;
};

inline
const std::string& Session::getId() const {
    return this->id;
}

inline
const ptime& Session::getStartTimestamp() const {
    return startTime;
}

inline
const ptime& Session::getStopTimestamp() const {
    return stopTime;
}

inline
const ptime& Session::getLastAccessTime() const {
    return lastAccessTime;
}

inline
bool Session::isExpired() const {
    return expired;
}

inline
int Session::getTimeout() const {
    return timeout;
}

inline
const std::string& Session::getHost() const {
    return host;
}

inline
bool Session::isLogin() const {
    return login;
}

inline
const std::map<std::string, std::string>& Session::getAttributes() const {
    return attributes;
}

inline
bool Session::isRememberMe(){
    return rememberMe;
}

inline
void Session::setRememberMe(bool rememberMe){
    this->rememberMe = true;
    if(updateCallback) updateCallback(shared_from_this());
}


}
}
}
#endif // #if !defined(CETTY_SHIRO_SESSION_SESSION_H)
