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

#include <cetty/shiro/authc/ModularRealmAuthenticator.h>
#include <cetty/shiro/session/SessionManager.h>
#include <cetty/shiro/subject/Subject.h>
#include <cetty/shiro/subject/SubjectFactory.h>
#include <cetty/shiro/subject/SubjectContext.h>

namespace cetty {
namespace shiro {

using namespace cetty::shiro::authc;
using namespace cetty::shiro::session;
using namespace cetty::shiro::subject;

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
    SecurityManager(): sessionManager(NULL){}
    SecurityManager(SessionManager *sessionManager){
        this->sessionManager = sessionManager;
    }

    /**
     * Authenticates a user based on the submitted {@code AuthenticationToken}.
     * <p/>
     * If the authentication is successful, an {@link AuthenticationInfo} instance is returned that represents the
     * user's account data relevant to Shiro.  This returned object is generally used in turn to construct a
     * {@code Subject} representing a more complete security-specific 'view' of an account that also allows access to
     * a {@code Session}.
     *
     * @param authenticationToken any representation of a user's principals and credentials submitted during an
     *                            authentication attempt.
     * @return the AuthenticationInfo representing the authenticating user's account data.
     */
    bool authenticate(const AuthenticationToken &token, AuthenticationInfo *info);

    /**
     * Starts a new session based on the specified contextual initialization data, which can be used by the underlying
     * implementation to determine how exactly to create the internal Session instance.
     * <p/>
     * This method is mainly used in framework development, as the implementation will often relay the argument
     * to an underlying {@link SessionFactory} which could use the context to construct the internal Session
     * instance in a specific manner.  This allows pluggable {@link org.apache.shiro.session.Session Session} creation
     * logic by simply injecting a {@code SessionFactory} into the {@code SessionManager} instance.
     *
     * @param context the contextual initialization data that can be used by the implementation or underlying
     *                {@link SessionFactory} when instantiating the internal {@code Session} instance.
     * @return the newly created session.
     * @see SessionFactory#createSession(SessionContext)
     * @since 1.0
     */
    SessionPtr start(SessionContext &context);

    /**
     * Retrieves the session corresponding to the specified contextual data (such as a session ID if applicable), or
     * {@code null} if no Session could be found.  If a session is found but invalid (stopped or expired), a
     * {@link SessionException} will be thrown.
     *
     * @param key the Session key to use to look-up the Session
     * @return the {@code Session} instance corresponding to the given lookup key or {@code null} if no session
     *         could be acquired.
     * @throws SessionException if a session was found but it was invalid (stopped/expired).
     * @since 1.0
     */
    SessionPtr getSession(const std::string &sessionId);

    /**
     * Logs in the specified Subject using the given {@code authenticationToken}, returning an updated Subject
     * instance reflecting the authenticated state if successful or throwing {@code AuthenticationException} if it is
     * not.
     * <p/>
     * Note that most application developers should probably not call this method directly unless they have a good
     * reason for doing so.  The preferred way to log in a Subject is to call
     * <code>subject.{@link org.apache.shiro.subject.Subject#login login(authenticationToken)}</code> (usually after
     * acquiring the Subject by calling {@link org.apache.shiro.SecurityUtils#getSubject() SecurityUtils.getSubject()}).
     * <p/>
     * Framework developers on the other hand might find calling this method directly useful in certain cases.
     *
     * @param subject             the subject against which the authentication attempt will occur
     * @param authenticationToken the token representing the Subject's principal(s) and credential(s)
     * @return the subject instance reflecting the authenticated state after a successful attempt
     * @throws AuthenticationException if the login attempt failed.
     * @since 1.0
     */
    bool login(AuthenticationToken &token, Subject &existing, Subject *subject);

    /**
     * Logs out the specified Subject from the system.
     * <p/>
     * Note that most application developers should not call this method unless they have a good reason for doing
     * so.  The preferred way to logout a Subject is to call
     * <code>{@link org.apache.shiro.subject.Subject#logout Subject.logout()}</code>, not the
     * {@code SecurityManager} directly.
     * <p/>
     * Framework developers on the other hand might find calling this method directly useful in certain cases.
     *
     * @param subject the subject to log out.
     * @since 1.0
     */
    void logout(Subject &subject);

    /**
     * Called when this object is being destroyed, allowing any necessary cleanup of internal resources.
     *
     * @throws Exception if an exception occurs during object destruction.
     */
    void destroy();

    /**
     * Convenience method for applications using a single realm that merely wraps the realm in a list and then invokes
     * the {@link #setRealms} method.
     *
     * @param realm the realm to set for a single-realm application.
     * @since 0.2
     */
    void setAuthenticatingRealm(AuthenticatingRealm &realm);

    /**
     * Sets the realms managed by this <tt>SecurityManager</tt> instance.
     *
     * @param realms the realms managed by this <tt>SecurityManager</tt> instance.
     * @throws IllegalArgumentException if the realms collection is null or empty.
     */
    void setAuthenticatingRealms(std::vector<AuthenticatingRealm> &realms);

    void afterRealmsSet() {}

    /**
     * Returns the {@link Realm Realm}s managed by this SecurityManager instance.
     *
     * @return the {@link Realm Realm}s managed by this SecurityManager instance.
     */
    std::vector<AuthenticatingRealm> &getRealms() { return realms; }

    /**
     * Returns the delegate <code>Authenticator</code> instance that this SecurityManager uses to perform all
     * authentication operations.  Unless overridden by the
     * {@link #setAuthenticator(org.apache.shiro.authc.Authenticator) setAuthenticator}, the default instance is a
     * {@link org.apache.shiro.authc.pam.ModularRealmAuthenticator ModularRealmAuthenticator}.
     *
     * @return the delegate <code>Authenticator</code> instance that this SecurityManager uses to perform all
     *         authentication operations.
     */
    ModularRealmAuthenticator &getAuthenticator(){
        return authenticator;
    }

    /**
     * Sets the delegate <code>Authenticator</code> instance that this SecurityManager uses to perform all
     * authentication operations.  Unless overridden by this method, the default instance is a
     * {@link org.apache.shiro.authc.pam.ModularRealmAuthenticator ModularRealmAuthenticator}.
     *
     * @param authenticator the delegate <code>Authenticator</code> instance that this SecurityManager will use to
     *                      perform all authentication operations.
     * @throws IllegalArgumentException if the argument is <code>null</code>.
     */
    void setAuthenticator(ModularRealmAuthenticator &authenticator){
        this->authenticator = authenticator;
    }

    /**
     * Sets the underlying delegate {@link SessionManager} instance that will be used to support this implementation's
     * <tt>SessionManager</tt> method calls.
     * <p/>
     * This <tt>SecurityManager</tt> implementation does not provide logic to support the inherited
     * <tt>SessionManager</tt> interface, but instead delegates these calls to an internal
     * <tt>SessionManager</tt> instance.
     * <p/>
     * If a <tt>SessionManager</tt> instance is not set, a default one will be automatically created and
     * initialized appropriately for the the existing runtime environment.
     *
     * @param sessionManager delegate instance to use to support this manager's <tt>SessionManager</tt> method calls.
     */
    void setSessionManager(SessionManager *sessionManager);

    void afterSessionManagerSet() {}

    /**
     * Returns this security manager's internal delegate {@link SessionManager SessionManager}.
     *
     * @return this security manager's internal delegate {@link SessionManager SessionManager}.
     * @see #setSessionManager(org.apache.shiro.session.mgt.SessionManager) setSessionManager
     */
    SessionManager *getSessionManager() { return this->sessionManager; }


    SubjectFactory &getSubjectFactory() { return subjectFactory;}

    void setSubjectFactory(const SubjectFactory &subjectFactory) {
        this->subjectFactory = subjectFactory;
    }

    SubjectContext createSubjectContext();

    /**
     * Creates a {@code Subject} instance for the user represented by the given method arguments.
     *
     * @param token    the {@code AuthenticationToken} submitted for the successful authentication.
     * @param info     the {@code AuthenticationInfo} of a newly authenticated user.
     * @param existing the existing {@code Subject} instance that initiated the authentication attempt
     * @return the {@code Subject} instance that represents the context and session data for the newly
     *         authenticated subject.
     */
    void createSubject(AuthenticationToken &token, AuthenticationInfo &info, Subject &existing, Subject *subject);

    /**
     * Binds a {@code Subject} instance created after authentication to the application for later use.
     * <p/>
     * The default implementation simply stores the Subject's principals and authentication state to the
     * {@code Subject}'s {@link Subject#getSession() session} to ensure it is available for reference later.
     *
     * @param subject the {@code Subject} instance created after authentication to be bound to the application
     *                for later use.
     */

    /**
     * This implementation attempts to resolve any session ID that may exist in the context by
     * passing it to the {@link #resolveSession(SubjectContext)} method.  The
     * return value from that call is then used to attempt to resolve the subject identity via the
     * {@link #resolvePrincipals(SubjectContext)} method.  The return value from that call is then used to create
     * the {@code Subject} instance by calling
     * <code>{@link #getSubjectFactory() getSubjectFactory()}.{@link SubjectFactory#createSubject createSubject}(resolvedContext);</code>
     *
     * @param subjectContext any data needed to direct how the Subject should be constructed.
     * @return the {@code Subject} instance reflecting the specified initialization data.
     * @see SubjectFactory#createSubject
     * @since 1.0
     */
    void createSubject(SubjectContext &context, Subject *subject);

    void bind(Subject &subject);

    /**
     * Saves the specified identity to the given session, making the session no longer anonymous.
     *
     * @param principals the Subject identity to save to the session
     * @param session    the Session to retain the Subject identity.
     * @throws IllegalArgumentException if the principals are null or empty or the session is null
     * @since 1.0
     */
    void bindPrincipalsToSession(const PrincipalCollection &principals, SessionPtr &session){
        session->setAttribute(SubjectContext::PRINCIPALS_SESSION_KEY, principals);
    }

    void onSuccessfulLogin(const AuthenticationToken &token, const AuthenticationInfo &info, const  Subject &subject) {};

    void onFailedLogin(const AuthenticationToken &token, const  Subject &subject){};

    void beforeLogout(const Subject &subject){};

    /**
     * Determines if there is a {@code SecurityManager} instance in the context, and if not, adds 'this' to the
     * context.  This ensures the SubjectFactory instance will have access to a SecurityManager during Subject
     * construction if necessary.
     *
     * @param context the subject context data that may contain a SecurityManager instance.
     * @return The SubjectContext to use to pass to a {@link SubjectFactory} for subject creation.
     * @since 1.0
     */
    void ensureSecurityManager(SubjectContext &context);

    /**
     * Attempts to resolve any associated session based on the context and returns a
     * context that represents this resolved {@code Session} to ensure it may be referenced if necessary by the
     * invoked {@link SubjectFactory} that performs actual {@link Subject} construction.
     * <p/>
     * If there is a {@code Session} already in the context because that is what the caller wants to be used for
     * {@code Subject} construction, or if no session is resolved, this method effectively does nothing
     * returns the Map method argument unaltered.
     *
     * @param context the subject context data that may resolve a Session instance.
     * @return The context to use to pass to a {@link SubjectFactory} for subject creation.
     * @since 1.0
     */
    void resolveSession(SubjectContext &context);

    SessionPtr resolveContextSession(SubjectContext &context);

    std::string getSessionId(SubjectContext &context);

    /**
     * Attempts to resolve an identity (a {@link PrincipalCollection}) for the context using heuristics.  The
     * implementation strategy:
     * <ol>
     * <li>Check the context to see if it can already {@link SubjectContext#resolvePrincipals resolve an identity}.  If
     * so, this method does nothing and returns the method argument unaltered.</li>
     * <li>Check for a RememberMe identity by calling {@link #getRememberedIdentity}.  If that method returns a
     * non-null value, place the remembered {@link PrincipalCollection} in the context.</li>
     * <li>If the remembered identity is discovered, associate it with the session to eliminate unnecessary
     * rememberMe accesses for the remainder of the session</li>
     * </ol>
     *
     * @param context the subject context data that may provide (directly or indirectly through one of its values) a
     *                {@link PrincipalCollection} identity.
     * @return The Subject context to use to pass to a {@link SubjectFactory} for subject creation.
     * @since 1.0
     */

    void resolvePrincipals(SubjectContext &context);
    /**
     * Satisfies SHIRO-157: associate a known identity with the current session to ensure that we don't need to
     * continually perform rememberMe operations for sessions that already have an identity.  Doing this prevents the
     * need to continually reference, decrypt and deserialize the rememberMe cookie every time - something that can
     * be computationally expensive if many requests are intercepted.
     * <p/>
     * Note that if the SubjectContext cannot {@link SubjectContext#resolveSession resolve} a session, a new session
     * will be created receive the principals and then appended to the SubjectContext so it can be used later when
     * constructing the Subject.
     *
     * @param principals the non-null, non-empty principals to bind to the SubjectContext's session
     * @param context    the context to use to locate or create a session to which the principals will be saved
     * @since 1.0
     */
    void bindPrincipalsToSession(PrincipalCollection &principals, SubjectContext &context);

    SessionContext createSessionContext(SubjectContext &subjectContext);

    void stopSession(Subject &subject);

    /**
     * Unbinds or removes the Subject's state from the application, typically called during {@link #logout}.
     * <p/>
     * This implementation is symmetric with the {@link #bind} method in that it will remove any principals and
     * authentication state from the session if the session exists.  If there is no subject session, this method
     * does not do anything.
     *
     * @param subject the subject to unbind from the application as it will no longer be used.
     */
    void unbind(Subject &subject);

private:
    SubjectFactory subjectFactory;
    ModularRealmAuthenticator authenticator;
    SessionManager *sessionManager;

    std::vector<AuthenticatingRealm> realms;
};

}
}
#endif // #if !defined(CETTY_SHIRO_SECURITYMANAGER_H)
