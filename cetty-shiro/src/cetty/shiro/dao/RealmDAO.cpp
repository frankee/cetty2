/*
 * RealmDAO.cpp
 *
 *  Created on: 2012-9-27
 *      Author: chenhl
 */


#include <cetty/shiro/dao/RealmDAO.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace shiro {
namespace dao {

AuthenticationInfoPtr
RealmDAO::QueryAuthenticationInfo(const std::string &principal,
                                  const std::string &realmName){
    soci::session &sql = getSql();
    AuthenticationInfoPtr info;

    try {
        // todo get authc info from sqlite

    } catch (const soci::soci_error &e){
        LOG_ERROR << e.what();
    }

    return info;
}

AuthorizationInfoPtr
RealmDAO::QueryAuthorizationInfo(const std::string &principal){
    soci::session &sql = getSql();
    AuthorizationInfoPtr info;

    try {
        // todo get authz info from sqlite

    } catch (const soci::soci_error &e){
        LOG_ERROR << e.what();
    }

    return info;
}

}
}
}

