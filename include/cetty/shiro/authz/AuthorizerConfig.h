/*
 * AuthorizerConfig.h
 *
 *  Created on: 2012-10-26
 *      Author: chenhl
 */

#ifndef AUTHORIZERCONFIG_H_
#define AUTHORIZERCONFIG_H_

#include <cetty/config/ConfigObject.h>

namespace cetty {
namespace shiro {
namespace authz {

using namespace cetty::config;

class AuthorizerConfig : public ConfigObject {
public:
    std::string permissionType;

    AuthorizerConfig();
    virtual ConfigObject* create() const { return new AuthorizerConfig; }
};

}
}
}

#endif /* AUTHORIZERCONFIG_H_ */
