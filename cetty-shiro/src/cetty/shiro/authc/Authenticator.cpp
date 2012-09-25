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

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::shiro::realm;

void Authenticator::authenticate(const AuthenticationToken& token,
                                 const AuthenticateCallback& callback) {
    if (token.getPrincipal().empty()) {
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
    if (info && credentialsMatcher(token, *info)) {
        callback(info);
    }
    else {
        LOG_TRACE << "Authenticate failed for " << "["
                  << token.getPrincipal()
                  << "].";
        callback(AuthenticationInfoPtr());
    }
}

}
}
}
