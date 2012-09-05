#if !defined(CETTY_SHIRO_AUTHZ_AUTHORIZER_H)
#define CETTY_SHIRO_AUTHZ_AUTHORIZER_H

/*
 * Authorizer.h
 *
 *  Created on: 2012-9-4
 *      Author: chenhl
 */

#include <vector>

#include <cetty/shiro/session/SessionPtr.h>
#include <cetty/shiro/realm/AuthorisingRealm.h>

namespace cetty {
namespace shiro {
namespace authz {

using namespace cetty::shiro::realm;
using namespace cetty::shiro::session;

class Authorizer {
public:
    Authorizer(){}
    Authorizer(const std::vector<AuthorisingRealm> &realms): realms(realms){}

    bool authoriseUser(const SessionPtr &sission, const std::string &operation);
    bool authoriseApp(const std::string &appKey, const std::string &operation);

    void setRealms(const std::vector<AuthorisingRealm> &realms){
        this->realms = realms;
    }

    void addRealm(const AuthorisingRealm &realm){
        this->realms.push_back(realm);
    }

private:
    std::vector<AuthorisingRealm> realms;
};

}
}
}




#endif // !defined(CETTY_SHIRO_AUTHZ_AUTHORIZER_H)
