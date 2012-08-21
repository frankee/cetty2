/*
 * ModularRealmAuthenticator.cpp
 *
 *  Created on: 2012-8-21
 *      Author: chenhl
 */

#include <assert.h>

#include <cetty/shiro/authc/ModularRealmAuthenticator.h>
#include <cetty/shiro/realm/AuthenticatingRealm.h>

namespace cetty {
namespace shiro {
namespace authc {

void ModularRealmAuthenticator::onLogout(const PrincipalCollection &principals){
    Authenticator::onLogout(principals);
    std::vector<AuthenticatingRealm>::iterator it;
    for(it = realms.begin(); it != realms.end(); ++it)
        it->onLogout(principals);
}

bool ModularRealmAuthenticator::doAuthenticate(const AuthenticationToken &token,
                                               AuthenticationInfo *info){
    assert(assertRealmsConfigured());
    std::vector<AuthenticatingRealm>::iterator it;
    for(it = realms.begin(); it != realms.end(); ++it)
        if(it->getAuthenticationInfo(token, info)) return true;
    return false;
}

}
}
}


