/*
 * ZurgSlaveConfig.h
 *
 *  Created on: 2012-10-31
 *      Author: chenhl
 */

#ifndef ZURGSLAVECONFIG_H_
#define ZURGSLAVECONFIG_H_

#include <cetty/config/ConfigObject.h>

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::config;

class ZurgSlaveConfig : public ConfigObject{
public:
    std::string name;
    std::string prefix;
    std::string masterAddress;

    int listenPort;
    int masterPort;

    int zombieInterval;
    int heartbeatInterval;

    // redirect stream used by process
    bool isRdtCmdStdout;
    bool isRdtCmdStderr;

    ZurgSlaveConfig();
    virtual ConfigObject* create() const { return new ZurgSlaveConfig; }
};

}
}
}

#endif /* ZURGSLAVECONFIG_H_ */
