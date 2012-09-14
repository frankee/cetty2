/*
 * Authorizer.cpp
 *
 *  Created on: 2012-9-4
 *      Author: chenhl
 */

#include <cetty/shiro/authz/Authorizer.h>
#include <cetty/shiro/authz/Permission.h>
#include <cetty/shiro/realm/AuthorizingRealm.h>

namespace cetty {
namespace shiro {
namespace authz {

using namespace cetty::shiro::realm;

void Authorizer::isPermitted(const std::string& principal,
                             const std::string& permission,
                             const AuthorizeCallback& callback) const {
    if (realmConfigured()) {
        PrincipalCollection principals(principal, realm->getName());
        PermissionPtr p = permissionResolver(permission);
        isPermitted(principals, p, callback);
    }
}

void Authorizer::isPermitted(const PrincipalCollection& principals,
                             const std::string& permission,
                             const AuthorizeCallback& callback) const {
    if (realmConfigured()) {
        PermissionPtr p = permissionResolver(permission);
        isPermitted(principals, p, callback);
    }
}

void Authorizer::isPermitted(const PrincipalCollection& principal,
                             const PermissionPtr& permission,
                             const AuthorizeCallback& callback) const {

}

bool Authorizer::realmConfigured() const {
    if (!realm) {
        //String msg = "Configuration error:  No realms have been configured!  One or more realms must be " +
        //"present to execute an authorization operation.";
        //throw new IllegalStateException(msg);
        return false;
    }

    return true;
}

}
}
}


