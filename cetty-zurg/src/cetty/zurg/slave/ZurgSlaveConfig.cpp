/*
 * ZurgSlaveConfig.cpp
 *
 *  Created on: 2012-10-31
 *      Author: chenhl
 */

#include <cetty/zurg/slave/ZurgSlaveConfig.h>
#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::config;

CETTY_CONFIG_ADD_DESCRIPTOR(
    ZurgSlaveConfig,
    new ConfigDescriptor(
        9,
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, name, STRING),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, prefix, STRING),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, masterAddress, STRING),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, listenPort, INT32),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, masterPort, STRING),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, zombieInterval, INT32),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, heartbeatInterval, INT32),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, isRdtCmdStdout, BOOL),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, isRdtCmdStderr, BOOL)
    ),
    new ZurgSlaveConfig
);


ZurgSlaveConfig::ZurgSlaveConfig() :
    ConfigObject("ZurgSlaveConfig"),
    name(std::string()),
    prefix(std::string()),
    masterAddress(std::string()),
    listenPort(0),
    masterPort(0),
    zombieInterval(3000),
    heartbeatInterval(3000),
    isRdtCmdStdout(true),
    isRdtCmdStderr(true){

}


}
}
}

