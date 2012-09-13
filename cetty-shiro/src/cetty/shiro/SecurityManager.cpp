/*
 * SecurityManager.cpp
 *
 *  Created on: 2012-8-22
 *      Author: chenhl
 */

#include <cetty/shiro/SecurityManager.h>
#include <cetty/shiro/session/Session.h>

namespace cetty {
namespace shiro {

void SecurityManager::init(){
    sessionManager = new SessionManager();

    AuthenticatingRealm authcRealm;
    authenticator.addRealm(authcRealm);

    AuthorizingRealm authzRealm;
    authorizer.addRealm(authzRealm);
}

SessionPtr SecurityManager::getSession(const std::string &sessionId){
    SessionPtr session = this->sessionManager->getSession(sessionId);
    if(session == NULL) return SessionPtr();
    return session;
}

const std::string &SecurityManager::login(AuthenticationToken &token){
    AuthenticationInfo info;
    if(!authenticate(token, &info)){
        onFailedLogin(token);
        return cetty::shiro::util::emptyString;
    }

    SessionPtr session = sessionManager->start();
    assert(session != NULL);

    bind(info, session);
    onSuccessfulLogin(token, info);

    return session->getId();
}

void SecurityManager::logout(const std::string &sessionId){
    beforeLogout(sessionId);

    SessionPtr session = getSession(sessionId);
    if(session != NULL){
        std::string userId = session->getAttribute(AuthenticationInfo::USER_ID);
        authenticator.onLogout(userId);
        session->stop();
    }
}

void SecurityManager::destroy(){
    if(sessionManager) {
        delete sessionManager;
        sessionManager = NULL;
    }
}

void SecurityManager::bind(AuthenticationInfo& info, const SessionPtr& session) {
    session->setAttribute(AuthenticationInfo::USER_ID, info.getUserId());
    session->setAttribute(AuthenticationInfo::CREDENTIALS, info.getCredentials());
    session->setAttribute(AuthenticationInfo::CREDENTIALS_SALT, info.getCredentialsSalt());
    session->setAttribute(AuthenticationInfo::CODE_TYPE, info.getCodeType());
}

}
}

