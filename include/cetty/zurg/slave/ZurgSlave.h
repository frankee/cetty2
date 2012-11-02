/*
 * ZurgSlave.h
 *
 *  Created on: 2012-10-31
 *      Author: chenhl
 */

#ifndef ZURGSLAVE_H_
#define ZURGSLAVE_H_

#include <cetty/zurg/slave/ZurgSlaveConfig.h>

namespace cetty {
namespace zurg {
namespace slave {

class ZurgSlave{
public:
    ZurgSlave();
    void start();

    const std::string &getName() const {
        return config_.name_;
    }

    const std::string &getPrefix() const {
        return config_.prefix_;
    }

   static const ZurgSlave &instance(){
        static ZurgSlave zs;
        return zs;
    }

private:
    void init();

    ZurgSlaveConfig config_;
};

}
}
}


#endif /* ZURGSLAVE_H_ */
