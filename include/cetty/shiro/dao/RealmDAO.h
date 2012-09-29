/*
 * RealmDao.h
 *
 *  Created on: 2012-9-27
 *      Author: chenhl
 */

#ifndef REALMDAO_H_
#define REALMDAO_H_

#include <cetty/shiro/authc/AuthenticationInfo.h>
#include <cetty/shiro/authc/AuthenticationInfoPtr.h>
#include <cetty/shiro/authz/AuthorizationInfo.h>
#include <cetty/shiro/authz/AuthorizationInfoPtr.h>

namespace cetty {
namespace shiro {
namespace dao {

using namespace cetty::shiro::authc;
using namespace cetty::shiro::authz;

class RealmDAO : public BaseDAO{
public:
    RealmDAO(){}
    virtual ~RealmDAO(){}

    virtual AuthenticationInfoPtr QueryAuthenticationInfo(const std::string &principal,
                                                          const std::string &realmname);
    virtual AuthorizationInfoPtr QueryAuthorizationInfo(const std::string &principal);

};

}
}
}

#endif /* REALMDAO_H_ */
