#ifndef MUDUO_PROTORPC_ZURG_SLAVESERVICE_H
#define MUDUO_PROTORPC_ZURG_SLAVESERVICE_H

#include <cetty/zurg/slave.pb.h>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace muduo {
namespace net {
class EventLoop;
}
}

namespace cetty{
namespace zurg {

class ApplicationManager;
class ProcessManager;

class SlaveServiceImpl : public SlaveService {
public:
    SlaveServiceImpl(muduo::net::EventLoop* loop, int zombieInterval);
    ~SlaveServiceImpl();

    void start();

    virtual void getHardware(const ConstGetHardwareRequestPtr& request,
        const GetHardwareResponsePtr& response,
        const DoneCallback& done);

    virtual void getFileContent(const ConstGetFileContentRequestPtr& request,
        const GetFileContentResponsePtr& response,
        const DoneCallback& done);

    virtual void getFileChecksum(const ConstGetFileChecksumRequestPtr& request,
        const GetFileChecksumResponsePtr& response,
        const DoneCallback& done);

    virtual void listProcesses(const ConstListProcessesRequestPtr& request,
        const ListProcessesResponsePtr& response,
        const DoneCallback& done);

    virtual void runCommand(const ConstRunCommandRequestPtr& request,
        const RunCommandResponsePtr& response,
        const DoneCallback& done);

    virtual void runScript(const ConstRunScriptRequestPtr& request,
        const RunCommandResponsePtr& response,
        const DoneCallback& done);

    virtual void addApplication(const ConstAddApplicationRequestPtr& request,
        const AddApplicationResponsePtr& response,
        const DoneCallback& done);

    virtual void startApplications(const ConstStartApplicationsRequestPtr& request,
        const StartApplicationsResponsePtr& response,
        const DoneCallback& done);

    virtual void stopApplication(const ConstStopApplicationRequestPtr& request,
        const StopApplicationResponsePtr& response,
        const DoneCallback& done);

    virtual void getApplications(const ConstGetApplicationsRequestPtr& request,
        const GetApplicationsResponsePtr& response,
        const DoneCallback& done);

    virtual void listApplications(const ConstListApplicationsRequestPtr& request,
        const ListApplicationsResponsePtr& response,
        const DoneCallback& done);

    virtual void removeApplications(const ConstRemoveApplicationsRequestPtr& request,
        const RemoveApplicationsResponsePtr& response,
        const DoneCallback& done);

private:
    void getFileChecksumDone(const ConstGetFileChecksumRequestPtr& request,
                             const GetFileChecksumResponsePtr& response,
                             const DoneCallback& done);

private:
    muduo::net::EventLoop* loop_;

    cetty::util::TimerPtr timer;

    boost::scoped_ptr<ProcessManager> children_;
    boost::scoped_ptr<ApplicationManager> apps_;
};

}
}
#endif  // MUDUO_PROTORPC_ZURG_SLAVESERVICE_H
