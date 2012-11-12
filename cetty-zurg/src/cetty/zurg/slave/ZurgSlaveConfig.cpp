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
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, name_, STRING),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, prefix_, STRING),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, masterAddress_, STRING),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, listenPort_, INT32),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, masterPort_, STRING),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, zombieInterval_, INT32),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, heartbeatInterval_, INT32),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, isRdtCmdStdout_, BOOL),
        CETTY_CONFIG_FIELD(ZurgSlaveConfig, isRdtCmdStderr_, BOOL)
    ),
    new ZurgSlaveConfig
);


ZurgSlaveConfig::ZurgSlaveConfig() :
    name_(std::string()),
    prefix_(std::string()),
    masterAddress_(std::string()),
    listenPort_(0),
    masterPort_(0),
    zombieInterval_(3000),
    heartbeatInterval_(3000),
    isRdtCmdStdout_(true),
    isRdtCmdStderr_(true){

}


}
}
}

