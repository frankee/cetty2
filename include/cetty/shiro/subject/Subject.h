#if !defined(CETTY_SHIRO_SUBJECT_SUBJECT_H)
#define CETTY_SHIRO_SUBJECT_SUBJECT_H
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
#include <cstdlib>

#include <cetty/shiro/session/Session.h>
#include <cetty/shiro/session/SessionPtr.h>
#include <cetty/shiro/authc/AuthenticationToken.h>
#include <cetty/shiro/subject/PrincipalCollection.h>

namespace cetty { namespace shiro {
    class SecurityManager;
}}

namespace cetty { namespace shiro { namespace session {
    class SessionContext;
}}}

namespace cetty {
namespace shiro {
namespace subject {

using namespace cetty::shiro;
using namespace cetty::shiro::session;
using namespace cetty::shiro::authc;

/**
 * A {@code Subject} represents state and security operations for a <em>single</em> application user.
 * These operations include authentication (login/logout), authorization (access control), and
 * session access. It is Shiro's primary mechanism for single-user security functionality.
 * <h3>Acquiring a Subject</h3>
 * To acquire the currently-executing {@code Subject}, application developers will almost always use
 * {@code SecurityUtils}:
 * <pre>
 * {@link SecurityUtils SecurityUtils}.{@link org.apache.shiro.SecurityUtils#getSubject() getSubject()}</pre>
 * Almost all security operations should be performed with the {@code Subject} returned from this method.
 * <h3>Permission methods</h3>
 * Note that there are many *Permission methods in this interface overloaded to accept String arguments instead of
 * {@link Permission Permission} instances. They are a convenience allowing the caller to use a String representation of
 * a {@link Permission Permission} if desired.  The underlying Authorization subsystem implementations will usually
 * simply convert these String values to {@link Permission Permission} instances and then just call the corresponding
 * type-safe method.  (Shiro's default implementations do String-to-Permission conversion for these methods using
 * {@link org.apache.shiro.authz.permission.PermissionResolver PermissionResolver}s.)
 * <p/>
 * These overloaded *Permission methods forgo type-saftey for the benefit of convenience and simplicity,
 * so you should choose which ones to use based on your preferences and needs.
 *
 * @since 0.1
 */
class Subject {

public:
    Subject():  securityManager(NULL), authenticated(false){}

    Subject(SecurityManager *securityManager)
      : securityManager(securityManager),
        authenticated(false){}

    Subject(const PrincipalCollection &principals,
            SecurityManager *securityManager,
            SessionPtr session,
            bool authenticated,
            const std::string &host)
      :principals(principals),
       securityManager(securityManager),
       session(session),
       authenticated(authenticated),
       host(host){}


    /**
     * Returns this Subject's application-wide uniquely identifying principal, or {@code null} if this
     * Subject is anonymous because it doesn't yet have any associated account data (for example,
     * if they haven't logged in).
     * <p/>
     * The term <em>principal</em> is just a fancy security term for any identifying attribute(s) of an application
     * user, such as a username, or user id, or public key, or anything else you might use in your application to
     * identify a user.
     * <h4>Uniqueness</h4>
     * Although given names and family names (first/last) are technically considered principals as well,
     * Shiro expects the object returned from this method to be an identifying attribute unique across
     * your entire application.
     * <p/>
     * This implies that things like given names and family names are usually poor
     * candidates as return values since they are rarely guaranteed to be unique;  Things often used for this value:
     * <ul>
     * <li>A {@code long} RDBMS surrogate primary key</li>
     * <li>An application-unique username</li>
     * <li>A {@link java.util.UUID UUID}</li>
     * <li>An LDAP Unique ID</li>
     * </ul>
     * or any other similar suitable unique mechanism valuable to your application.
     * <p/>
     * Most implementations will simply return
     * <code>{@link #getPrincipals()}.{@link org.apache.shiro.subject.PrincipalCollection#getPrimaryPrincipal() getPrimaryPrincipal()}</code>
     *
     * @return this Subject's application-specific unique identity.
     * @see org.apache.shiro.subject.PrincipalCollection#getPrimaryPrincipal()
     */
    std::string getPrincipal() { return getPrimaryPrincipal(getPrincipals()); }

    /**
     * Returns this Subject's principals (identifying attributes) in the form of a {@code PrincipalCollection} or
     * {@code null} if this Subject is anonymous because it doesn't yet have any associated account data (for example,
     * if they haven't logged in).
     * <p/>
     * The word &quot;principals&quot; is nothing more than a fancy security term for identifying attributes associated
     * with a Subject, aka, application user.  For example, user id, a surname (family/last name), given (first) name,
     * social security number, nickname, username, etc, are all examples of a principal.
     *
     * @return all of this Subject's principals (identifying attributes).
     * @see #getPrincipal()
     * @see org.apache.shiro.subject.PrincipalCollection#getPrimaryPrincipal()
     */
    const PrincipalCollection &getPrincipals() const{ return this->principals; }

    /**
     * Performs a login attempt for this Subject/user.  If unsuccessful,
     * an {@link AuthenticationException} is thrown, the subclass of which identifies why the attempt failed.
     * If successful, the account data associated with the submitted principals/credentials will be
     * associated with this {@code Subject} and the method will return quietly.
     * <p/>
     * Upon returning quietly, this {@code Subject} instance can be considered
     * authenticated and {@link #getPrincipal() getPrincipal()} will be non-null and
     * {@link #isAuthenticated() isAuthenticated()} will be {@code true}.
     *
     * @param token the token encapsulating the subject's principals and credentials to be passed to the
     *              Authentication subsystem for verification.
     * @throws org.apache.shiro.authc.AuthenticationException
     *          if the authentication attempt fails.
     * @since 0.9
     */
    bool login(AuthenticationToken &token);

    /**
     * Logs out this Subject and invalidates and/or removes any associated entities,
     * such as a {@link Session Session} and authorization data.  After this method is called, the Subject is
     * considered 'anonymous' and may continue to be used for another log-in if desired.
     * <h3>Web Environment Warning</h3>
     * Calling this method in web environments will usually remove any associated session cookie as part of
     * session invalidation.  Because cookies are part of the HTTP header, and headers can only be set before the
     * response body (html, image, etc) is sent, this method in web environments must be called before <em>any</em>
     * content has been rendered.
     * <p/>
     * The typical approach most applications use in this scenario is to redirect the user to a different
     * location (e.g. home page) immediately after calling this method.  This is an effect of the HTTP protocol
     * itself and not a reflection of Shiro's implementation.
     * <p/>
     * Non-HTTP environments may of course use a logged-out subject for login again if desired.
     */
    void logout();

    /**
     * Returns {@code true} if this Subject/user proved their identity <em>during their current session</em>
     * by providing valid credentials matching those known to the system, {@code false} otherwise.
     * <p/>
     * Note that even if this Subject's identity has been remembered via 'remember me' services, this method will
     * still return {@code false} unless the user has actually logged in with proper credentials <em>during their
     * current session</em>.  See the {@link #isRemembered() isRemembered()} method JavaDoc for more.
     *
     * @return {@code true} if this Subject proved their identity during their current session
     *         by providing valid credentials matching those known to the system, {@code false} otherwise.
     * @since 0.9
     */
    bool isAuthenticated() { return authenticated; }

    /**
     * Returns the application {@code Session} associated with this Subject.  If no session exists when this
     * method is called, a new session will be created, associated with this Subject, and then returned.
     *
     * @return the application {@code Session} associated with this Subject.
     * @see #getSession(boolean)
     * @since 0.2
     */

    SessionPtr getSession() { return getSession(true); }

    /**
     * Returns the application {@code Session} associated with this Subject.  Based on the boolean argument,
     * this method functions as follows:
     * <ul>
     * <li>If there is already an existing session associated with this {@code Subject}, it is returned and
     * the {@code create} argument is ignored.</li>
     * <li>If no session exists and {@code create} is {@code true}, a new session will be created, associated with
     * this {@code Subject} and then returned.</li>
     * <li>If no session exists and {@code create} is {@code false}, {@code null} is returned.</li>
     * </ul>
     *
     * @param create boolean argument determining if a new session should be created or not if there is no existing session.
     * @return the application {@code Session} associated with this {@code Subject} or {@code null} based
     *         on the above described logic.
     * @since 0.2
     */
    SessionPtr getSession(bool create);

    SecurityManager *getSecurityManager() const{ return securityManager; }

    /**
     * Returns the host name or IP associated with the client who created/is interacting with this Subject.
     *
     * @return the host name or IP associated with the client who created/is interacting with this Subject.
     */
    const std::string &getHost() { return this->host; }

    virtual ~Subject(){}


protected:
    SessionContext createSessionContext();

    bool hasPrincipals() { return !(getPrincipals().isEmpty()); }


private:
    PrincipalCollection principals;
    SecurityManager *securityManager;
    SessionPtr session;

    bool authenticated;
    std::string host;


private:
    std::string getPrimaryPrincipal(const PrincipalCollection &principals){ return principals.getPrimaryPrincipal(); }

    // should delete the session?
    void sessionStopped();

};
}
}
}

#endif //#if !defined(CETTY_SHIRO_SUBJECT_SUBJECT_H)
