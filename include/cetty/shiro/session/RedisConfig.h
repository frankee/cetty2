/*
 * RedisConfig.h
 *
 *  Created on: 2012-10-16
 *      Author: chenhl
 */

#if !defined(CETTY_SHIRO_SESSION_REDISCONFIG_H)
#define CETTY_SHIRO_SESSION_REDISCONFIG_H

#include <cetty/config/ConfigObject.h>

namespace cetty {
namespace shiro {
namespace session {

using namespace cetty::config;

class RedisConfig : public ConfigObject {
public:
    std::string ip;
    int port;

    RedisConfig();
    virtual ConfigObject* create() const { return new RedisConfig; }
};

}
}
}


#endif // #if !defined(CETTY_SHIRO_SESSION_REDISCONFIG_H)
