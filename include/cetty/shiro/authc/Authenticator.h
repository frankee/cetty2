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
#include <cetty/shiro/subject/PrincipalCollection.h>
#include <boost/function.hpp>

namespace cetty {
namespace shiro {
namespace authc {

class AuthenticationToken;
class AuthenticationInfo;

using namespace cetty::shiro::subject;

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
    typedef boost::function1<void, const PrincipalCollection&> LogoutCallback;

public:
    /**
     * Default no-argument constructor. Ensures the internal
     * {@link AuthenticationListener AuthenticationListener} collection is a non-null {@code ArrayList}.
     */
    Authenticator() {}

    /**
     * Sets the {@link AuthenticationListener AuthenticationListener}s that should be notified during authentication
     * attempts.
     *
     * @param listeners one or more {@code AuthenticationListener}s that should be notified due to an
     *                  authentication attempt.
     */

    void setAuthenticationListeners(const std::vector<LoginSuccessCallback> &successListeners,
                                    const std::vector<LoginFailureCallback> &failureListeners,
                                    const std::vector<LogoutCallback> &logoutListeners) {
        this->successListeners.assign(successListeners.begin(), successListeners.end());
        this->failureListeners.assign(failureListeners.begin(), failureListeners.end());
        this->logoutListeners.assign(logoutListeners.begin(), logoutListeners.end());
    }

    void setSuccessListener(const std::vector<LoginSuccessCallback> &successListeners){
        this->successListeners.assign(successListeners.begin(), successListeners.end());
    }

    void setFailureListener(const std::vector<LoginFailureCallback> &failureListeners){
        this->failureListeners.assign(failureListeners.begin(), failureListeners.end());
    }

    void setLogoutListener(const std::vector<LogoutCallback> &logoutListeners){
        this->logoutListeners.assign(logoutListeners.begin(), logoutListeners.end());
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

    /**
     * This implementation merely calls
     * {@link #notifyLogout(org.apache.shiro.subject.PrincipalCollection) notifyLogout} to allow any registered listeners
     * to react to the logout.
     *
     * @param principals the identifying principals of the {@code Subject}/account logging out.
     */
    virtual void onLogout(const PrincipalCollection &principals) { notifyLogout(principals); }

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
    bool authenticate(const AuthenticationToken &token, AuthenticationInfo *info);

    virtual ~Authenticator(){}

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
    void notifyLogout(const PrincipalCollection &principals) {
        std::vector<LogoutCallback>::iterator it = this->logoutListeners.begin();
        for (; it != this->logoutListeners.end(); ++it) (*it)(principals);
    }

    /**
     * Template design pattern hook for subclasses to implement specific authentication behavior.
     * <p/>
     * Common behavior for most authentication attempts is encapsulated in the
     * {@link #authenticate} method and that method invokes this one for custom behavior.
     * <p/>
     * <b>N.B.</b> Subclasses <em>should</em> throw some kind of
     * {@code AuthenticationException} if there is a problem during
     * authentication instead of returning {@code null}.  A {@code null} return value indicates
     * a configuration or programming error, since {@code AuthenticationException}s should
     * indicate any expected problem (such as an unknown account or username, or invalid password, etc).
     *
     * @param token the authentication token encapsulating the user's login information.
     * @return an {@code AuthenticationInfo} object encapsulating the user's account information
     *         important to Shiro.
     * @throws AuthenticationException if there is a problem logging in the user.
     */
    virtual bool doAuthenticate(const AuthenticationToken &token, AuthenticationInfo *info) = 0;

private:

    /**
     * Any registered listeners that wish to know about things during the authentication process.
     */
    std::vector<LoginSuccessCallback> successListeners;
    std::vector<LoginFailureCallback> failureListeners;
    std::vector<LogoutCallback> logoutListeners;
};

}
}
}

#endif // #if !defined(CETTY_SHIRO_AUTHC_ABSTRACTAUTHENTICATOR_H)
