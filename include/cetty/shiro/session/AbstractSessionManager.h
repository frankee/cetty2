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

/**
 * Base abstract class of the {@link SessionManager SessionManager} interface, enabling configuration of an
 * application-wide {@link #getGlobalSessionTimeout() globalSessionTimeout}.  Default global session timeout is
 * {@code 30} minutes.
 *
 * @since 0.1
 */
class AbstractSessionManager {

protected:
    static const long MILLIS_PER_SECOND = 1000;
    static const long MILLIS_PER_MINUTE = 60 * MILLIS_PER_SECOND;
    static const long MILLIS_PER_HOUR = 60 * MILLIS_PER_MINUTE;

public:
    /**
     * Default main session timeout value, equal to {@code 30} minutes.
     */
    static const long DEFAULT_GLOBAL_SESSION_TIMEOUT = 30 * MILLIS_PER_MINUTE;

private:
    long globalSessionTimeout = DEFAULT_GLOBAL_SESSION_TIMEOUT;

public:
    AbstractSessionManager() {}

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
    long getGlobalSessionTimeout() {
        return this->globalSessionTimeout;
    }

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
    void setGlobalSessionTimeout(long globalSessionTimeout) {
        this->globalSessionTimeout = globalSessionTimeout;
    }
};
}
}
