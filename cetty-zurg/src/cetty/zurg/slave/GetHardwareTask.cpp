/*
 * GetHardwareTask.cpp
 *
 *  Created on: 2012-11-2
 *      Author: chenhl
 */

#include <cetty/zurg/slave/GetHardwareTask.h>

namespace cetty {
namespace zurg {
namespace slave {

GetHardwareTask::GetHardwareTask(
    const ConstGetHardwareRequestPtr& request,
    const DoneCallback& done
): lshw_(request->lshw()),
   lspciDone_(false),
   lscpuDone_(false),
   ifconfigDone_(false),
   lshwDone_(false),
   done_(done) {

    LOG_DEBUG << this;
}

void GetHardwareTask::start(SlaveServiceImpl* slave) {
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

void GetHardwareTask::checkAllDone() {
    LOG_DEBUG << this;
    bool allDone = lspciDone_ && lscpuDone_ && ifconfigDone_ && (lshw_ == false || lshwDone_);

    if (allDone) {
        LOG_INFO << "GetHardwareTask all done";
        done_(&resp_);
    }
}


}
}
}


