/*
 * AuthorisingRealm.cpp
 *
 *  Created on: 2012-9-4
 *      Author: chenhl
 */



#include <cetty/shiro/realm/AuthorizingRealm.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/shiro/PrincipalCollection.h>
#include <cetty/shiro/authc/AuthenticationInfo.h>

namespace cetty {
namespace shiro {
namespace realm {

using namespace cetty::shiro;
using namespace cetty::shiro::authc;

void AuthorizingRealm::getAuthorizationInfo(const PrincipalCollection& principals, const GetAuthorizationInfoCallback& callback) {
    if (!callback) {
        return;
    }

    if (principals.isEmpty()) {
        callback(AuthorizationInfoPtr());
    }

    LOG_TRACE << "Retrieving AuthorizationInfo for principals ["
              << principals.getPrimaryPrincipal()
              << "]";

    AuthorizationInfoPtr info = AuthorizationInfoPtr();

    LOG_TRACE << "Attempting to retrieve the AuthorizationInfo from cache.";
    std::map<std::string, AuthorizationInfoPtr>::iterator itr =
        authorizations.find(principals.getPrimaryPrincipal());

    if (itr != authorizations.end()) {
        info = itr->second;
        LOG_TRACE << "AuthorizationInfo found in cache for principals ["
                  << principals.getPrimaryPrincipal()
                  << "]";
    }
    else {
        LOG_TRACE << "No AuthorizationInfo found in cache for principals ["
                  << principals.getPrimaryPrincipal()
                  << "]";
    }

    if (!info) {
        // Call template method if the info was not found in a cache
        doGetAuthorizationInfo(principals,
                               boost::bind(&AuthorizingRealm::onGetAuthorizationInfo,
                                           this,
                                           _1,
                                           principals,
                                           callback));
    }
    else {
        callback(info);
    }
}

void AuthorizingRealm::onGetAuthorizationInfo(const AuthorizationInfoPtr& info,
        const PrincipalCollection& principals,
        const GetAuthorizationInfoCallback& callback) {
    // If the info is not null and the cache has been created, then cache the authorization info.
    if (info) {
        LOG_TRACE << "Caching authorization info for principals: ["
                  << principals.getPrimaryPrincipal()
                  << "].";

        authorizations.insert(std::make_pair(principals.getPrimaryPrincipal(), info));
        callback(info);
    }

    else{
        LOG_TRACE << "Can't find authorization info of principals: ["
                  << principals.getPrimaryPrincipal()
                  <<"].";
        callback(AuthorizationInfoPtr());
    }


}

}
}
}
