/*
 * ProcStatFile.h
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#ifndef PROCSTATFILE_H_
#define PROCSTATFILE_H_

namespace cetty {
namespace zurg {
namespace slave {

struct ProcStatFile {
public:
    explicit ProcStatFile(int pid);

    bool valid;
    int error;
    int ppid;
    int64_t startTime;

private:
    void parse(const char* buffer);
};

}
}
}

#endif /* PROCSTATFILE_H_ */
