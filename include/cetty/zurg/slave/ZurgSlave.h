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

private:
    void init();

    ZurgSlaveConfig config_;
};

}
}
}


#endif /* ZURGSLAVE_H_ */
