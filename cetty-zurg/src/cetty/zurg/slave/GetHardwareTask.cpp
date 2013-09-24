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
        RunCommandRequestPtr lshwRequest(new RunCommandRequest);
        RunCommandResponsePtr lshwResponse(new RunCommandResponse);
        lshwRequest->set_command("lshw");
        slave.runCommand(lshwRequest,
                         lshwResponse,
                         boost::bind(&GetHardwareTask::lshwDONE,
                                     self,
                                     _1,
                                     lshwRequest));
    }

    RunCommandRequestPtr lspciRequest(new RunCommandRequest);
    RunCommandResponsePtr lspciResponse(new RunCommandResponse);
    lspciRequest->set_command("lspci");
    slave.runCommand(lspciRequest,
                     lspciResponse,
                     boost::bind(&GetHardwareTask::lspciDONE,
                                 self,
                                 _1,
                                 lspciRequest));

    RunCommandRequestPtr lscpuRequest(new RunCommandRequest);
    RunCommandResponsePtr lscpuResponse(new RunCommandResponse);
    lscpuRequest->set_command("lscpu");
    slave.runCommand(lscpuRequest,
                     lscpuResponse,
                     boost::bind(&GetHardwareTask::lscpuDONE,
                                 self,
                                 _1,
                                 lscpuRequest));

    RunCommandRequestPtr ifconfigRequest(new RunCommandRequest);
    RunCommandResponsePtr ifconfigResponse(new RunCommandResponse);
    ifconfigRequest->set_command("/sbin/ifconfig");
    slave.runCommand(ifconfigRequest,
                     ifconfigResponse,
                     boost::bind(&GetHardwareTask::ifconfigDONE,
                                 self,
                                 _1,
                                 ifconfigRequest));
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


