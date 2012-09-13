#if !defined(CETTY_SHIRO_AUTHZ_AUTHORIZATIONINFO_H)
#define CETTY_SHIRO_AUTHZ_AUTHORIZATIONINFO_H
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <vector>
#include <string>
#include <cetty/shiro/authz/PermissionPtr.h>
#include <cetty/shiro/authz/AuthorizationInfoPtr.h>

namespace cetty {
namespace shiro {
namespace authz {

/**
 * <code>AuthorizationInfo</code> represents a single Subject's stored authorization data (roles, permissions, etc)
 * used during authorization (access control) checks only.
 * <p/>
 * Roles are represented as a <code>Collection</code> of Strings
 * ({@link java.util.Collection Collection}<{@link String String}>), typically each element being the Role name.
 * <p/>
 * {@link Permission Permission}s are provided in two ways:
 * <ul>
 * <li>A <code>Collection</code> of Strings, where each String can usually be converted into <code>Permission</code>
 * objects by a <code>Realm</code>'s
 * {@link org.apache.shiro.authz.permission.PermissionResolver PermissionResolver}</li>
 * <li>A <code>Collection</code> of {@link Permission Permission} objects</li>
 * </ul>
 * Both permission collections together represent the total aggregate collection of permissions.  You may use one
 * or both depending on your preference and needs.
 * <p/>
 * Because the act of authorization (access control) is orthoganal to authentication (log-in), this interface is
 * intended to represent only the account data needed by Shiro during an access control check
 * (role, permission, etc).  Shiro also has a parallel
 * {@link org.apache.shiro.authc.AuthenticationInfo AuthenticationInfo} interface for use during the authentication
 * process that represents identity data such as principals and credentials.
 * <p/>
 * Because many if not most {@link org.apache.shiro.realm.Realm Realm}s store both sets of data for a Subject, it might be
 * convenient for a <code>Realm</code> implementation to utilize an implementation of the
 * {@link org.apache.shiro.authc.Account Account} interface instead, which is a convenience interface that combines both
 * <code>AuthenticationInfo</code> and <code>AuthorizationInfo</code>.  Whether you choose to implement these two
 * interfaces separately or implement the one <code>Account</code> interface for a given <code>Realm</code> is
 * entirely based on your application's needs or your preferences.
 *
 * @see org.apache.shiro.authc.AuthenticationInfo AuthenticationInfo
 * @see org.apache.shiro.authc.Account
 * @since 0.9
 */
class AuthorizationInfo {
public:
    AuthorizationInfo();
    AuthorizationInfo(const std::vector<std::string>& roles);

    /**
     * Returns the names of all roles assigned to a corresponding Subject.
     *
     * @return the names of all roles assigned to a corresponding Subject.
     */
    const std::vector<std::string>& getRoles() const;

    /**
     * Sets the roles assigned to the account.
     * @param roles the roles assigned to the account.
     */
    void setRoles(const std::vector<std::string>& roles);

    template<typename I>
    void setRoles(const I& begin, const I& end);

    /**
     * Adds (assigns) a role to those associated with the account.  If the account doesn't yet have any roles, a
     * new roles collection (a Set) will be created automatically.
     * @param role the role to add to those associated with the account.
     */
    void addRole(const std::string& role);

    template<typename I>
    void addRoles(const I& begin, const I& end);

    /**
     * Returns all string-based permissions assigned to the corresponding Subject.  The permissions here plus those
     * returned from {@link #getObjectPermissions() getObjectPermissions()} represent the total set of permissions
     * assigned.  The aggregate set is used to perform a permission authorization check.
     * <p/>
     * This method is a convenience mechanism that allows Realms to represent permissions as Strings if they choose.
     * When performing a security check, a <code>Realm</code> usually converts these strings to object
     * {@link Permission Permission}s via an internal
     * {@link org.apache.shiro.authz.permission.PermissionResolver PermissionResolver}
     * in order to perform the actual permission check.  This is not a requirement of course, since <code>Realm</code>s
     * can perform security checks in whatever manner deemed necessary, but this explains the conversion mechanism that
     * most Shiro Realms execute for string-based permission checks.
     *
     * @return all string-based permissions assigned to the corresponding Subject.
     */
    const std::vector<std::string>& getStringPermissions() const;


    void setStringPermissions(const std::vector<std::string>& permissions);

    template<typename I>
    void setStringPermissions(const I& begin, const I& end);

    /**
     * Returns all type-safe {@link Permission Permission}s assigned to the corresponding Subject.  The permissions
     * returned from this method plus any returned from {@link #getStringPermissions() getStringPermissions()}
     * represent the total set of permissions.  The aggregate set is used to perform a permission authorization check.
     *
     * @return all type-safe {@link Permission Permission}s assigned to the corresponding Subject.
     */
    const std::vector<PermissionPtr>& getPermissions() const;
    
private:
    std::vector<std::string>   roles;
    std::vector<PermissionPtr> permissions;
    std::vector<std::string>   stringPermissions;
};

}
}
}

#endif //#if !defined(CETTY_SHIRO_AUTHZ_AUTHORIZATIONINFO_H)

// Local Variables:
// mode: c++
// End:
