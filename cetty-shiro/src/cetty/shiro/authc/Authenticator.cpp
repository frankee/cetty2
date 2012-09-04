/*
 * Authenticator.cpp
 *
 *  Created on: 2012-8-20
 *      Author: chenhl
 */

#include <cetty/shiro/authc/Authenticator.h>
#include <cetty/shiro/authc/AuthenticationToken.h>

namespace cetty {
namespace shiro {
namespace authc {
bool Authenticator::authenticate(const AuthenticationToken &token,
                                 AuthenticationInfo *info){
    if(token.getPrincipal().empty()) return false;
    if(!doAuthenticate(token, info)) {
        notifyFailure(token);
        return false;
    }
    notifySuccess(token, *info);
    return true;
}


}
}
}


