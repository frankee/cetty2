#if !defined(CETTY_SHIRO_AUTHENTICATION_TOKEN_H)
#define CETTY_SHIRO_AUTHENTICATION_TOKEN_H

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

#include <string>

namespace cetty {
namespace shiro {
namespace authc {

/**
 * <p>An <tt>AuthenticationToken</tt> is a consolidation of an account's principals and supporting
 * credentials submitted by a user during an authentication attempt.
 * <p/>
 * <p>The token is submitted to an {@link Authenticator Authenticator} via the
 * {@link Authenticator#authenticate(AuthenticationToken) authenticate(token)} method.  The
 * Authenticator then executes the authentication/log-in process.
 * <p/>
 * <p><tt>AuthenticationToken</tt> would have username/password
 * pairs, X.509 Certificate, PGP key, or anything else you can think of.  The token can be
 * anything needed by an {@link Authenticator} to authenticate properly.
 * <p/>
 * <p>Because applications represent user data and credentials in different ways, implementations
 * of this interface are application-specific.  You are free to acquire a user's principals and
 * credentials however you wish (e.g. web form, Swing form, fingerprint identification, etc) and
 * then submit them to the Shiro framework in the form of an implementation of this
 * interface.
 * <p/>
 */
class AuthenticationToken {
public:

    AuthenticationToken() {
        init("", "", "", false);
}

    /**
     * Constructs a new AuthenticationToken encapsulating the principal and credential submitted
     * during an authentication attempt, with a <tt>null</tt> {@link #getHost() host} and a
     * <tt>rememberMe</tt> default of <tt>false</tt>.
     *
     * @param principal the principal submitted for authentication
     * @param credential the credential character array submitted for authentication
     */
    AuthenticationToken(const std::string &principal, const std::string &credential) {
        init(principal, credential, std::string(), false);
    }

    /**
     * Constructs a new AuthenticationToken encapsulating the principal and credential submitted, the
     * inetAddress from where the attempt is occurring, and a default <tt>rememberMe</tt> value of <tt>false</tt>
     *
     * @param principal the principal submitted for authentication
     * @param credential the credential character array submitted for authentication
     * @param host     the host name or IP string from where the attempt is occuring
     * @since 0.2
     */
    AuthenticationToken(const std::string &principal,
        const std::string &credential,
        const std::string& host) {
        init(principal, credential, host, false);
    }

    /**
     * Constructs a new AuthenticationToken encapsulating the principal and credential submitted, as well as if the user
     * wishes their identity to be remembered across sessions.
     *
     * @param principal the principal submitted for authentication
     * @param credential the credential character array submitted for authentication
     * @param rememberMe if the user wishes their identity to be remembered across sessions
     * @since 0.9
     */
    AuthenticationToken(const std::string &principal,
        const std::string &credential,
        bool rememberMe) {
        init(principal, credential, std::string(), rememberMe);
    }

    /**
     * Constructs a new AuthenticationToken encapsulating the principal and credential submitted, if the user
     * wishes their identity to be remembered across sessions, and the inetAddress from where the attempt is ocurring.
     *
     * @param principal the principal submitted for authentication
     * @param credential the credential character array submitted for authentication
     * @param rememberMe if the user wishes their identity to be remembered across sessions
     * @param host       the host name or IP string from where the attempt is occuring
     * @since 1.0
     */
    AuthenticationToken(const std::string &principal,
        const std::string &credential,
        const std::string& host,
        bool rememberMe) {
        init(principal, credential, host, rememberMe);
    }


    void init(const std::string &principal, const std::string& credential, const std::string& host, bool rememberMe){}

    /*--------------------------------------------
    |  A C C E S S O R S / M O D I F I E R S    |
    ============================================*/

    const std::string& getPrincipal() const { return principal; }
    const std::string& getCredentials() const { return credential; }

    /**
     * Returns the host name or IP string from where the authentication attempt occurs.  May be <tt>""</tt> if the
     * host name/IP is unknown or explicitly omitted.  It is up to the Authenticator implementation processing this
     * token if an authentication attempt without a host is valid or not.
     * <p/>
     * <p>(Shiro's default Authenticator allows <tt>null</tt> hosts to support localhost and proxy server environments).</p>
     *
     * @return the host from where the authentication attempt occurs, or <tt>""</tt> if it is unknown or
     *         explicitly omitted.
     * @since 1.0
     */
    const std::string &getHost() const{
        return host;
    }

    /**
     * Sets the host name or IP string from where the authentication attempt occurs.  It is up to the Authenticator
     * implementation processing this token if an authentication attempt without a host is valid or not.
     * <p/>
     * <p>(Shiro's default Authenticator allows <tt>null</tt> hosts to allow localhost and proxy server environments).</p>
     *
     * @param host the host name or IP string from where the attempt is occuring
     * @since 1.0
     */
    void setHost(const std::string &host) {
        this->host = host;
    }

    /**
     * Returns <tt>true</tt> if the submitting user wishes their identity (principal(s)) to be remembered
     * across sessions, <tt>false</tt> otherwise.  Unless overridden, this value is <tt>false</tt> by default.
     *
     * @return <tt>true</tt> if the submitting user wishes their identity (principal(s)) to be remembered
     *         across sessions, <tt>false</tt> otherwise (<tt>false</tt> by default).
     * @since 0.9
     */
    bool isRememberMe() {
        return rememberMe;
    }

    /**
     * Sets if the submitting user wishes their identity (pricipal(s)) to be remembered across sessions.  Unless
     * overridden, the default value is <tt>false</tt>, indicating <em>not</em> to be remembered across sessions.
     *
     * @param rememberMe value inidicating if the user wishes their identity (principal(s)) to be remembered across
     *                   sessions.
     * @since 0.9
     */
    void setRememberMe(bool rememberMe) {
        this->rememberMe = rememberMe;
    }

    /*--------------------------------------------
    |               M E T H O D S               |
    ============================================*/

    /**
     * Clears out the username, password, rememberMe, and inetAddress.
     */
    void clear();

    /**
     * Returns the String representation.  It does not include the credentials in the resulting
     * string for security reasons to prevent accidentially printing out a password
     * that might be widely viewable).
     *
     * @return the String representation of the <tt>AuthenticationToken</tt>, omitting
     *         the password.
     */
    std::string toString();

private:
    std::string principal;
    std::string credential;

    /**
     * Whether or not 'rememberMe' should be enabled for the corresponding login attempt;
     * default is <code>false</code>
     */
    bool rememberMe;

    /**
     * The location from where the login attempt occurs, or <code>null</code> if not known or explicitly
     * omitted.
     */
    std::string host;
};

}
}
}

#endif // #if !defined(CETTY_SHIRO_AUTHENTICATION_TOKEN_H)
