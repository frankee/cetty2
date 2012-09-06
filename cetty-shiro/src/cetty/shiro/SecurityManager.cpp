/*
 * SecurityManager.cpp
 *
 *  Created on: 2012-8-22
 *      Author: chenhl
 */

#include <cetty/shiro/SecurityManager.h>
#include <cetty/shiro/util/EmptyObj.h>

namespace cetty {
namespace shiro {

using namespace cetty::shiro::util;

void SecurityManager::init(){
    sessionManager = new SessionManager();

    AuthenticatingRealm authcRealm;
    authenticator.addRealm(authcRealm);

    AuthorisingRealm authzRealm;
    authorizer.addRealm(authzRealm);
}

bool SecurityManager::authenticate(const AuthenticationToken &token,
                                   AuthenticationInfo *info
                                  )
{
    return this->authenticator.authenticate(token, info);
}

bool SecurityManager::authoriseUser(const std::string &sessionId,
                                    const std::string &operation
                                   )
{
    DelegateSession delegateSession;
    if(!getSession(sessionId, &delegateSession)) return false;

    SessionPtr session = delegateSession.getSession();
    if(session == NULL) return false;
    return authorizer.authoriseUser(session, operation);
}

bool SecurityManager::authoriseApp(const std::string &appKey,
                                   const std::string &operation
                                  )
{
    return authorizer.authoriseApp(appKey, operation);
}

bool SecurityManager::getSession(const std::string &sessionId, DelegateSession *delegateSession){
    SessionPtr session = this->sessionManager->getSession(sessionId);
    if(session == NULL) return false;

    assert(delegateSession != NULL);
    delegateSession->setId(sessionId);
    delegateSession->setSessionManager(this->sessionManager);

    return true;
}

const std::string &SecurityManager::login(AuthenticationToken &token){
    AuthenticationInfo info;
    if(!authenticate(token, &info)){
        onFailedLogin(token);
        return cetty::shiro::util::emptyString;
    }

    SessionPtr session = sessionManager->start();
    assert(session != NULL);

    DelegateSession delegateSession;
    delegateSession.setId(session->getId());
    delegateSession.setSessionManager(sessionManager);

    bind(info, delegateSession);
    onSuccessfulLogin(token, info);

    return delegateSession.getId();
}

void SecurityManager::logout(const std::string &sessionId){
    beforeLogout(sessionId);

    DelegateSession session;
    if(getSession(sessionId, &session)){
        std::string userId = session.getAttribute(AuthenticationInfo::USER_ID);
        authenticator.onLogout(userId);
        session.stop();
    }
}

void SecurityManager::destroy(){
    if(sessionManager) {
        delete sessionManager;
        sessionManager = NULL;
    }
}

void SecurityManager::bind(AuthenticationInfo &info, DelegateSession &session){
    session.setAttribute(AuthenticationInfo::USER_ID, info.getUserId());
    session.setAttribute(AuthenticationInfo::CREDENTIALS, info.getCredentials());
    session.setAttribute(AuthenticationInfo::CREDENTIALS_SALT, info.getCredentialsSalt());
    session.setAttribute(AuthenticationInfo::CODE_TYPE, info.getCodeType());
}


void SecurityManager::stopSession(const std::string &sessionId){
    sessionManager->stop(sessionId);
}

}
}

