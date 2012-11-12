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
    std::string name_;
    std::string prefix_;
    std::string masterAddress_;

    int listenPort_;
    int masterPort_;

    int zombieInterval_;
    int heartbeatInterval_;

    // redirect stream used by process
    bool isRdtCmdStdout_;
    bool isRdtCmdStderr_;

    ZurgSlaveConfig();
    virtual ConfigObject* create() const { return new ZurgSlaveConfig; }
};

}
}
}

#endif /* ZURGSLAVECONFIG_H_ */
