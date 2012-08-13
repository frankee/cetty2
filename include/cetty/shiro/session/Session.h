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

#include <boost/date_time/posix_time/ptime.hpp>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace shiro {
namespace session {

using namespace boost::posix_time;

/**
 * A {@code Session} is a stateful data context associated with a single Subject (user, daemon process,
 * etc) who interacts with a software system over a period of time.
 * <p/>
 * A {@code Session} is intended to be managed by the business tier and accessible via other
 * tiers without being tied to any given client technology.  This is a <em>great</em> benefit to Java
 * systems, since until now, the only viable session mechanisms were the
 * {@code javax.servlet.http.HttpSession} or Stateful Session EJB's, which many times
 * unnecessarily coupled applications to web or ejb technologies.
 *
 * @since 0.1
 */
class Session : public cetty::util::ReferenceCounter<Session, int> {
protected:
    static const long MILLIS_PER_SECOND = 1000;
    static const long MILLIS_PER_MINUTE = 60 * MILLIS_PER_SECOND;
    static const long MILLIS_PER_HOUR = 60 * MILLIS_PER_MINUTE;

public:
    Session() { init(); }
    Session(const std::string& host) {
        init();
        this->host = host;
    }

    const std::string& getId() const {
        return this->id;
    }

    void setId(const std::string& id) {
        this->id = id;
    }

    const ptime& getStartTimestamp() const {
        return startTimestamp;
    }

    void setStartTimestamp(const ptime& startTimestamp) {
        this->startTimestamp = startTimestamp;
    }

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
    time_t getStopTimestamp() const {
        return stopTimestamp;
    }

    void setStopTimestamp(time_t stopTimestamp) {
        this->stopTimestamp = stopTimestamp;
    }

    time_t getLastAccessTime() const {
        return lastAccessTime;
    }

    void setLastAccessTime(time_t lastAccessTime) {
        this->lastAccessTime = lastAccessTime;
    }

    /**
     * Returns true if this session has expired, false otherwise.  If the session has
     * expired, no further user interaction with the system may be done under this session.
     *
     * @return true if this session has expired, false otherwise.
     */
    bool isExpired() {
        return expired;
    }

    void setExpired(bool expired) {
        this->expired = expired;
    }

    long getTimeout() {
        return timeout;
    }

    void setTimeout(long timeout) {
        this->timeout = timeout;
    }

    const std::string& getHost() const {
        return host;
    }

    void setHost(const std::string& host) {
        this->host = host;
    }

    const std::map<std::string, std::string>& getAttributes() const {
        return attributes;
    }

    void setAttributes(const std::map<std::string, std::string>& attributes) {
        this->attributes.insert(attributes.begin(), attributes.end());
    }

    void touch() {
        this->lastAccessTime = time(NULL);
    }

    void stop() {
        if (this->stopTimestamp == 0) {
            this->stopTimestamp = time(NULL);
        }
    }

protected:
    bool isStopped() {
        return getStopTimestamp() != 0;
    }

    void expire() {
        stop();
        this->expired = true;
    }

    /**
     * @since 0.9
     */
    bool isValid() {
        return !isStopped() && !isExpired();
    }

    /**
     * Determines if this session is expired.
     *
     * @return true if the specified session has expired, false otherwise.
     */
    bool isTimedOut() {

        if (isExpired()) {
            return true;
        }

        long timeout = getTimeout();

        if (timeout >= 0l) {

            time_t lastAccessTime = getLastAccessTime();

            if (lastAccessTime <= 0) {
                return true;
            }

            time_t diff = time(NULL) - lastAccessTime;

            if (diff >= (timeout / 1000)) { return true; }
            else { return false; }

        }

        return false;
    }

    void validate();

private:
    const std::map<std::string, std::string>& getAttributesLazy() {
        return getAttributes();
    }

public:
    const std::set<std::string>& getAttributeKeys();
    std::string getAttribute(const std::string& key) {
        std::map<std::string, std::string>::iterator it = attributes.find(key);

        if (it == attributes.end()) { return std::string(); }

        return it->second;
    }

    void setAttribute(std::string, std::string);

    std::string removeAttribute(const std::string& key);

    /**
     * Returns {@code true} if the specified argument is an {@code instanceof} {@code SimpleSession} and both
     * {@link #getId() id}s are equal.  If the argument is a {@code SimpleSession} and either 'this' or the argument
     * does not yet have an ID assigned, the value of {@link #onEquals(SimpleSession) onEquals} is returned, which
     * does a necessary attribute-based comparison when IDs are not available.
     * <p/>
     * Do your best to ensure {@code SimpleSession} instances receive an ID very early in their lifecycle to
     * avoid the more expensive attributes-based comparison.
     *
     * @param obj the object to compare with this one for equality.
     * @return {@code true} if this object is equivalent to the specified argument, {@code false} otherwise.
     */
    bool equals(const Session& session);

protected:
    /**
     * Provides an attribute-based comparison (no ID comparison) - incurred <em>only</em> when 'this' or the
     * session object being compared for equality do not have a session id.
     *
     * @param ss the SimpleSession instance to compare for equality.
     * @return true if all the attributes, except the id, are equal to this object's attributes.
     * @since 1.0
     */
    bool onEquals(const Session& ss);

public:
    /**
     * Returns the hashCode.  If the {@link #getId() id} is not {@code null}, its hashcode is returned immediately.
     * If it is {@code null}, an attributes-based hashCode will be calculated and returned.
     * <p/>
     * Do your best to ensure {@code SimpleSession} instances receive an ID very early in their lifecycle to
     * avoid the more expensive attributes-based calculation.
     *
     * @return this object's hashCode
     * @since 1.0
     */
    int hashCode();

    /**
     * Returns the string representation of this SimpleSession, equal to
     * <code>getClass().getName() + &quot;,id=&quot; + getId()</code>.
     *
     * @return the string representation of this SimpleSession, equal to
     *         <code>getClass().getName() + &quot;,id=&quot; + getId()</code>.
     * @since 1.0
     */
    std::string toString() { return std::string(); }

private:
    std::string id;
    time_t startTimestamp;
    time_t stopTimestamp;
    time_t lastAccessTime;
    long timeout;
    bool expired;
    std::string host;

    std::map<std::string, std::string> attributes;

    void init() {
        this->timeout = SessionManager::DEFAULT_GLOBAL_SESSION_TIMEOUT;
        this->startTimestamp = time(NULL);
        this->lastAccessTime = this->startTimestamp;
    }


};
}
}
}
#endif // #if !defined(CETTY_SHIRO_SESSION_SESSION_H)
