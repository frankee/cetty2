#if !defined(CETTY_ZURG_SLAVE_SLAVESERVICEIMPL_H)
#define CETTY_ZURG_SLAVE_SLAVESERVICEIMPL_H

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include <cetty/zurg/proto/slave.pb.h>
#include <cetty/channel/EventLoopPtr.h>

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::channel;
using namespace cetty::zurg::proto;

class ProcessManager;
class ApplicationManager;

class SlaveServiceImpl : public cetty::zurg::proto::SlaveService {
public:
    SlaveServiceImpl(const EventLoopPtr& loop, int zombieInterval);
    virtual ~SlaveServiceImpl();

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
    EventLoopPtr loop;

    boost::scoped_ptr<ProcessManager> children;
    boost::scoped_ptr<ApplicationManager> apps;
};

}
}
}

#endif //#if !defined(CETTY_ZURG_SLAVE_SLAVESERVICEIMPL_H)

// Local Variables:
// mode: c++
// End:
