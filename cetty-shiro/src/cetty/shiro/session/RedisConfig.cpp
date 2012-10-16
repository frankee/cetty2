/*
 * RedisConfig.cpp
 *
 *  Created on: 2012-10-16
 *      Author: chenhl
 */


#include <cetty/shiro/session/RedisConfig.h>
#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace shiro {
namespace session {

CETTY_CONFIG_ADD_DESCRIPTOR(RedisConfig,
                            new ConfigDescriptor(
                                2,
                                CETTY_CONFIG_FIELD(RedisConfig, ip, STRING),
                                CETTY_CONFIG_FIELD(RedisConfig, port, INT32)
                            ),
                                new RedisConfig);


RedisConfig::RedisConfig()
    :ip("127.0.0.1"),
     port(6379){}

}
}
}

