#if !defined(CETTY_SHIRO_AUTHC_AUTHENTICATION_TOKEN_H)
#define CETTY_SHIRO_AUTHC_AUTHENTICATION_TOKEN_H

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
    AuthenticationToken(): rememberMe(false) {}

    /**
     * Constructs a new AuthenticationToken encapsulating the principal and credential submitted
     * during an authentication attempt, with a <tt>null</tt> {@link #getHost() host} and a
     * <tt>rememberMe</tt> default of <tt>false</tt>.
     *
     * @param principal the principal submitted for authentication
     * @param credential the credential character array submitted for authentication
     */
    AuthenticationToken(const std::string& principal,
                        const std::string& credentials)
        : rememberMe(false), principal(principal), credentials(credentials) {
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
    AuthenticationToken(const std::string& principal,
                        const std::string& credentials,
                        const std::string& host)
        : rememberMe(false),
          principal(principal),
          credentials(credentials),
          host(host) {
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
    AuthenticationToken(const std::string& principal,
                        const std::string& credentials,
                        bool rememberMe)
        : rememberMe(rememberMe),
          principal(principal),
          credentials(credentials) {
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
    AuthenticationToken(const std::string& principal,
                        const std::string& credentials,
                        const std::string& host,
                        bool rememberMe)
        : rememberMe(rememberMe),
          principal(principal),
          credentials(credentials),
          host(host) {
    }

    AuthenticationToken(const AuthenticationToken& token) {
        this->principal = token.principal;
        this->credentials = token.credentials;
        this->rememberMe = token.rememberMe;
        this->host = token.host;
    }

    AuthenticationToken& operator=(const AuthenticationToken& token) {
        this->principal = token.principal;
        this->credentials = token.credentials;
        this->rememberMe = token.rememberMe;
        this->host = token.host;

        return *this;
    }

    /**
     * Returns the account identity submitted during the authentication process.
     * <p/>
     * <p>Most application authentications are username/password based and have this
     * object represent a username.  If this is the case for your application,
     * take a look at the {@link UsernamePasswordToken UsernamePasswordToken}, as it is probably
     * sufficient for your use.
     * <p/>
     * <p>Ultimately, the object returned is application specific and can represent
     * any account identity (user id, X.509 certificate, etc).
     *
     * @return the account identity submitted during the authentication process.
     * @see UsernamePasswordToken
     */
    const std::string& getPrincipal() const { return principal; }

    /**
     * Sets the principal for submission during an authentication attempt.
     *
     * @param principal the principal to be used for submission during an authentication attempt.
     */
    void setPrincipal(const std::string& principal) { this->principal = principal; }

    /**
     * Returns the credentials submitted by the user during the authentication process that verifies
     * the submitted {@link #getPrincipal() account identity}.
     * <p/>
     * <p>Most application authentications are username/password based and have this object
     * represent a submitted password.  If this is the case for your application,
     * take a look at the {@link UsernamePasswordToken UsernamePasswordToken}, as it is probably
     * sufficient for your use.
     * <p/>
     * <p>Ultimately, the credentials Object returned is application specific and can represent
     * any credential mechanism.
     *
     * @return the credential submitted by the user during the authentication process.
     */
    const std::string& getCredentials() const { return credentials; }

    /**
     * Sets the credentials for submission during an authentication attempt.
     *
     * @param credentials the credentials to be used for submission during an authentication attemp.
     */
    void setCredentials(const std::string& credentials) { this->credentials = credentials; }

    /**
     * Returns the host name of the client from where the
     * authentication attempt originates or if the Shiro environment cannot or
     * chooses not to resolve the hostname to improve performance, this method
     * returns the String representation of the client's IP address.
     * <p/>
     * When used in web environments, this value is usually the same as the
     * {@code ServletRequest.getRemoteHost()} value.
     *
     * @return the fully qualified name of the client from where the
     *         authentication attempt originates or the String representation
     *         of the client's IP address is hostname resolution is not
     *         available or disabled.
     */
    const std::string& getHost() const { return host; }

    /**
     * Sets the host name or IP string from where the authentication attempt occurs.  It is up to the Authenticator
     * implementation processing this token if an authentication attempt without a host is valid or not.
     * <p/>
     * <p>(Shiro's default Authenticator
     * allows <tt>null</tt> hosts to allow localhost and proxy server environments).</p>
     *
     * @param host the host name or IP string from where the attempt is occuring
     * @since 1.0
     */
    void setHost(const std::string& host) { this->host = host; }

    /**
     * Returns {@code true} if the submitting user wishes their identity (principal(s)) to be remembered
     * across sessions, {@code false} otherwise.
     *
     * @return {@code true} if the submitting user wishes their identity (principal(s)) to be remembered
     *         across sessions, {@code false} otherwise.
     */
    bool isRememberMe() const { return rememberMe; }

    /**
     * Sets if the submitting user wishes their identity (pricipal(s)) to be remembered across sessions.  Unless
     * overridden, the default value is <tt>false</tt>, indicating <em>not</em> to be remembered across sessions.
     *
     * @param rememberMe value inidicating if the user wishes their identity (principal(s)) to be remembered across
     *                   sessions.
     * @since 0.9
     */
    void setRememberMe(bool rememberMe) { this->rememberMe = rememberMe; }

    void setNonce(const std::string& nonce) { this->nonce = nonce; }
    const std::string& getNonce() const { return nonce; }

    void setSeries(const std::string& series) { this->series = series; }
    const std::string& getSeries() const { return series; }

    /**
     * Clears out the principal, credentials, rememberMe, and inetAddress.
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
    /**
     * Whether or not 'rememberMe' should be enabled for the corresponding login attempt;
     * default is <code>false</code>
     */
    bool rememberMe;

    std::string principal;
    std::string credentials;

    std::string nonce;
    std::string series;

    /**
     * The location from where the login attempt occurs, or <code>null</code> if not known or explicitly
     * omitted.
     */
    std::string host;
};

}
}
}

#endif // #if !defined(CETTY_SHIRO_AUTHC_AUTHENTICATION_TOKEN_H)
