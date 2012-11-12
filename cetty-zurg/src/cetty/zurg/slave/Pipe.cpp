/*
 * Pipe.cpp
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#include <cetty/zurg/slave/Pipe.h>
#include <cetty/zurg/Util.h>
#include <stdexcept>

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::zurg;

Pipe::Pipe(){
    if (::pipe(pipefd_)) {
        setFd(pipefd_[0], FD_CLOEXEC);
        setFd(pipefd_[1], FD_CLOEXEC);
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


