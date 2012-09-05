#if !defined(CETTY_SHIRO_REALM_AUTHORISINGREALM_H)
#define CETTY_SHIRO_REALM_AUTHORISINGREALM_H

#include <cetty/shiro/realm/Realm.h>

namespace cetty {
namespace shiro {
namespace realm {

class AuthorisingRealm : public Realm{
public:
    AuthorisingRealm(){}
    AuthorisingRealm(bool isCached, std::string name);


    bool authoriseApp(const std::string &appKey, const std::string &operation);

};

}
}
}

#endif // #if !defined(CETTY_SHIRO_REALM_AUTHORISINGREALM_H)
