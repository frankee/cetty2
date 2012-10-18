/*
 * Authenticator.cpp
 *
 *  Created on: 2012-8-20
 *      Author: chenhl
 */

#include <cetty/shiro/authc/Authenticator.h>

#include <boost/bind.hpp>

#include <cetty/shiro/authc/AuthenticationToken.h>
#include <cetty/shiro/realm/AuthenticatingRealm.h>
#include <cetty/shiro/authc/CredentialsMatcher.h>
#include <cetty/shiro/authc/HashedCredentialsMatcher.h>
#include <cetty/shiro/authc/SimpleCredentialsMatcher.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/config/ConfigCenter.h>

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::shiro::realm;

const std::string Authenticator::HASHEDCREDENTIALSMATCHER = "hashed_credentials_matcher";
const std::string Authenticator::SIMPLECREDENTIALSMATCHER = "simple_credentials_matcher";

Authenticator::Authenticator(): credentialsMatcher(NULL) {
    ConfigCenter::instance().configure(&config);
    init();
}

void Authenticator::init(){
    if(config.credentialsMatcher == HASHEDCREDENTIALSMATCHER){
        credentialsMatcher = new HashedCredentialsMatcher();
    } else if (config.credentialsMatcher == SIMPLECREDENTIALSMATCHER){
        credentialsMatcher = new SimpleCredentialsMatcher();
    }
}

void Authenticator::authenticate(const AuthenticationToken& token,
                                 const AuthenticateCallback& callback) {
    if (token.getPrincipal().empty()) {
        LOG_TRACE << "Principal of token is empty.";
        callback(AuthenticationInfoPtr());
        return;
    }

    if (realm) {
        realm->getAuthenticationInfo(token,
                                     boost::bind(&Authenticator::onGetAuthenticationInfo,
                                             this,
                                             _1,
                                             token,
                                             callback));
    }
    else {
        LOG_ERROR << "No realm configuration";
        callback(AuthenticationInfoPtr());
    }
}

void Authenticator::onGetAuthenticationInfo(const AuthenticationInfoPtr& info,
        const AuthenticationToken& token,
        const AuthenticateCallback& callback) {
    if (info && credentialsMatcher->match(token, *info)) {
        callback(info);
    }
    else {
        LOG_TRACE << "Authenticate failed for " << "["
                  << token.getPrincipal()
                  << "].";
        callback(AuthenticationInfoPtr());
    }
}

Authenticator::~Authenticator(){
    if(credentialsMatcher){
        delete credentialsMatcher;
        credentialsMatcher = NULL;
    }
}

}
}
}
