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
#include <cetty/shiro/authc/Sha256CredentialsMatcher.h>

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
    typedef boost::function2<void, const AuthenticationToken&, const AuthenticationInfo*> LoginSuccessCallback;
    typedef boost::function1<void, const AuthenticationToken&> LoginFailureCallback;
    typedef boost::function1<void, std::string> LogoutCallback;

    typedef boost::function1<void, const AuthenticationInfoPtr&> AuthenticateCallback;

public:
    /**
     * Default no-argument constructor. Ensures the internal
     * {@link AuthenticationListener AuthenticationListener} collection is a non-null {@code ArrayList}.
     */
    Authenticator() {}
    Authenticator(const RealmPtr& realm);

    virtual ~Authenticator() {}

    /**
     * Returns the <code>CredentialsMatcher</code> used during an authentication attempt to verify submitted
     * credentials with those stored in the system.
     *
     * <p>Unless overridden by the {@link #setCredentialsMatcher setCredentialsMatcher} method, the default
     * value is a {@link org.apache.shiro.authc.credential.SimpleCredentialsMatcher SimpleCredentialsMatcher} instance.
     *
     * @return the <code>CredentialsMatcher</code> used during an authentication attempt to verify submitted
     *         credentials with those stored in the system.
     */
    Sha256CredentialsMatcher *getCredentialsMatcher() const { return matcher; }

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
    void setCredentialsMatcher(Sha256CredentialsMatcher *matcher) {
        this->matcher = matcher;
    }

    /**
     * This implementation merely calls
     * {@link #notifyLogout(org.apache.shiro.subject.PrincipalCollection) notifyLogout} to allow any registered listeners
     * to react to the logout.
     *
     * @param principals the identifying principals of the {@code Subject}/account logging out.
     */
    virtual void onLogout(const std::string &user) { notifyLogout(user); }

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
    void authenticate(const AuthenticationToken &token,
        const AuthenticateCallback& callback);

    /**
     * Sets all realms used by this Authenticator, providing PAM (Pluggable Authentication Module) configuration.
     *
     * @param realms the realms to consult during authentication attempts.
     */
    void setRealm(const AuthenticatingRealm&realm) {
    }

    /**
     * Returns the realm(s) used by this {@code Authenticator} during an authentication attempt.
     *
     * @return the realm(s) used by this {@code Authenticator} during an authentication attempt.
     */
    const AuthenticatingRealmPtr& getRealm() const{
        return this->realm;
    }

    /**
     * Sets the {@link AuthenticationListener AuthenticationListener}s that should be notified during authentication
     * attempts.
     *
     * @param listeners one or more {@code AuthenticationListener}s that should be notified due to an
     *                  authentication attempt.
     */
    void addSuccessListener(const LoginSuccessCallback& successListeners){
    }

    void addFailureListener(const LoginFailureCallback& failureListeners){
    }

    void addLogoutListener(const LogoutCallback& logoutListeners){
    }

    /**
     * Returns the {@link AuthenticationListener AuthenticationListener}s that should be notified during authentication
     * attempts.
     *
     * @return the {@link AuthenticationListener AuthenticationListener}s that should be notified during authentication
     *         attempts.
     */
    const std::vector<LoginSuccessCallback>& getSuccessListener() const{
        return this->successListeners;
    }

    const std::vector<LoginFailureCallback>& getFailureListener() const{
        return this->failureListeners;
    }

    const std::vector<LogoutCallback>& getLogoutListener() const {
        return this->logoutListeners;
    }

protected:
    /**
     * Notifies any registered {@link AuthenticationListener AuthenticationListener}s that
     * authentication was successful for the specified {@code token} which resulted in the specified
     * {@code info}.  This implementation merely iterates over the internal {@code listeners} collection and
     * calls {@link AuthenticationListener#onSuccess(AuthenticationToken, AuthenticationInfo) onSuccess}
     * for each.
     *
     * @param token the submitted {@code AuthenticationToken} that resulted in a successful authentication.
     * @param info  the returned {@code AuthenticationInfo} resulting from the successful authentication.
     */
    void notifySuccess(const AuthenticationToken &token, const AuthenticationInfo &info) {
        std::vector<LoginSuccessCallback>::iterator it = this->successListeners.begin();
        for (; it != this->successListeners.end(); ++it) (*it)(token, &info);

    }

    /**
     * Notifies any registered {@link AuthenticationListener AuthenticationListener}s that
     * authentication failed for the
     * specified {@code token} which resulted in the specified {@code ae} exception.  This implementation merely
     * iterates over the internal {@code listeners} collection and calls
     * {@link AuthenticationListener#onFailure(AuthenticationToken, AuthenticationException) onFailure}
     * for each.
     *
     * @param token the submitted {@code AuthenticationToken} that resulted in a failed authentication.
     * @param ae    the resulting {@code AuthenticationException} that caused the authentication to fail.
     */
    void notifyFailure(const AuthenticationToken &token) {
        std::vector<LoginFailureCallback>::iterator it = this->failureListeners.begin();
        for (; it != this->failureListeners.end(); ++it) (*it)(token);
    }

    /**
     * Notifies any registered {@link AuthenticationListener AuthenticationListener}s that a
     * {@code Subject} has logged-out.  This implementation merely
     * iterates over the internal {@code listeners} collection and calls
     * {@link AuthenticationListener#onLogout(org.apache.shiro.subject.PrincipalCollection) onLogout}
     * for each.
     *
     * @param principals the identifying principals of the {@code Subject}/account logging out.
     */
    void notifyLogout(const std::string &user) {
        std::vector<LogoutCallback>::iterator it = this->logoutListeners.begin();
        for (; it != this->logoutListeners.end(); ++it) (*it)(user);
    }

private:
    /**
     * Any registered listeners that wish to know about things during the authentication process.
     */
    std::vector<LoginSuccessCallback> successListeners;
    std::vector<LoginFailureCallback> failureListeners;
    std::vector<LogoutCallback> logoutListeners;

     /**
     * List of realms that will be iterated through when a user authenticates
     * notice the lifecycle of AuthenticationRealm.
     */
    AuthenticatingRealmPtr realm;

    /**
     * Password matcher used to determine if the provided password matches
     * the password stored in the data store.
     */
    Sha256CredentialsMatcher *matcher;
};

}
}
}

#endif // #if !defined(CETTY_SHIRO_AUTHC_ABSTRACTAUTHENTICATOR_H)
