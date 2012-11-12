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
    //  wait child process terminate cycle
    int zombieInterval_;
    // deliver heart information to server cycle
    int heartbeatInterval_;
    int parentThreadCnt_;
    int childThreadCnt_;

    bool error_;


    ZurgSlaveConfig();
    virtual ConfigObject* create() const { return new ZurgSlaveConfig; }
};

}
}
}

#endif /* ZURGSLAVECONFIG_H_ */
