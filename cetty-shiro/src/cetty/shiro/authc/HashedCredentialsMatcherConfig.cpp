/*
 * HashedCredentialsMatcherConfig.cpp
 *
 *  Created on: 2012-10-9
 *      Author: chenhl
 */

#include <cetty/shiro/authc/HashedCredentialsMatcherConfig.h>
#include <cetty/shiro/authc/HashedCredentialsMatcher.h>
#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace shiro {
namespace authc {

CETTY_CONFIG_ADD_DESCRIPTOR(HashedCredentialsMatcherConfig,
                            new ConfigDescriptor(
                                4,
                                CETTY_CONFIG_FIELD(HashedCredentialsMatcherConfig, hashSalted, BOOL),
                                CETTY_CONFIG_FIELD(HashedCredentialsMatcherConfig, storedCredentialsHexEncoded, BOOL),
                                CETTY_CONFIG_FIELD(HashedCredentialsMatcherConfig, hashIterations, INT32),
                                CETTY_CONFIG_FIELD(HashedCredentialsMatcherConfig, hashAlgorithm, STRING)
                            ),
                            new HashedCredentialsMatcherConfig);

HashedCredentialsMatcherConfig::HashedCredentialsMatcherConfig()
    : ConfigObject("HashedCredentialsMatcherConfig"),
      hashSalted(true),
      storedCredentialsHexEncoded(true),
      hashIterations(1),
      hashAlgorithm(HashedCredentialsMatcher::SHA1ENGINE){};

}
}
}


