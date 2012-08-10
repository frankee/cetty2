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

namespace cetty {
namespace shiro {

using namespace ::cetty::shiro::authc;
using namespace ::cetty::shiro::session;
using namespace cetty::shiro::subject;
/**
 * The Shiro framework's default concrete implementation of the {@link SecurityManager} interface,
 * based around a collection of {@link org.apache.shiro.realm.Realm}s.  This implementation delegates its
 * authentication, authorization, and session operations to wrapped {@link Authenticator}, {@link Authorizer}, and
 * {@link org.apache.shiro.session.mgt.SessionManager SessionManager} instances respectively via superclass
 * implementation.
 * <p/>
 * To greatly reduce and simplify configuration, this implementation (and its superclasses) will
 * create suitable defaults for all of its required dependencies, <em>except</em> the required one or more
 * {@link Realm Realm}s.  Because {@code Realm} implementations usually interact with an application's data model,
 * they are almost always application specific;  you will want to specify at least one custom
 * {@code Realm} implementation that 'knows' about your application's data/security model
 * (via {@link #setRealm} or one of the overloaded constructors).  All other attributes in this class hierarchy
 * will have suitable defaults for most enterprise applications.
 * <p/>
 * <b>RememberMe notice</b>: This class supports the ability to configure a
 * {@link #setRememberMeManager RememberMeManager}
 * for {@code RememberMe} identity services for login/logout, BUT, a default instance <em>will not</em> be created
 * for this attribute at startup.
 * <p/>
 * Because RememberMe services are inherently client tier-specific and
 * therefore aplication-dependent, if you want {@code RememberMe} services enabled, you will have to specify an
 * instance yourself via the {@link #setRememberMeManager(RememberMeManager) setRememberMeManager}
 * mutator.  However if you're reading this JavaDoc with the
 * expectation of operating in a Web environment, take a look at the
 * {@code org.apache.shiro.web.DefaultWebSecurityManager} implementation, which
 * <em>does</em> support {@code RememberMe} services by default at startup.
 *
 * @since 0.2
 */
class SecurityManager {

private:
    SubjectFactory *subjectFactory;
    ModularRealmAuthenticator *authenticator;
    SessionManager *sessionManager;

public:
    /**
     * Default no-arg constructor.
     */
    SecurityManager() {
        subjectFactory = new SubjectFactory();
        authenticator = new ModularRealmAuthenticator();
        sessionManager = new SessionManager();
    }

    SecurityManager(SubjectFactory *subjectFactory,
        ModularRealmAuthenticator *authenticator,
        SessionManager *sessionManager)
    {
       this->subjectFactory = subjectFactory;
       this->authenticator = authenticator;
       this->sessionManager = sessionManager;
    }


    SubjectFactory *getSubjectFactory() {
        return subjectFactory;
    }

    void setSubjectFactory(SubjectFactory *subjectFactory) {
        this->subjectFactory = subjectFactory;
    }

protected:
    SubjectContext *createSubjectContext() {
        return new SubjectContext();
    }

    /**
     * Creates a {@code Subject} instance for the user represented by the given method arguments.
     *
     * @param token    the {@code AuthenticationToken} submitted for the successful authentication.
     * @param info     the {@code AuthenticationInfo} of a newly authenticated user.
     * @param existing the existing {@code Subject} instance that initiated the authentication attempt
     * @return the {@code Subject} instance that represents the context and session data for the newly
     *         authenticated subject.
     */
    Subject *createSubject(AuthenticationToken *token, AuthenticationInfo *info, Subject *existing) {
        SubjectContext *context = createSubjectContext();
        context->setAuthenticated(true);
        context->setAuthenticationToken(token);
        context->setAuthenticationInfo(info);
        if (existing != NULL) {
            context->setSubject(existing);
        }
        return createSubject(context);
    }

    /**
     * Binds a {@code Subject} instance created after authentication to the application for later use.
     * <p/>
     * The default implementation simply stores the Subject's principals and authentication state to the
     * {@code Subject}'s {@link Subject#getSession() session} to ensure it is available for reference later.
     *
     * @param subject the {@code Subject} instance created after authentication to be bound to the application
     *                for later use.
     */
    void bind(Subject *subject) {
        // TODO consider refactoring to use Subject.Binder.
        // This implementation was copied from SessionSubjectBinder that was removed
        PrincipalCollection *principals = subject->getPrincipals();
        if (principals != NULL && !principals->isEmpty()) {
            Session *session = subject->getSession();
            bindPrincipalsToSession(principals, session);
        } else {
            Session *session = subject->getSession(false);
            if (session != NULL) {
                session->removeAttribute(SubjectContext::PRINCIPALS_SESSION_KEY);
            }
        }

        if (subject->isAuthenticated()) {
            Session *session = subject->getSession();
            session->setAttribute(SubjectContext::AUTHENTICATED_SESSION_KEY, subject->isAuthenticated());
        } else {
            Session *session = subject.getSession(false);
            if (session != NULL) {
                session->removeAttribute(SubjectContext::AUTHENTICATED_SESSION_KEY);
            }
        }
    }

    /**
     * Saves the specified identity to the given session, making the session no longer anonymous.
     *
     * @param principals the Subject identity to save to the session
     * @param session    the Session to retain the Subject identity.
     * @throws IllegalArgumentException if the principals are null or empty or the session is null
     * @since 1.0
     */
    void bindPrincipalsToSession(PrincipalCollection *principals, Session *session){
        session->setAttribute(SubjectContext::PRINCIPALS_SESSION_KEY, principals);
    }

    /**
     * First authenticates the {@code AuthenticationToken} argument, and if successful, constructs a
     * {@code Subject} instance representing the authenticated account's identity.
     * <p/>
     * Once constructed, the {@code Subject} instance is then {@link #bind bound} to the application for
     * subsequent access before being returned to the caller.
     *
     * @param token the authenticationToken to process for the login attempt.
     * @return a Subject representing the authenticated user.
     * @throws AuthenticationException if there is a problem authenticating the specified {@code token}.
     */
    Subject *login(Subject *subject, AuthenticationToken *token) {
        AuthenticationInfo *info =  authenticate(token);
        if(info == NULL) {
            onFailedLogin(token, ae, subject);
            return NULL;
        }

        Subject *loggedIn = createSubject(token, info, subject);
        bind(loggedIn);
        onSuccessfulLogin(token, info, loggedIn);
        return loggedIn;
    }

    void onSuccessfulLogin(AuthenticationToken *token, AuthenticationInfo *info, Subject *subject) {
        rememberMeSuccessfulLogin(token, info, subject);
    }

    void rememberMeSuccessfulLogin(AuthenticationToken *token, AuthenticationInfo *info, Subject *subject);

    void onFailedLogin(AuthenticationToken *token, AuthenticationException ae, Subject *subject);
    void rememberMeFailedLogin(AuthenticationToken *token, AuthenticationException ae, Subject *subject);

    void beforeLogout(Subject *subject);

    void rememberMeLogout(Subject *subject);

    SubjectContext *copy(SubjectContext *subjectContext);

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
    Subject *createSubject(SubjectContext *subjectContext) {
        //create a copy so we don't modify the argument's backing map:
        SubjectContext *context = copy(subjectContext);

        //ensure that the context has a SecurityManager instance, and if not, add one:
        context = ensureSecurityManager(context);

        //Resolve an associated Session (usually based on a referenced session ID), and place it in the context before
        //sending to the SubjectFactory.  The SubjectFactory should not need to know how to acquire sessions as the
        //process is often environment specific - better to shield the SF from these details:
        context = resolveSession(context);

        //Similarly, the SubjectFactory should not require any concept of RememberMe - translate that here first
        //if possible before handing off to the SubjectFactory:
        context = resolvePrincipals(context);

        return getSubjectFactory().createSubject(context);
    }

    /**
     * Determines if there is a {@code SecurityManager} instance in the context, and if not, adds 'this' to the
     * context.  This ensures the SubjectFactory instance will have access to a SecurityManager during Subject
     * construction if necessary.
     *
     * @param context the subject context data that may contain a SecurityManager instance.
     * @return The SubjectContext to use to pass to a {@link SubjectFactory} for subject creation.
     * @since 1.0
     */
    SubjectContext *ensureSecurityManager(SubjectContext *context) {
        if (context->resolveSecurityManager() != NULL)  return context;
        context->setSecurityManager(this);
        return context;
    }

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
    SubjectContext *resolveSession(SubjectContext *context) {
        if (context->resolveSession() != NULL) {
            return context;
        }

        Session *session = resolveContextSession(context);
        if (session != NULL) {
                context->setSession(session);
        return context;
    }
        return context;
    }

    Session *resolveContextSession(SubjectContext *context);

    SessionKey *getSessionKey(SubjectContext *context);

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

    SubjectContext *resolvePrincipals(SubjectContext *context);
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
    void bindPrincipalsToSession(PrincipalCollection *principals, SubjectContext *context);

    SessionContext *createSessionContext(SubjectContext *subjectContext);
    void logout(Subject *subject);

    void stopSession(Subject *subject);

    /**
     * Unbinds or removes the Subject's state from the application, typically called during {@link #logout}.
     * <p/>
     * This implementation is symmetric with the {@link #bind} method in that it will remove any principals and
     * authentication state from the session if the session exists.  If there is no subject session, this method
     * does not do anything.
     *
     * @param subject the subject to unbind from the application as it will no longer be used.
     */
    void unbind(Subject *subject);

    Session *start(SessionContext *context);

    PrincipalCollection *getRememberedIdentity(SubjectContext *subjectContext);
};
}
}
#endif // #if !defined(CETTY_SHIRO_SECURITYMANAGER_H)
