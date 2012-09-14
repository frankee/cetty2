/*
 * Authenticator.cpp
 *
 *  Created on: 2012-8-20
 *      Author: chenhl
 */

#include <cetty/shiro/authc/Authenticator.h>
#include <cetty/shiro/authc/AuthenticationToken.h>
#include <cetty/shiro/realm/AuthenticatingRealm.h>

namespace cetty {
namespace shiro {
namespace authc {

    using namespace cetty::shiro::realm;

    void Authenticator::authenticate(const AuthenticationToken &token,
        const AuthenticateCallback& callback) {
    if(token.getPrincipal().empty()) {
        return;
    }

    //if(!doAuthenticate(token, info)) {
    //    notifyFailure(token);
    //    return false;
    //}

    //notifySuccess(token, *info);
}

}
}
}


