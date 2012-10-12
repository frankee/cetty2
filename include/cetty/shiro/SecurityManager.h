#if !defined(CETTY_SHIRO_SECURITYMANAGER_H)
#define CETTY_SHIRO_SECURITYMANAGER_H

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

#include <boost/function.hpp>

#include <cetty/shiro/authz/Authorizer.h>
#include <cetty/shiro/session/SessionManager.h>
#include <cetty/shiro/authc/Authenticator.h>
#include <cetty/shiro/authc/AuthenticationInfoPtr.h>
#include <cetty/shiro/SecurityManagerConfig.h>

namespace authc {
    class AuthenticationToken;
}

namespace cetty {
namespace shiro {

using namespace cetty::shiro::authc;
using namespace cetty::shiro::authz;
using namespace cetty::shiro::session;
using namespace cetty::shiro;


/**
 * A {@code SecurityManager} executes all security operations for <em>all</em> Subjects (aka users) across a
 * single application.
 * <p/>
 * The interface itself primarily exists as a convenience - it extends the {@link org.apache.shiro.authc.Authenticator},
 * {@link Authorizer}, and {@link SessionManager} interfaces, thereby consolidating
 * these behaviors into a single point of reference.  For most Shiro usages, this simplifies configuration and
 * tends to be a more convenient approach than referencing {@code Authenticator}, {@code Authorizer}, and
 * {@code SessionManager} instances separately;  instead one only needs to interact with a single
 * {@code SecurityManager} instance.
 * <p/>
 * In addition to the above three interfaces, this interface provides a number of methods supporting
 * {@link Subject} behavior. A {@link org.apache.shiro.subject.Subject Subject} executes
 * authentication, authorization, and session operations for a <em>single</em> user, and as such can only be
 * managed by {@code A SecurityManager} which is aware of all three functions.  The three parent interfaces on the
 * other hand do not 'know' about {@code Subject}s to ensure a clean separation of concerns.
 * <p/>
 * <b>Usage Note</b>: In actuality the large majority of application programmers won't interact with a SecurityManager
 * very often, if at all.  <em>Most</em> application programmers only care about security operations for the currently
 * executing user, usually attained by calling
 * {@link org.apache.shiro.SecurityUtils#getSubject() SecurityUtils.getSubject()}.
 * <p/>
 * Framework developers on the other hand might find working with an actual SecurityManager useful.
 *
 * @see org.apache.shiro.mgt.DefaultSecurityManager
 * @since 0.2
 */
class SecurityManager {
public:
    typedef boost::function<void (int, const AuthenticationToken&, const AuthenticationInfoPtr&, const SessionPtr&)> LoginCallback;
    typedef boost::function<void (const SessionPtr&)> BeforeLogoutCallback;
    typedef boost::function<void (const AuthenticationInfoPtr&)> AuthenticateCallback;
    typedef boost::function3<void, bool, const std::string&, const std::string&> AuthorizeCallback;

public:
    static SecurityManager& instance();

public:
    SecurityManager();
    virtual ~SecurityManager();

    /// login by user name and password
    void login(const AuthenticationToken& token, const LoginCallback& callback);
    void logout(const std::string& sessionId);

    void isPermitted(const std::string& principal,
        const std::string& permission,
        const AuthorizeCallback& callback) const;

    void isPermitted(const PrincipalCollection& principals,
        const std::string& permission,
        const AuthorizeCallback& callback) const;

    void isPermitted(const PrincipalCollection& principal,
        const PermissionPtr& permission,
        const AuthorizeCallback& callback) const;


    SessionManager* getSessionManager();
    Authenticator& getAuthenticator();
    Authorizer& getAuthorizer();

    void setRealm(const RealmPtr &realm);
    const RealmPtr& getRealms() const;

private:
    void init();

    /// login by session id
    void getSession(const std::string& id, SessionManager::SessionCallback callback );

    void bind(const AuthenticationToken& token, const AuthenticationInfo& info, const SessionPtr& session);

    void fireFailedLoginEvent(const AuthenticationToken& token);
    void fireSuccessfulLoginEvent(const AuthenticationToken& token, const AuthenticationInfoPtr& info);

    void onAuthenticate(const AuthenticationInfoPtr& info,
                        const AuthenticationToken& token,
                        const LoginCallback& callback);

    void onStartSession(const SessionPtr& session,
                        const AuthenticationInfoPtr& info,
                        const AuthenticationToken& token,
                        const LoginCallback& callback);

    void onLogout(const SessionPtr& session);

private:
    SecurityManagerConfig config;

    Authenticator authenticator;
    Authorizer    authorizer;

    SessionManager* sessionManager;

    RealmPtr realms;
};

}
}
#endif // #if !defined(CETTY_SHIRO_SECURITYMANAGER_H)
