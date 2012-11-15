/*
 * Pipe.h
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#ifndef PIPE_H_
#define PIPE_H_

#include <boost/noncopyable.hpp>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <assert.h>


namespace cetty {
namespace zurg {
namespace slave {

class Pipe : boost::noncopyable {
public:
    Pipe();
    ~Pipe();

    int readFd() const { return pipefd_[kRead]; }
    int writeFd() const { return pipefd_[kWrite]; }

    ssize_t read(int64_t* x);
    void write(int64_t x);

    void closeRead();
    void closeWrite();

private:
    int pipefd_[2];
    static const int kRead = 0;
    static const int kWrite = 1;
};

inline
void Pipe::write(int64_t x) {
    ssize_t n = ::write(writeFd(), &x, sizeof(x));
    assert(n == sizeof(x));
}

inline
ssize_t Pipe::read(int64_t* x) {
    return ::read(readFd(), x, sizeof(*x));
}

inline
void Pipe::closeRead() {
    ::close(readFd());
    pipefd_[kRead] = -1;
}

inline
void Pipe::closeWrite() {
    ::close(writeFd());
    pipefd_[kWrite] = -1;
}

}
}
}



#endif /* PIPE_H_ */
