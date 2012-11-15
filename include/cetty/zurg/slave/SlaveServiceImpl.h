/*
 * SlaveServiceImpl.h
 *
 *  Created on: 2012-10-31
 *      Author: chenhl
 */

#ifndef SLAVESERVICEIMPL_H_
#define SLAVESERVICEIMPL_H_

#include <cetty/zurg/slave/slave.pb.h>
#include <cetty/channel/EventLoopPtr.h>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::channel;

class ProcessManager;
class ApplicationManager;

class SlaveServiceImpl : public SlaveService {
public:
    SlaveServiceImpl(const EventLoopPtr& loop);
    virtual ~SlaveServiceImpl();

    virtual void getHardware(
        const ConstGetHardwareRequestPtr& request,
        const GetHardwareResponsePtr& response,
        const DoneCallback& done);

    virtual void getFileContent(
        const ConstGetFileContentRequestPtr& request,
        const GetFileContentResponsePtr& response,
        const DoneCallback& done);

    virtual void getFileChecksum(
        const ConstGetFileChecksumRequestPtr& request,
        const GetFileChecksumResponsePtr& response,
        const DoneCallback& done);

    virtual void listProcesses(
        const ConstListProcessesRequestPtr& request,
        const ListProcessesResponsePtr& response,
        const DoneCallback& done);

    virtual void runCommand(
        const ConstRunCommandRequestPtr& request,
        const RunCommandResponsePtr& response,
        const DoneCallback& done);

    virtual void runScript(
        const ConstRunScriptRequestPtr& request,
        const RunCommandResponsePtr& response,
        const DoneCallback& done);

    virtual void addApplication(
        const ConstAddApplicationRequestPtr& request,
        const AddApplicationResponsePtr& response,
        const DoneCallback& done);

    virtual void startApplications(
        const ConstStartApplicationsRequestPtr& request,
        const StartApplicationsResponsePtr& response,
        const DoneCallback& done);

    virtual void stopApplication(
        const ConstStopApplicationRequestPtr& request,
        const StopApplicationResponsePtr& response,
        const DoneCallback& done);

    virtual void listApplications(
        const ConstListApplicationsRequestPtr& request,
        const ListApplicationsResponsePtr& response,
        const DoneCallback& done);

    virtual void removeApplications(
        const ConstRemoveApplicationsRequestPtr& request,
        const RemoveApplicationsResponsePtr& response,
        const DoneCallback& done);

private:
    void getFileChecksumDone(const ConstGetFileChecksumRequestPtr& request,
                             const google::protobuf::Message* message,
                             const DoneCallback& done);

private:
    boost::scoped_ptr<ProcessManager> processManager;
    boost::scoped_ptr<ApplicationManager> applicationManager;
};

}
}
}

#endif /* SLAVESERVICEIMPL_H_ */
