/*
 * PermissionResolver.h
 *
 *  Created on: 2012-10-26
 *      Author: chenhl
 */

#ifndef PERMISSIONRESOLVER_H_
#define PERMISSIONRESOLVER_H_

#include <cetty/shiro/authz/PermissionPtr.h>
#include <cetty/shiro/authz/Permission.h>
#include <cetty/shiro/authz/AllPermission.h>
#include <cetty/shiro/authz/WildcardPermission.h>

namespace cetty {
namespace shiro {
namespace authz {

PermissionPtr allPermissionResolver(const std::string &permission){
    PermissionPtr pp = new AllPermission(permission);
    return pp;
}

PermissionPtr wildcardPermissionResolver(const std::string &permission){
    PermissionPtr pp = new WildcardPermission(permission);
    return pp;
}

}
}
}


#endif /* PERMISSIONRESOLVER_H_ */
