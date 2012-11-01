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
    GetHardwareTask(const GetHardwareRequestPtr& request, const DoneCallback& done)
        : lshw_(request->lshw()),
          lspciDone_(false),
          lscpuDone_(false),
          ifconfigDone_(false),
          lshwDone_(false),
          done_(done) {
        LOG_DEBUG << this;
    }

    ~GetHardwareTask() {
        LOG_DEBUG << this;
    }

    void start(SlaveServiceImpl* slave) {
        GetHardwareTaskPtr thisPtr(shared_from_this());

        if (lshw_) {
            RunCommandRequestPtr lshw(new RunCommandRequest);
            lshw->set_command("lshw");
            slave->runCommand(lshw, NULL,  boost::bind(&GetHardwareTask::lshwDone, thisPtr, _1));
        }

        RunCommandRequestPtr lspci(new RunCommandRequest);
        lspci->set_command("lspci");
        slave->runCommand(lspci, NULL,  boost::bind(&GetHardwareTask::lspciDone, thisPtr, _1));

        RunCommandRequestPtr lscpu(new RunCommandRequest);
        lscpu->set_command("lscpu");
        slave->runCommand(lscpu, NULL,  boost::bind(&GetHardwareTask::lscpuDone, thisPtr, _1));

        RunCommandRequestPtr ifconfig(new RunCommandRequest);
        ifconfig->set_command("/sbin/ifconfig");
        slave->runCommand(ifconfig, NULL,  boost::bind(&GetHardwareTask::ifconfigDone, thisPtr, _1));
    }

private:

#define DEFINE_DONE(KEY)                                        \
    void KEY##Done(const google::protobuf::Message* message)      \
    {                                                             \
        assert(KEY##Done_ == false);                                \
        KEY##Done_ = true;                                          \
        const zurg::RunCommandResponse* out =                       \
                down_cast<const zurg::slave::RunCommandResponse*>(message);    \
        if (out->error_code() == 0)                                 \
        {                                                           \
            resp_.set_##KEY(out->std_output());                       \
        }                                                           \
        checkAllDone();                                             \
    }

    DEFINE_DONE(lspci)
    DEFINE_DONE(lscpu)
    DEFINE_DONE(lshw)
    DEFINE_DONE(ifconfig)

#undef DEFINE_DONE

    void checkAllDone() {
        LOG_DEBUG << this;
        bool allDone = lspciDone_ && lscpuDone_ && ifconfigDone_ && (lshw_ == false || lshwDone_);

        if (allDone) {
            LOG_INFO << "GetHardwareTask all done";
            done_(&resp_);
        }
    }

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
