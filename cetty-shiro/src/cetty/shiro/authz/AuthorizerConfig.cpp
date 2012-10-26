/*
 * AuthorizerConfig.cpp
 *
 *  Created on: 2012-10-26
 *      Author: chenhl
 */

#include <cetty/shiro/authz/AuthorizerConfig.h>
#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace shiro {
namespace authz {

CETTY_CONFIG_ADD_DESCRIPTOR(AuthorizerConfig,
    (new ConfigDescriptor( 1, // param count
          CETTY_CONFIG_FIELD(AuthorizerConfig, permissionType, STRING))
    ), // new ConfigDescriptor
    new AuthorizerConfig);

AuthorizerConfig::AuthorizerConfig()
   : ConfigObject("AuthorizerConfig"){}

}
}
}


