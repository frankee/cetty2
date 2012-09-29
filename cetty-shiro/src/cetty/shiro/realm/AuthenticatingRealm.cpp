/*
 * AuthenticatingRealm.cpp
 *
 *  Created on: 2012-8-20
 *      Author: chenhl
 */

#include <cetty/shiro/realm/AuthenticatingRealm.h>
#include <cetty/shiro/authc/AuthenticationToken.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace shiro {
namespace realm {

void AuthenticatingRealm::getAuthenticationInfo(const AuthenticationToken& token,
                                                const GetAuthenticationInfoCallback& callback){
    if (!callback) {
        LOG_ERROR << "Call back can't be null";
        return;
    }

    if (token.getPrincipal().empty()) {
        callback(AuthenticationInfoPtr());
    }

    LOG_TRACE << "Retrieving AuthenticationInfo for principal ["
              << token.getPrincipal()
              << "]";

    AuthenticationInfoPtr info = AuthenticationInfoPtr();

    LOG_TRACE << "Attempting to retrieve the AuthenticationInfo from cache.";
    std::map<std::string, AuthenticationInfoPtr>::iterator itr =
        infos.find(token.getPrincipal());

    if (itr != infos.end()) {
        info = itr->second;
        LOG_TRACE << "AuthenticationInfo found in cache for principals ["
                  << token.getPrincipal() << "]";
    }
    else {
        LOG_TRACE << "No AuthenticationInfo  found in cache for principals ["
                  << token.getPrincipal() << "]";
    }

    if (!info) {
        // Call template method if the info was not found in a cache
        doGetAuthenticationInfo(token,
                                boost::bind(&AuthenticatingRealm::onGetAuthenticationInfo,
                                            this,
                                            token,
                                            _1,
                                            callback));
    }
    else {
        callback(info);
    }
}

void AuthenticatingRealm::onGetAuthenticationInfo(const AuthenticationToken& token,
                                                  const AuthenticationInfoPtr& info,
                                                  const GetAuthenticationInfoCallback& callback){
    if (info) {
        LOG_TRACE << "Caching authorization info for principals: ["
                  << token.getPrincipal() << "].";

        infos.insert(std::make_pair(token.getPrincipal(), info));
    }

    else{
        LOG_TRACE << "Can't find authorization info of principals: ["
                  << token.getPrincipal() <<"].";
    }

    callback(info);
}

}
}
}


