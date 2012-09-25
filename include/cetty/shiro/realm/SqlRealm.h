#if !defined(CETTY_SHIRO_REALM_SQLREALM_H)
#define CETTY_SHIRO_REALM_SQLREALM_H

#include <cetty/shiro/realm/AuthorizingRealm.h>

namespace cetty {
namespace shiro {
namespace realm {

class SqlRealm : public AuthorizingRealm {
public:
    virtual void getAuthenticationInfo(const AuthenticationToken& token,
                                       const GetAuthenticationInfoCallback& callback);

    // register to DAO method
    virtual void onGetAuthenticationInfo(// const AuthenticationToken& token,
                                         const GetAuthenticationInfoCallback& callback);


    virtual void doGetAuthorizationInfo(const PrincipalCollection& principals,
                                        const GetAuthorizationInfoCallback& callback);

    // regist to DAO method
    virtual void onGetAuthorizationInfo(//const PrincipalCollection& principals,
                                         const GetAuthorizationInfoCallback& callback);
};

}
}
}

#endif // #if !defined(CETTY_SHIRO_REALM_SQLREALM_H)
