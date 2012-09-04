/*
 * AuthenticatingRealm.cpp
 *
 *  Created on: 2012-8-20
 *      Author: chenhl
 */

#include <cetty/shiro/realm/AuthenticatingRealm.h>

#include <cetty/shiro/dao/RedisDao.h>

namespace cetty {
namespace shiro {
namespace realm {
bool AuthenticatingRealm::getAuthenticationInfo(
    const AuthenticationToken &token,
    AuthenticationInfo *info)
{
    if(!doGetAuthenticationInfo(token, info)) return false;

    Sha256CredentialsMatcher *mch = getCredentialsMatcher();
    if(mch == NULL) return false;

    if(!mch->doCredentialsMatch(token, *info)) return false;
    return true;
}

bool AuthenticatingRealm::doGetAuthenticationInfo(
    const AuthenticationToken &token,
    AuthenticationInfo *info)
{
    std::string password = ::getPassword(token.getPrincipal());

    if(password == token.getCredentials()){
        PrincipalCollection pc;
        pc.add(token.getPrincipal(), getName());
        info->setPrincipals(pc);
        info->setCredentials(password);
        return true;
    }

    return false;
}


}
}
}


