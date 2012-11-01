/*
 * Pipe.cpp
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#include <cetty/zurg/slave/Pipe.h>

namespace cetty {
namespace zurg {
namespace slave {

Pipe::Pipe(){
    if (::pipe2(pipefd_, O_CLOEXEC)) {
        char buf[512];
        throw std::runtime_error(strerror_r(errno, buf, sizeof buf));
    }
}

Pipe::~Pipe() {
    if (readFd() >= 0) closeRead();
    if (writeFd() >= 0) closeWrite();
}

}
}
}


