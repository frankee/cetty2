#if !defined(CETTY_SHIRO_AUTHZ_AUTHORIZER_H)
#define CETTY_SHIRO_AUTHZ_AUTHORIZER_H

/*
 * Authorizer.h
 *
 *  Created on: 2012-9-4
 *      Author: chenhl
 */

#include <vector>
#include <boost/function.hpp>

#include <cetty/shiro/PrincipalCollection.h>
#include <cetty/shiro/session/SessionPtr.h>
#include <cetty/shiro/realm/AuthorizingRealmPtr.h>

namespace cetty {
namespace shiro {
namespace authz {

using namespace cetty::shiro;
using namespace cetty::shiro::realm;
using namespace cetty::shiro::session;

/**
 * An <tt>Authorizer</tt> performs authorization (access control) operations for any given Subject
 * (aka 'application user').
 *
 * <p>Each method requires a subject principal to perform the action for the corresponding Subject/user.
 *
 * <p>This principal argument is usually an object representing a user database primary key or a String username or
 * something similar that uniquely identifies an application user.  The runtime value of the this principal
 * is application-specific and provided by the application's configured Realms.
 *
 * <p>Note that there are many *Permission methods in this interface overloaded to accept String arguments instead of
 * {@link Permission Permission} instances. They are a convenience allowing the caller to use a String representation of
 * a {@link Permission Permission} if desired.  Most implementations of this interface will simply convert these
 * String values to {@link Permission Permission} instances and then just call the corresponding type-safe method.
 * (Shiro's default implementations do String-to-Permission conversion for these methods using
 * {@link org.apache.shiro.authz.permission.PermissionResolver PermissionResolver}s.)
 *
 * <p>These overloaded *Permission methods <em>do</em> forego type-saftey for the benefit of convenience and simplicity,
 * so you should choose which ones to use based on your preferences and needs.
 *
 * @since 0.1
 */
class Authorizer {
public:
    typedef boost::funtion1<PermissionPtr, const std::string&> ResolvePermissionFunctor;

public:
    Authorizer() {}
    Authorizer(const RealmPtr& realm);
    Authorizer(const RealmPtr& realm, PermissionResolver&)

    /**
     * Constructor that accepts the <code>Realm</code>s to consult during an authorization check.  Immediately calls
     * {@link #setRealms setRealms(realms)}.
     *
     * @param realms the realms to consult during an authorization check.
     */
    Authorizer(const std::vector<RealmPtr>& realms) {
        setRealms(realms);
    }

    virtual ~Authorizer() {}

    /**
     * Returns <tt>true</tt> if the corresponding subject/user is permitted to perform an action or access a resource
     * summarized by the specified permission string.
     *
     * <p>This is an overloaded method for the corresponding type-safe {@link Permission Permission} variant.
     * Please see the class-level JavaDoc for more information on these String-based permission methods.
     *
     * @param principals the application-specific subject/user identifier.
     * @param permission the String representation of a Permission that is being checked.
     * @return true if the corresponding Subject/user is permitted, false otherwise.
     * @see #isPermitted(PrincipalCollection principals,Permission permission)
     * @since 0.9
     */
    bool isPermitted(const PrincipalCollection& principals,
                     const std::string& permission) const {
                         assertRealmsConfigured();

                         for (Realm realm : getRealms()) {
                             if (!(realm instanceof Authorizer)) { continue; }

                             if (((Authorizer) realm).isPermitted(principals, permission)) {
                                 return true;
                             }
                         }

                         return false;
    }

    /**
     * Returns <tt>true</tt> if the corresponding subject/user is permitted to perform an action or access a resource
     * summarized by the specified permission.
     *
     * <p>More specifically, this method determines if any <tt>Permission</tt>s associated
     * with the subject {@link Permission#implies(Permission) imply} the specified permission.
     *
     * @param subjectPrincipal the application-specific subject/user identifier.
     * @param permission       the permission that is being checked.
     * @return true if the corresponding Subject/user is permitted, false otherwise.
     */
    bool isPermitted(const PrincipalCollection& subjectPrincipal,
                     const PermissionPtr& permission) const;

    /**
     * Checks if the corresponding Subject implies the given permission strings and returns a boolean array
     * indicating which permissions are implied.
     *
     * <p>This is an overloaded method for the corresponding type-safe {@link Permission Permission} variant.
     * Please see the class-level JavaDoc for more information on these String-based permission methods.
     *
     * @param subjectPrincipal the application-specific subject/user identifier.
     * @param permissions      the String representations of the Permissions that are being checked.
     * @return an array of booleans whose indices correspond to the index of the
     *         permissions in the given list.  A true value at an index indicates the user is permitted for
     *         for the associated <tt>Permission</tt> string in the list.  A false value at an index
     *         indicates otherwise.
     * @since 0.9
     */
    void isPermitted(const PrincipalCollection& subjectPrincipal,
                     const std::vector<std::string>& permissions,
                     std::vector<bool> permitFlags) const;

    /**
     * Checks if the corresponding Subject/user implies the given Permissions and returns a boolean array indicating
     * which permissions are implied.
     *
     * <p>More specifically, this method should determine if each <tt>Permission</tt> in
     * the array is {@link Permission#implies(Permission) implied} by permissions
     * already associated with the subject.
     *
     * <p>This is primarily a performance-enhancing method to help reduce the number of
     * {@link #isPermitted} invocations over the wire in client/server systems.
     *
     * @param subjectPrincipal the application-specific subject/user identifier.
     * @param permissions      the permissions that are being checked.
     * @return an array of booleans whose indices correspond to the index of the
     *         permissions in the given list.  A true value at an index indicates the user is permitted for
     *         for the associated <tt>Permission</tt> object in the list.  A false value at an index
     *         indicates otherwise.
     */
    void isPermitted(const PrincipalCollection& subjectPrincipal,
                     const std::vector<PermissionPtr>& permissions,
                     std::vector<bool> permitFlags) const;

    /**
     * Returns <tt>true</tt> if the corresponding Subject/user implies all of the specified permissions, <tt>false</tt>
     * otherwise.
     *
     * <p>More specifically, this method determines if all of the given <tt>Permission</tt>s are
     * {@link Permission#implies(Permission) implied by} permissions already associated with the subject.
     *
     * @param subjectPrincipal the application-specific subject/user identifier.
     * @param permissions      the permissions to check.
     * @return true if the user has all of the specified permissions, false otherwise.
     */
    bool isPermittedAll(const PrincipalCollection& principal,
                        const std::vector<std::string>& permissions) const;

    /**
     * Returns <tt>true</tt> if the corresponding Subject/user implies all of the specified permissions, <tt>false</tt>
     * otherwise.
     *
     * <p>More specifically, this method determines if all of the given <tt>Permission</tt>s are
     * {@link Permission#implies(Permission) implied by} permissions already associated with the subject.
     *
     * @param subjectPrincipal the application-specific subject/user identifier.
     * @param permissions      the permissions to check.
     * @return true if the user has all of the specified permissions, false otherwise.
     */
    bool isPermittedAll(const PrincipalCollection& principal,
                        const std::vector<PermissionPtr>& permissions) const;

    /**
     * Returns <tt>true</tt> if the corresponding Subject/user has the specified role, <tt>false</tt> otherwise.
     *
     * @param subjectPrincipal the application-specific subject/user identifier.
     * @param roleIdentifier   the application-specific role identifier (usually a role id or role name).
     * @return <tt>true</tt> if the corresponding subject has the specified role, <tt>false</tt> otherwise.
     */
    bool hasRole(const PrincipalCollection& principal,
                 const std::string& roleIdentifier) const;

    /**
     * Checks if the corresponding Subject/user has the specified roles, returning a boolean array indicating
     * which roles are associated with the given subject.
     *
     * <p>This is primarily a performance-enhancing method to help reduce the number of
     * {@link #hasRole} invocations over the wire in client/server systems.
     *
     * @param subjectPrincipal the application-specific subject/user identifier.
     * @param roleIdentifiers  the application-specific role identifiers to check (usually role ids or role names).
     * @return an array of booleans whose indices correspond to the index of the
     *         roles in the given identifiers.  A true value indicates the user has the
     *         role at that index.  False indicates the user does not have the role at that index.
     */
    void hasRoles(const PrincipalCollection& principal,
                  const std::vector<std::string>& roleIdentifiers,
                  std::vector<bool>* roleFlags) const;

    /**
     * Returns <tt>true</tt> if the corresponding Subject/user has all of the specified roles, <tt>false</tt> otherwise.
     *
     * @param subjectPrincipal the application-specific subject/user identifier.
     * @param roleIdentifiers  the application-specific role identifiers to check (usually role ids or role names).
     * @return true if the user has all the roles, false otherwise.
     */
    bool hasAllRoles(const PrincipalCollection& subjectPrincipal,
                     const std::vector<std::string>& roleIdentifiers) const;


    /**
     * Returns the realms wrapped by this <code>Authorizer</code> which are consulted during an authorization check.
     *
     * @return the realms wrapped by this <code>Authorizer</code> which are consulted during an authorization check.
     */
    const std::vector<RealmPtr>& getRealms() const {
    }

    /**
     * Sets the realms wrapped by this <code>Authorizer</code> which are consulted during an authorization check.
     *
     * @param realms the realms wrapped by this <code>Authorizer</code> which are consulted during an authorization check.
     */
    void setRealms(const std::vector<RealmPtr>& realms) {
        this.realms = realms;
    }

    void setRealm(const RealmPtr& realm) {

    }

    /**
     * Sets the specified {@link PermissionResolver PermissionResolver} on <em>all</em> of the wrapped realms that
     * implement the {@link org.apache.shiro.authz.permission.PermissionResolverAware PermissionResolverAware} interface.
     * <p/>
     * Only call this method if you want the permission resolver to be passed to all realms that implement the
     * <code>PermissionResolver</code> interface.  If you do not want this to occur, the realms must
     * configure themselves individually (or be configured individually).
     *
     * @param permissionResolver the permissionResolver to set on all of the wrapped realms that implement the
     *                           {@link org.apache.shiro.authz.permission.PermissionResolverAware PermissionResolverAware} interface.
     */
    void setPermissionResolver(const ResolvePermissionFunctor& permissionResolver) {
        this.permissionResolver = permissionResolver;
    }

private:
    /**
     * Used by the {@link Authorizer Authorizer} implementation methods to ensure that the {@link #setRealms realms}
     * has been set.  The default implementation ensures the property is not null and not empty.
     *
     * @throws IllegalStateException if the <tt>realms</tt> property is configured incorrectly.
     */
    void assertRealmsConfigured() {
        if (realms.empty()) {
            //String msg = "Configuration error:  No realms have been configured!  One or more realms must be " +
            //"present to execute an authorization operation.";
            //throw new IllegalStateException(msg);
        }
    }

protected:
    /**
     * The realms to consult during any authorization check.
     */
    std::vector<AuthorizingRealmPtr> realms;

    /**
     * A PermissionResolver to be used by <em>all</em> configured realms.  Leave <code>null</code> if you wish
     * to configure different resolvers for different realms.
     */
    ResolvePermissionFunctor permissionResolver;
};

}
}
}




#endif // !defined(CETTY_SHIRO_AUTHZ_AUTHORIZER_H)
