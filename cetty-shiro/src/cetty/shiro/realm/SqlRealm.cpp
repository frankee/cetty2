/*
 * SqlRealm.cpp
 *
 *  Created on: 2012-9-19
 *      Author: chenhl
 */



#include <cetty/shiro/realm/SqlRealm.h>
#include <cetty/shiro/authc/AuthenticationToken.h>

namespace cetty {
namespace shiro {
namespace realm {

void SqlRealm::getAuthenticationInfo(const AuthenticationToken& token,
                                     const GetAuthenticationInfoCallback& callback){
    std::string principal = token.getPrincipal();

    // todo get authenticationInfo from data source

}

void SqlRealm::onGetAuthenticationInfo(// const AuthenticationToken& token,
                                       const GetAuthenticationInfoCallback& callback){

}

void SqlRealm::doGetAuthorizationInfo(const PrincipalCollection& principals,
                                      const GetAuthorizationInfoCallback& callback){
    std::string principle = principals.getPrimaryPrincipal();

}

void SqlRealm::onGetAuthorizationInfo(//const PrincipalCollection& principals,
                                      const GetAuthorizationInfoCallback& callback){

}

}
}
}
