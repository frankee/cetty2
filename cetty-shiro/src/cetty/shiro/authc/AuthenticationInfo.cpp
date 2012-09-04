/*
 * AuthenticationInfo.cpp
 *
 *  Created on: 2012-8-17
 *      Author: chenhl
 */
#include <cetty/shiro/authc/AuthenticationInfo.h>

namespace cetty {
namespace shiro {
namespace authc {
AuthenticationInfo::AuthenticationInfo(
        const std::string &principal,
        const std::string &credentials,
        const std::string &realmName){
    this->principals.add(principal, realmName);
    this->credentials = credentials;
}

AuthenticationInfo::AuthenticationInfo(
        const std::string &principal,
        const std::string &hashedCredentials,
        const ByteSource &credentialsSalt,
        const std::string &realmName){
    this->principals.add(principal, realmName);
    this->credentials = hashedCredentials;
    this->credentialsSalt = credentialsSalt;
}

AuthenticationInfo::AuthenticationInfo(
        const PrincipalCollection &principals,
        const std::string &credentials){
    this->principals = principals;
    this->credentials = credentials;
}

AuthenticationInfo::AuthenticationInfo(
        const PrincipalCollection &principals,
        const std::string &hashedCredentials,
        const ByteSource &credentialsSalt){
    this->principals = principals;
    this->credentials = hashedCredentials;
    this->credentialsSalt = credentialsSalt;
}


}
}
}



