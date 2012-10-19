/*
 * SecurityManagerConfig.cpp
 *
 *  Created on: 2012-9-29
 *      Author: chenhl
 */

#include <cetty/shiro/SecurityManagerConfig.h>
#include <cetty/shiro/SecurityManager.h>
#include <cetty/shiro/realm/SqlRealm.h>
#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace shiro {

using namespace cetty::config;

CETTY_CONFIG_ADD_DESCRIPTOR(SecurityManagerConfig,
                            (new ConfigDescriptor(
                                1, // param count
                                CETTY_CONFIG_FIELD(SecurityManagerConfig, realm, STRING)
                            )),
                            new SecurityManagerConfig);

SecurityManagerConfig::SecurityManagerConfig()
   : ConfigObject("SecurityManagerConfig"){}

}
}
