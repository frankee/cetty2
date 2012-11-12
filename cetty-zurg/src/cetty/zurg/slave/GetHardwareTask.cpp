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

GetHardwareTask::GetHardwareTask(const ConstGetHardwareRequestPtr& request,
                                 const GetHardwareResponsePtr& response,
                                 const DoneCallback& done,
                                 SlaveServiceImpl& slaveService)
    : lshw(request->lshw()),
      lshwDone(false),
      lspciDone(false),
      lscpuDone(false),
      ifconfigDone(false),
      response(response),
      done(done) {

    LOG_DEBUG << this;

    if (!response) {
        this->response = new GetHardwareResponse;
    }

    start(slaveService);
}

void GetHardwareTask::start(SlaveServiceImpl& slave) {
    GetHardwareTaskPtr self = shared_from_this();

    if (lshw) {
        RunCommandRequestPtr lshw(new RunCommandRequest);
        lshw->set_command("lshw");
        slave.runCommand(lshw,
                         new RunCommandResponse,
                         boost::bind(&GetHardwareTask::lshwDONE,
                                     self,
                                     _1,
                                     lshw));
    }

    RunCommandRequestPtr lspci(new RunCommandRequest);
    lspci->set_command("lspci");
    slave.runCommand(lspci,
                     new RunCommandResponse,
                     boost::bind(&GetHardwareTask::lspciDONE, self, _1, lspci));

    RunCommandRequestPtr lscpu(new RunCommandRequest);
    lscpu->set_command("lscpu");
    slave.runCommand(lscpu,
                     new RunCommandResponse,
                     boost::bind(&GetHardwareTask::lscpuDONE, self, _1, lscpu));

    RunCommandRequestPtr ifconfig(new RunCommandRequest);
    ifconfig->set_command("/sbin/ifconfig");
    slave.runCommand(ifconfig,
                     new RunCommandResponse,
                     boost::bind(&GetHardwareTask::ifconfigDONE, self, _1, ifconfig));
}

void GetHardwareTask::checkAllDone() {
    LOG_DEBUG << this;

    if (lspciDone
            && lscpuDone
            && ifconfigDone
            && (lshw == false || lshwDone)) {
        LOG_INFO << "GetHardwareTask all done";
        done(response);
    }
}

}
}
}


