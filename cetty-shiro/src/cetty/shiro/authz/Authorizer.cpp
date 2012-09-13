/*
 * Authorizer.cpp
 *
 *  Created on: 2012-9-4
 *      Author: chenhl
 */

#include <cetty/shiro/authz/Authorizer.h>

namespace cetty {
namespace shiro {
namespace authz {


    bool Authorizer::isPermitted(const PrincipalCollection& principals, const std::string& permission) const {
        assertRealmsConfigured();
        PermissionPtr p = permissionResolver(permission);

        return isPermitted(principals, p);

        for (std::size_t i = 0, j = realm.size(); i < j; ++i) {

            if (((Authorizer) realm).isPermitted(principals, permission)) {
                return true;
            }
        }

        return false;
    }

}
}
}


