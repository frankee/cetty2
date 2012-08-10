#if !defined(CETTY_SHIRO_SESSION_DELEGATINGSESSION_H)
#define CETTY_SHIRO_SESSION_DELEGATINGSESSION_H
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

namespace cetty {
namespace shiro {
namespace session {

/**
 * A DelegatingSession is a client-tier representation of a server side
 * {@link org.apache.shiro.session.Session Session}.
 * This implementation is basically a proxy to a server-side {@link NativeSessionManager NativeSessionManager},
 * which will return the proper results for each method call.
 * <p/>
 * <p>A <tt>DelegatingSession</tt> will cache data when appropriate to avoid a remote method invocation,
 * only communicating with the server when necessary.
 * <p/>
 * <p>Of course, if used in-process with a NativeSessionManager business POJO, as might be the case in a
 * web-based application where the web classes and server-side business pojos exist in the same
 * JVM, a remote method call will not be incurred.
 *
 * @since 0.1
 */
class DelegatingSession {
public:
    DelegatingSession(const  SessionManager &sessionManager, const SessionKey &key)
      :sessionManager(sessionManager), key(key), startTimestamp(0){}

    /**
     * @see org.apache.shiro.session.Session#getId()
     */
    const std::string &getId() {
        return key.getSessionId();
    }

    /**
     * @see org.apache.shiro.session.Session#getStartTimestamp()
     */
    time_t getStartTimestamp() {
        if (startTimestamp == 0) {
            startTimestamp = sessionManager.getStartTimestamp(key);
        }
        return startTimestamp;
    }

    /**
     * @see org.apache.shiro.session.Session#getLastAccessTime()
     */
    time_t getLastAccessTime() {
        //can't cache - only business pojo knows the accurate time:
        return sessionManager.getLastAccessTime(key);
    }

    long getTimeout() {
        return sessionManager.getTimeout(key);
    }

    void setTimeout(long maxIdleTimeInMillis){
        sessionManager.setTimeout(key, maxIdleTimeInMillis);
    }

    std::string getHost() {
        if (host == "") {
            host = sessionManager.getHost(key);
        }
        return host;
    }

    /**
     * @see org.apache.shiro.session.Session#touch()
     */
    void touch(){
        sessionManager.touch(key);
    }

    /**
     * @see org.apache.shiro.session.Session#stop()
     */
    void stop(){
        sessionManager.stop(key);
    }

    /**
     * @see org.apache.shiro.session.Session#getAttributeKeys
     */
    std::vector<std::string> *getAttributeKeys(){
        return sessionManager.getAttributeKeys(key);
    }

    /**
     * @see org.apache.shiro.session.Session#getAttribute(Object key)
     */
    const std::string &getAttribute(const std::string &attributeKey){
        return sessionManager.getAttribute(key, attributeKey);
    }

    /**
     * @see Session#setAttribute(Object key, Object value)
     */
    void setAttribute(std::string attributeKey, std::string value){
        if (value == "") {
            removeAttribute(attributeKey);
        } else {
            sessionManager.setAttribute(key, attributeKey, value);
        }
    }

    /**
     * @see Session#removeAttribute(Object key)
     */
    std::string removeAttribute(const std::string &attributeKey){
        return sessionManager.removeAttribute(key, attributeKey);
    }

private:
    SessionKey key;

    //cached fields to avoid a server-side method call if out-of-process:
    time_t startTimestamp;
    std::string host;

    /**
     * Handle to the target NativeSessionManager that will support the delegate calls.
     */
    SessionManager &sessionManager;
};
}
}
}
#endif // #if !defined(CETTY_SHIRO_SESSION_DELEGATINGSESSION_H)
