#if !defined(CETTY_SHIRO_AUTHC_ABSTRACTAUTHENTICATOR_H)
#define CETTY_SHIRO_AUTHC_ABSTRACTAUTHENTICATOR_H

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
#include <cetty/shiro/realm/AuthenticatingRealmPtr.h>
#include <cetty/shiro/authc/AuthenticationInfoPtr.h>
#include <cetty/shiro/authc/CredentialsMatcher.h>

namespace cetty {
namespace shiro {
namespace authc {

class AuthenticationToken;
class AuthenticationInfo;

using namespace cetty::shiro::realm;

/**
 * An Authenticator is responsible for authenticating accounts in an application.  It
 * is one of the primary entry points into the Shiro API.
 * <p/>
 * Although not a requirement, there is usually a single 'master' Authenticator configured for
 * an application.  Enabling Pluggable Authentication Module (PAM) behavior
 * (Two Phase Commit, etc.) is usually achieved by the single {@code Authenticator} coordinating
 * and interacting with an application-configured set of {@link org.apache.shiro.realm.Realm Realm}s.
 * <p/>
 * Note that most Shiro users will not interact with an {@code Authenticator} instance directly.
 * Shiro's default architecture is based on an overall {@code SecurityManager} which typically
 * wraps an {@code Authenticator} instance.
 *
 * @see org.apache.shiro.mgt.SecurityManager
 * @see AbstractAuthenticator AbstractAuthenticator
 * @see org.apache.shiro.authc.pam.ModularRealmAuthenticator ModularRealmAuthenticator
 * @since 0.1
 */
class Authenticator {
public:
    typedef boost::function3<void, int, const AuthenticationToken&, const AuthenticationInfoPtr&> LoginCallback;
    typedef boost::function1<void, std::string> LogoutCallback;

    typedef boost::function1<void, const AuthenticationInfoPtr&> AuthenticateCallback;

public:
    /**
     * Default no-argument constructor. Ensures the internal
     * {@link AuthenticationListener AuthenticationListener} collection is a non-null {@code ArrayList}.
     */
    Authenticator() {}
    Authenticator(const RealmPtr& realm);
    Authenticator(const RealmPtr& realm, const CredentialsMatcher& matcher);

    virtual ~Authenticator() {}

    /**
     * Sets the CrendialsMatcher used during an authentication attempt to verify submitted credentials with those
     * stored in the system.  The implementation of this matcher can be switched via configuration to
     * support any number of schemes, including plain text comparisons, hashing comparisons, and others.
     *
     * <p>Unless overridden by this method, the default value is a
     * {@link org.apache.shiro.authc.credential.SimpleCredentialsMatcher} instance.
     *
     * @param credentialsMatcher the matcher to use.
     */
    void setCredentialsMatcher(const CredentialsMatcher& matcher) {
        this->credentialsMatcher = matcher;
    }

    /**
     * Implementation of the {@link Authenticator} interface that functions in the following manner:
     * <ol>
     * <li>Calls template {@link #doAuthenticate doAuthenticate} method for subclass execution of the actual
     * authentication behavior.</li>
     * <li>If an {@code AuthenticationException} is thrown during {@code doAuthenticate},
     * {@link #notifyFailure(AuthenticationToken, AuthenticationException) notify} any registered
     * {@link AuthenticationListener AuthenticationListener}s of the exception and then propogate the exception
     * for the caller to handle.</li>
     * <li>If no exception is thrown (indicating a successful login),
     * {@link #notifySuccess(AuthenticationToken, AuthenticationInfo) notify} any registered
     * {@link AuthenticationListener AuthenticationListener}s of the successful attempt.</li>
     * <li>Return the {@code AuthenticationInfo}</li>
     * </ol>
     *
     * @param token the submitted token representing the subject's (user's) login principals and credentials.
     * @return the AuthenticationInfo referencing the authenticated user's account data.
     * @throws AuthenticationException if there is any problem during the authentication process - see the
     *                                 interface's JavaDoc for a more detailed explanation.
     */
    void authenticate(const AuthenticationToken& token,
                      const AuthenticateCallback& callback);

    /**
     * Sets all realms used by this Authenticator, providing PAM (Pluggable Authentication Module) configuration.
     *
     * @param realms the realms to consult during authentication attempts.
     */
    void setRealm(const AuthenticatingRealmPtr& realm) {
        if (realm) {
            this->realm = realm;
        }
    }

    /**
     * Returns the realm(s) used by this {@code Authenticator} during an authentication attempt.
     *
     * @return the realm(s) used by this {@code Authenticator} during an authentication attempt.
     */
    const AuthenticatingRealmPtr& getRealm() const {
        return this->realm;
    }

private:
    void onGetAuthenticationInfo(const AuthenticationInfoPtr& info,
                                 const AuthenticationToken& token,
                                 const AuthenticateCallback& callback);

private:
    /**
    * List of realms that will be iterated through when a user authenticates
    * notice the lifecycle of AuthenticationRealm.
    */
    AuthenticatingRealmPtr realm;

    /**
     * Password matcher used to determine if the provided password matches
     * the password stored in the data store.
     */
    CredentialsMatcher credentialsMatcher;
};

}
}
}

#endif // #if !defined(CETTY_SHIRO_AUTHC_ABSTRACTAUTHENTICATOR_H)
