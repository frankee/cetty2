/*
 * AuthenticatorConfig.cpp
 *
 *  Created on: 2012-10-9
 *      Author: chenhl
 */

#include <cetty/shiro/authc/AuthenticatorConfig.h>
#include <cetty/shiro/authc/Authenticator.h>

#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace shiro {
namespace authc {

CETTY_CONFIG_ADD_DESCRIPTOR(AuthenticatorConfig,
                            new ConfigObjectDescriptor(
                                1,
                                CETTY_CONFIG_FIELD(AuthenticatorConfig, credentialsMatcher, STRING)
                                ),
                            new AuthenticatorConfig);

AuthenticatorConfig::AuthenticatorConfig()
    : ConfigObject("AuthenticatorConfig"){

}

}
}
}
