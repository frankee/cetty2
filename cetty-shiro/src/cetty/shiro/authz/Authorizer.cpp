/*
 * Authorizer.cpp
 *
 *  Created on: 2012-9-4
 *      Author: chenhl
 */

#include <cetty/shiro/authz/Authorizer.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/shiro/authz/PermissionResolver.h>
#include <cetty/shiro/authz/AuthorizationInfoPtr.h>
#include <cetty/shiro/authz/AuthorizationInfo.h>

namespace cetty {
namespace shiro {
namespace authz {

using namespace cetty::config;
using namespace cetty::shiro::realm;

const std::string Authorizer::ALL_PERMISSION = "all_permission";
const std::string Authorizer::WILDCARD_PERMISSION = "wildcard_permission";

Authorizer::Authorizer(){
    ConfigCenter::instance().configure(&config);

    if(config.permissionType == WILDCARD_PERMISSION){
        permissionResolver = &wildcardPermissionResolver;
    } else {
        permissionResolver = &allPermissionResolver;
    }
}

void Authorizer::isPermitted(const std::string& principal,
                             const std::string& permission,
                             const AuthorizeCallback& callback) const {
    assert(realmConfigured());

    PrincipalCollection principals(principal, realm->getName());
    PermissionPtr p = permissionResolver(permission);
    isPermitted(principals, p, callback);
}

void Authorizer::isPermitted(const PrincipalCollection& principals,
                             const std::string& permission,
                             const AuthorizeCallback& callback) const {
    assert(realmConfigured());

    PermissionPtr p = permissionResolver(permission);
    isPermitted(principals, p, callback);
}

void Authorizer::isPermitted(const PrincipalCollection& principal,
                             const PermissionPtr& permission,
                             const AuthorizeCallback& callback) const {
    realm->getAuthorizationInfo(principal,
                                boost::bind(
                                    &Authorizer::doPermite,
                                    this,
                                    _1, // AuthorizationInfoPtr
                                    principal,
                                    permission,
                                    callback));
}

void Authorizer::doPermite(AuthorizationInfoPtr info,
                           PrincipalCollection principal,
                           PermissionPtr permission,
                           AuthorizeCallback callback) const {
    if (!callback) {
        LOG_ERROR << "doPermite input callback can NOT be NULL";
        return;
    }

    if(!info){
        LOG_TRACE<< "Can't find account info for principal ["
                 << principal.getPrimaryPrincipal() << "]";
        callback(false, principal.getPrimaryPrincipal(), permission->stringPermission());
    }

    if(config.permissionType == ALL_PERMISSION)
        callback(true, principal.getPrimaryPrincipal(), permission->stringPermission());

    std::vector<PermissionPtr> permissions = info->getPermissions();
    std::vector<PermissionPtr>::iterator it;

    for (it = permissions.begin(); it != permissions.end(); ++it) {
        if ((*it)->implies(permission)) {
            LOG_TRACE << "[" << principal.getPrimaryPrincipal() << "]"
                <<"is permited for permission [" << permission->stringPermission() << "].";
            callback(true, principal.getPrimaryPrincipal(), permission->stringPermission());
            return;
        }
    }

    std::vector<std::string> strPermissions = info->getStringPermissions();
    std::vector<std::string>::iterator begin = strPermissions.begin();

    for (; begin != strPermissions.end(); ++begin) {
        PermissionPtr pm = permissionResolver(*begin);

        if (pm->implies(permission)) {
            LOG_TRACE << "[" << principal.getPrimaryPrincipal() << "]"
                      <<"is permited for permission [" << permission->stringPermission() << "].";
            callback(true, principal.getPrimaryPrincipal(), permission->stringPermission());
            return;
        }
    }

    LOG_TRACE << "[" << principal.getPrimaryPrincipal() << "]"
              <<"is not permited for permission [" << permission->stringPermission() << "].";

    callback(false, principal.getPrimaryPrincipal(), permission->stringPermission());
}

bool Authorizer::realmConfigured() const {
    if (!realm) {
        LOG_TRACE << "Configuration error:  No realms have been configured!  One or more realms must be "
                  << "present to execute an authorization operation.";
        return false;
    }

    return true;
}

}
}
}
