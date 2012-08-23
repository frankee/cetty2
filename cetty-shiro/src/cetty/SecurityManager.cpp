/*
 * SecurityManager.cpp
 *
 *  Created on: 2012-8-22
 *      Author: chenhl
 */

#include <cetty/shiro/SecurityManager.h>
#include <cetty/shiro/subject/PrincipalCollection.h>
#include <cetty/shiro/subject/SubjectFactory.h>

namespace cetty {
namespace shiro {

bool SecurityManager::authenticate(const AuthenticationToken &token, AuthenticationInfo *info){
    return this->authenticator.authenticate(token, info);
}

SessionPtr SecurityManager::start(SessionContext &context){
    return this->sessionManager->start(context);
}

SessionPtr SecurityManager::getSession(const std::string &sessionId){
    return this->sessionManager->getSession(sessionId);
}

bool SecurityManager::login(AuthenticationToken &token,
                            Subject &existing,
                            Subject *subject){
    AuthenticationInfo info;
    if(!authenticate(token, &info)){
        onFailedLogin(token, existing);
        return false;
    }

    assert(subject != NULL);
    createSubject(token, info, existing, subject);

    bind(*subject);
    onSuccessfulLogin(token, info, *subject);

    return true;
}

void SecurityManager::logout(Subject &subject){
    beforeLogout(subject);
    PrincipalCollection principals = subject.getPrincipals();
    if (!principals.isEmpty()) {
        ModularRealmAuthenticator authc = getAuthenticator();
        authc.onLogout(principals);
         }

    unbind(subject);
    stopSession(subject);
}

void SecurityManager::destroy(){
    if(sessionManager) {
        delete sessionManager;
        sessionManager = NULL;
    }

    realms.clear();
    authenticator.clean();
}

void SecurityManager::setAuthenticatingRealm(AuthenticatingRealm &realm){
    std::vector<AuthenticatingRealm> realms;
    realms.push_back(realm);
    setAuthenticatingRealms(realms);
}

void SecurityManager::setAuthenticatingRealms(std::vector<AuthenticatingRealm> &realms){
    if(realms.empty()) return;
    this->realms = realms;
    authenticator.setRealms(this->realms);
    afterRealmsSet();
}

void SecurityManager::setSessionManager(SessionManager *sessionManager){
       if(sessionManager == NULL) return;
       if(sessionManager == this->sessionManager) return;
       if(this->sessionManager != NULL) delete this->sessionManager;
       this->sessionManager = sessionManager;
       afterSessionManagerSet();
}

SubjectContext SecurityManager::createSubjectContext(){
    return SubjectContext();
}

void SecurityManager::createSubject(AuthenticationToken &token,
                                    AuthenticationInfo &info,
                                    Subject &existing,
                                    Subject *subject){
    SubjectContext context = createSubjectContext();
    context.setAuthenticated(true);
    context.setAuthenticationToken(token);
    context.setAuthenticationInfo(info);
    context.setSubject(existing);

    createSubject(context, subject);
}

void SecurityManager::createSubject(SubjectContext &context, Subject *subject){
    //create a copy so we don't modify the argument's backing map:
    SubjectContext newContext = context;

    //ensure that the context has a SecurityManager instance, and if not, add one:
    ensureSecurityManager(newContext);

    //Resolve an associated Session (usually based on a referenced session ID), and place it in the context before
    //sending to the SubjectFactory.  The SubjectFactory should not need to know how to acquire sessions as the
    //process is often environment specific - better to shield the SF from these details:
    resolveSession(newContext);

    //Similarly, the SubjectFactory should not require any concept of RememberMe - translate that here first
    //if possible before handing off to the SubjectFactory:
    resolvePrincipals(newContext);

    assert(subject != NULL);
    (getSubjectFactory()).createSubject(newContext, subject);
}



void SecurityManager::bind(Subject &subject){
    // TODO consider refactoring to use Subject.Binder.
    // This implementation was copied from SessionSubjectBinder that was removed
    PrincipalCollection principals = subject.getPrincipals();
    if (!principals.isEmpty()) {
        SessionPtr session = subject.getSession();
        bindPrincipalsToSession(principals, session);
    } else {
        SessionPtr session = subject.getSession(false);
        if (session) {
            session->removeAttribute(SubjectContext::PRINCIPALS_SESSION_KEY);
        }
    }

    if (subject.isAuthenticated()) {
        SessionPtr session = subject.getSession();
        session->setAttribute(SubjectContext::AUTHENTICATED_SESSION_KEY, subject.isAuthenticated());
    } else {
        SessionPtr session = subject.getSession(false);
        if (session) {
            session->removeAttribute(SubjectContext::AUTHENTICATED_SESSION_KEY);
        }
    }
}

void SecurityManager::ensureSecurityManager(SubjectContext &context){
    if (context.resolveSecurityManager() == NULL)
        context.setSecurityManager(this);
}

void SecurityManager::resolveSession(SubjectContext &context){
    if (context.resolveSession()) return;

    SessionPtr session = resolveContextSession(context);
    if (session) context.setSession(session);
}

SessionPtr SecurityManager::resolveContextSession(SubjectContext &context){
    std::string sessionId = getSessionId(context);
    if (!sessionId.empty()) {
        return getSession(sessionId);
    }
    return SessionPtr();
}

std::string SecurityManager::getSessionId(SubjectContext &context){
    std::string sessionId = context.getSessionId();
    return sessionId;
}

void SecurityManager::resolvePrincipals(SubjectContext &context){}

void SecurityManager::bindPrincipalsToSession(PrincipalCollection &principals, SubjectContext &context){
    SecurityManager *securityManager = context.resolveSecurityManager();
    assert(securityManager != NULL);

    SessionPtr session = context.resolveSession();
    if (!session) {
        SessionContext sessionContext = createSessionContext(context);
        session = start(sessionContext);
        context.setSession(session);
    }
    bindPrincipalsToSession(principals, session);
}

SessionContext  SecurityManager::createSessionContext(SubjectContext &subjectContext){
    SessionContext sessionContext;

    std::string sessionId = subjectContext.getSessionId();
    if (!sessionId.empty()) {
        sessionContext.setSessionId(sessionId);
    }
    std::string host = subjectContext.resolveHost();
    if (!host.empty()) {
        sessionContext.setHost(host);
    }
    return sessionContext;
}


void SecurityManager::stopSession(Subject &subject){
    SessionPtr session = subject.getSession();
    if(session) session->stop();
}

void SecurityManager::unbind(Subject &subject){
    SessionPtr session = subject.getSession(false);
    if (session) {
        session->removeAttribute(SubjectContext::PRINCIPALS_SESSION_KEY);
        session->removeAttribute(SubjectContext::AUTHENTICATED_SESSION_KEY);
    }
}

}
}

