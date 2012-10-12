/*
 * SecurityManager.cpp
 *
 *  Created on: 2012-8-22
 *      Author: chenhl
 */

#include <cetty/shiro/SecurityManager.h>


#include <cetty/shiro/session/Session.h>
#include <cetty/shiro/authc/AuthenticationToken.h>
#include <cetty/shiro/realm/AuthorizingRealm.h>
#include <cetty/shiro/realm/SqlRealm.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/config/ConfigCenter.h>

#include <boost/bind.hpp>

namespace cetty {
namespace shiro {

using namespace cetty::shiro::session;
using namespace cetty::shiro::authc;
using namespace cetty::shiro::realm;
using namespace cetty::shiro::authz;

const std::string SecurityManager::SQL_REALM = "sql_realm";

SecurityManager::SecurityManager(){
    sessionManager = new SessionManager();

    ConfigCenter::instance().configure(&config);
    init();
}

void SecurityManager::init(){
    std::string realm = config.realm;
    if(realm == SQL_REALM){
        realms = new SqlRealm();
    } else {
        LOG_ERROR << "No realm is named [" << realm << "]";
    }

    authenticator.setRealm(boost::dynamic_pointer_cast<AuthenticatingRealm>(realms));
    authorizer.setRealm(boost::dynamic_pointer_cast<AuthorizingRealm>(realms));
}

SecurityManager::~SecurityManager(){
    if(sessionManager != NULL){
        delete sessionManager;
        sessionManager = NULL;
    }
}

void SecurityManager::login(const AuthenticationToken& token,
                            const LoginCallback& callback) {
    authenticator.authenticate(token,
                               boost::bind(&SecurityManager::onAuthenticate,
                                           this,
                                           _1,
                                           token,
                                           callback));
}

void SecurityManager::onAuthenticate(const AuthenticationInfoPtr& info,
                                     const AuthenticationToken& token,
                                     const LoginCallback& callback) {
    if (info) {
        sessionManager->start(token.getHost(),
                              boost::bind(&SecurityManager::onStartSession,
                                          this,
                                          _1,
                                          info,
                                          token,
                                          callback));
    }
    else {
        fireFailedLoginEvent(token);
    }
}

void SecurityManager::onStartSession(const SessionPtr& session,
                                     const AuthenticationInfoPtr& info,
                                     const AuthenticationToken& token,
                                     const LoginCallback& callback) {
    if (session) {
        bind(token, *info, session);
        fireSuccessfulLoginEvent(token, info);
        callback(0, token, info, session);
    }
    else {
        fireFailedLoginEvent(token);
        callback(1, token, info, session);
    }
}

void SecurityManager::getSession(const std::string& id,
                                 SessionManager::SessionCallback callback ){
    sessionManager->getSession(id, callback);
}

void SecurityManager::logout(const std::string& sessionId) {
    sessionManager->getSession(sessionId,
                               boost::bind(&SecurityManager::onLogout,
                                           this,
                                           _1));
}

void SecurityManager::onLogout(const SessionPtr& session){
    AuthenticatingRealm *authRealm = (AuthenticatingRealm *)realms.get();
    if(authRealm != NULL) authRealm->onLogout(session->getAttribute("username"));
    session->stop();
}


void SecurityManager::bind(const AuthenticationToken& token,
                           const AuthenticationInfo& info,
                           const SessionPtr& session) {
    session->setHost(token.getHost());
    session->setLogin(true);
}


SessionManager* SecurityManager::getSessionManager(){
    return sessionManager;
}

void SecurityManager::fireFailedLoginEvent(const AuthenticationToken& token){
    LOG_TRACE << token.getPrincipal()
              << " is failed to login.";
}
void SecurityManager::fireSuccessfulLoginEvent(const AuthenticationToken& token,
                                               const AuthenticationInfoPtr& info){
    LOG_TRACE << token.getPrincipal()
              << " is successful to login.";
}

void SecurityManager::isPermitted(const std::string& principal,
                                  const std::string& permission,
                                  const AuthorizeCallback& callback) const{
    authorizer.isPermitted(principal, permission, callback);
}

void SecurityManager::isPermitted(const PrincipalCollection& principals,
                                  const std::string& permission,
                                  const AuthorizeCallback& callback) const{
    authorizer.isPermitted(principals, permission, callback);
}

void SecurityManager::isPermitted(const PrincipalCollection& principal,
                                  const PermissionPtr& permission,
                                  const AuthorizeCallback& callback) const{
    authorizer.isPermitted(principal, permission, callback);
}

void SecurityManager::setRealm(const RealmPtr &realm){
    this->realms = realm;
}

const RealmPtr& SecurityManager::getRealms() const{
    return this->realms;
}

}
}
