/*
 * SecurityManager.cpp
 *
 *  Created on: 2012-8-22
 *      Author: chenhl
 */

#include <cetty/shiro/SecurityManager.h>

#include <boost/bind.hpp>
#include <cetty/shiro/session/Session.h>
#include <cetty/shiro/authc/AuthenticationToken.h>
#include <cetty/shiro/realm/AuthorizingRealm.h>

namespace cetty {
namespace shiro {

using namespace cetty::shiro::session;
using namespace cetty::shiro::authc;
using namespace cetty::shiro::realm;
using namespace cetty::shiro::authz;

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
        //bind(info, session);
        fireSuccessfulLoginEvent(token, info);
    }
    else {
        fireFailedLoginEvent(token);
    }
}

void SecurityManager::logout(const std::string& sessionId) {
    //beforeLogout(sessionId);

    //SessionPtr session = getSession(sessionId);

    //if (session != NULL) {
    //    std::string userId = session->getAttribute(AuthenticationInfo::USER_ID);
    //    authenticator.onLogout(userId);
    //    session->stop();
    //}
}

void SecurityManager::bind(AuthenticationInfo& info, const SessionPtr& session) {
    //session->setAttribute(AuthenticationInfo::USER_ID, info.getUserId());
    //session->setAttribute(AuthenticationInfo::CREDENTIALS, info.getCredentials());
    //session->setAttribute(AuthenticationInfo::CREDENTIALS_SALT, info.getCredentialsSalt());
    //session->setAttribute(AuthenticationInfo::CODE_TYPE, info.getCodeType());
}

}
}

