#if !defined(CETTY_SHIRO_AUTHZ_AUTHORIZER_H)
#define CETTY_SHIRO_AUTHZ_AUTHORIZER_H

#include <vector>
#include <boost/function.hpp>

#include <cetty/shiro/authz/Permission.h>
#include <cetty/shiro/authz/PermissionPtr.h>
#include <cetty/shiro/PrincipalCollection.h>
#include <cetty/shiro/realm/AuthorizingRealm.h>
#include <cetty/shiro/realm/AuthorizingRealmPtr.h>

namespace cetty {
namespace shiro {
namespace authz {

using namespace cetty::shiro;
using namespace cetty::shiro::realm;
using namespace cetty::shiro::authz;

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
    typedef boost::function1<PermissionPtr, const std::string&> ResolvePermissionFunctor;
    typedef boost::function3<void, bool, const std::string&, const std::string&> AuthorizeCallback;

public:
    Authorizer() {}

    /**
     * Constructor that accepts the <code>Realm</code>s to consult during an authorization check.  Immediately calls
     * {@link #setRealms setRealms(realms)}.
     *
     * @param realms the realms to consult during an authorization check.
     */
    Authorizer(const AuthorizingRealmPtr& realm):realm(realm){}
    Authorizer(const AuthorizingRealmPtr& realm, const ResolvePermissionFunctor& functor)
        :realm(realm), permissionResolver(functor){}

    virtual ~Authorizer() {}

    void isPermitted(const std::string& principal,
                     const std::string& permission,
                     const AuthorizeCallback& callback) const;

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
    void isPermitted(const PrincipalCollection& principals,
                     const std::string& permission,
                     const AuthorizeCallback& callback) const;

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
    void isPermitted(const PrincipalCollection& principal,
                     const PermissionPtr& permission,
                     const AuthorizeCallback& callback) const;

    /**
     * Returns the realms wrapped by this <code>Authorizer</code> which are consulted during an authorization check.
     *
     * @return the realms wrapped by this <code>Authorizer</code> which are consulted during an authorization check.
     */
    const AuthorizingRealmPtr& getRealm() const {
        return realm;
    }

    /**
     * Sets the realms wrapped by this <code>Authorizer</code> which are consulted during an authorization check.
     *
     * @param realms the realms wrapped by this <code>Authorizer</code> which are consulted during an authorization check.
     */
    void setRealm(const AuthorizingRealmPtr& realm) {this->realm = realm; }

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
        this->permissionResolver = permissionResolver;
    }

private:
    bool realmConfigured() const;

    void doPermite(const AuthorizationInfoPtr& info,
        const PrincipalCollection& principal,
        const PermissionPtr& permission,
        const AuthorizeCallback& callback) const;

protected:
    AuthorizingRealmPtr realm;

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
