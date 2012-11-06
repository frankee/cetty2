/*
 * ProcStatFile.h
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#ifndef PROCSTATFILE_H_
#define PROCSTATFILE_H_

#include <stdint.h>

namespace cetty {
namespace zurg {
namespace slave {

struct ProcStatFile {
public:
    explicit ProcStatFile(int pid);

    bool valid_;
    int error_;
    int ppid_;
    int64_t startTime_;

private:
    void parse(const char* buffer);
};

}
}
}

#endif /* PROCSTATFILE_H_ */
