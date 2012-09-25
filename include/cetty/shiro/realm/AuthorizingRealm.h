#if !defined(CETTY_SHIRO_REALM_AUTHORISINGREALM_H)
#define CETTY_SHIRO_REALM_AUTHORISINGREALM_H

#include <map>
#include <boost/function.hpp>

#include <cetty/shiro/realm/AuthenticatingRealm.h>
#include <cetty/shiro/authz/AuthorizationInfo.h>
#include <cetty/shiro/authz/AuthorizationInfoPtr.h>
#include <cetty/shiro/PrincipalCollection.h>

namespace cetty {
namespace shiro {
namespace realm {

using namespace cetty::shiro::authz;

/**
 * An {@code AuthorizingRealm} extends the {@code AuthenticatingRealm}'s capabilities by adding Authorization
 * (access control) support.
 * <p/>
 * This implementation will perform all role and permission checks automatically (and subclasses do not have to
 * write this logic) as long as the
 * {@link #getAuthorizationInfo(org.apache.shiro.subject.PrincipalCollection)} method returns an
 * {@link AuthorizationInfo}.  Please see that method's JavaDoc for an in-depth explanation.
 * <p/>
 * If you find that you do not want to utilize the {@link AuthorizationInfo AuthorizationInfo} construct,
 * you are of course free to subclass the {@link AuthenticatingRealm AuthenticatingRealm} directly instead and
 * implement the remaining Realm interface methods directly.  You might do this if you want have better control
 * over how the Role and Permission checks occur for your specific data source.  However, using AuthorizationInfo
 * (and its default implementation {@link org.apache.shiro.authz.SimpleAuthorizationInfo SimpleAuthorizationInfo}) is sufficient in the large
 * majority of Realm cases.
 *
 * @see org.apache.shiro.authz.SimpleAuthorizationInfo
 * @since 0.2
 */
class AuthorizingRealm : public AuthenticatingRealm {
public:
    typedef boost::function1<void, const AuthorizationInfoPtr&> GetAuthorizationInfoCallback;

public:
    AuthorizingRealm() {}
    AuthorizingRealm(std::string name): AuthenticatingRealm(name){}

    /**
     * Returns an account's authorization-specific information for the specified {@code principals},
     * or {@code null} if no account could be found.  The resulting {@code AuthorizationInfo} object is used
     * by the other method implementations in this class to automatically perform access control checks for the
     * corresponding {@code Subject}.
     * <p/>
     * This implementation obtains the actual {@code AuthorizationInfo} object from the subclass's
     * implementation of
     * {@link #doGetAuthorizationInfo(org.apache.shiro.subject.PrincipalCollection) doGetAuthorizationInfo}, and then
     * caches it for efficient reuse if caching is enabled (see below).
     * <p/>
     * Invocations of this method should be thought of as completely orthogonal to acquiring
     * {@link #getAuthenticationInfo(org.apache.shiro.authc.AuthenticationToken) authenticationInfo}, since either could
     * occur in any order.
     * <p/>
     * For example, in &quot;Remember Me&quot; scenarios, the user identity is remembered (and
     * assumed) for their current session and an authentication attempt during that session might never occur.
     * But because their identity would be remembered, that is sufficient enough information to call this method to
     * execute any necessary authorization checks.  For this reason, authentication and authorization should be
     * loosely coupled and not depend on each other.
     * <h3>Caching</h3>
     * The {@code AuthorizationInfo} values returned from this method are cached for efficient reuse
     * if caching is enabled.  Caching is enabled automatically when an {@link #setAuthorizationCache authorizationCache}
     * instance has been explicitly configured, or if a {@link #setCacheManager cacheManager} has been configured, which
     * will be used to lazily create the {@code authorizationCache} as needed.
     * <p/>
     * If caching is enabled, the authorization cache will be checked first and if found, will return the cached
     * {@code AuthorizationInfo} immediately.  If caching is disabled, or there is a cache miss, the authorization
     * info will be looked up from the underlying data store via the
     * {@link #doGetAuthorizationInfo(org.apache.shiro.subject.PrincipalCollection)} method, which must be implemented
     * by subclasses.
     * <h4>Changed Data</h4>
     * If caching is enabled and if any authorization data for an account is changed at
     * runtime, such as adding or removing roles and/or permissions, the subclass implementation should clear the
     * cached AuthorizationInfo for that account via the
     * {@link #clearCachedAuthorizationInfo(org.apache.shiro.subject.PrincipalCollection) clearCachedAuthorizationInfo}
     * method.  This ensures that the next call to {@code getAuthorizationInfo(PrincipalCollection)} will
     * acquire the account's fresh authorization data, where it will then be cached for efficient reuse.  This
     * ensures that stale authorization data will not be reused.
     *
     * @param principals the corresponding Subject's identifying principals with which to look up the Subject's
     *                   {@code AuthorizationInfo}.
     * @return the authorization information for the account associated with the specified {@code principals},
     *         or {@code null} if no account could be found.
     */
    void getAuthorizationInfo(const PrincipalCollection& principals, const GetAuthorizationInfoCallback& callback);

protected:
    virtual void doGetAuthorizationInfo(const PrincipalCollection& principals,
                                        const GetAuthorizationInfoCallback& callback) = 0;

private:
    void onGetAuthorizationInfo(
        const AuthorizationInfoPtr& info,
        const PrincipalCollection& principals,
        const GetAuthorizationInfoCallback& callback);

protected:
    std::map<std::string, AuthorizationInfoPtr> authorizations;
};

}
}
}

#endif // #if !defined(CETTY_SHIRO_REALM_AUTHORISINGREALM_H)
