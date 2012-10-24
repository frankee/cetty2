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
class AuthorizingRealm : public AuthenticatingRealm{
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
     */
    void getAuthorizationInfo(const PrincipalCollection& principals, const GetAuthorizationInfoCallback& callback);

    virtual void onLogout(const std::string &principal){
        AuthenticatingRealm::onLogout(principal);
        authorizations.erase(principal);
    }

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
