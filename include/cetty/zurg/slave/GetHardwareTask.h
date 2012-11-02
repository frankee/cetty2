/*
 * GetHardwareTask.h
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#ifndef GETHARDWARETASK_H_
#define GETHARDWARETASK_H_

#include <cetty/logging/LoggerHelper.h>
#include <boost/enable_shared_from_this.hpp>
#include <google/protobuf/stubs/common.h>

#include <iostream>

namespace cetty {
namespace zurg {
namespace slave {

using namespace google::protobuf;

class GetHardwareTask;
typedef boost::shared_ptr<GetHardwareTask> GetHardwareTaskPtr;

class GetHardwareTask : public boost::enable_shared_from_this<GetHardwareTask>,
                        boost::noncopyable {
public:
    GetHardwareTask(const ConstGetHardwareRequestPtr& request, const DoneCallback& done);

    ~GetHardwareTask() {
        LOG_DEBUG << this;
    }

    void start(SlaveServiceImpl* slave);

private:

#define DEFINE_DONE(KEY)                                         \
    void KEY##Done(const google::protobuf::Message* message) { \
        assert(KEY##Done_ == false);                                \
        KEY##Done_ = true;                                          \
        const RunCommandResponse* out =                       \
              down_cast<const zurg::slave::RunCommandResponse*>(message);    \
        if (out->error_code() == 0) { \
            resp_.set_##KEY(out->std_output()); \
        } \
        checkAllDone();                                             \
    }

    DEFINE_DONE(lspci)
    DEFINE_DONE(lscpu)
    DEFINE_DONE(lshw)
    DEFINE_DONE(ifconfig)

#undef DEFINE_DONE

    void checkAllDone();

    bool lshw_;
    bool lspciDone_;
    bool lscpuDone_;
    bool ifconfigDone_;
    bool lshwDone_;

    GetHardwareResponse resp_;
    DoneCallback done_;
};

}
}
}


#endif /* GETHARDWARETASK_H_ */
