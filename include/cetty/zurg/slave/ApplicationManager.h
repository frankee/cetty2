/*
 * ApplicationManager.h
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#ifndef APPLICATIONMANAGER_H_
#define APPLICATIONMANAGER_H_

#include <cetty/zurg/slave/slave.pb.h>
#include <cetty/zurg/slave/slave_service.pb.h>
#include <cetty/channel/EventLoopPtr.h>
#include <cetty/channel/EventLoop.h>
#include <cetty/zurg/slave/Process.h>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <string>

struct rusage;

namespace cetty {
namespace zurg {
namespace slave {

class Process;
class ProcessManager;

using namespace cetty::channel;

struct Application {
    AddApplicationRequest request;
    ApplicationStatus status;
};

typedef boost::shared_ptr<Application> ApplicationPtr;

class ApplicationManager : boost::noncopyable {
public:
    ApplicationManager(ProcessManager*);
    ~ApplicationManager();

    void add(const ConstAddApplicationRequestPtr& request,
        const AddApplicationResponsePtr& response,
        const DoneCallback& done);

    void start(const ConstStartApplicationsRequestPtr& request,
        const StartApplicationsResponsePtr& response,
        const DoneCallback& done);

    void stop(const ConstStopApplicationRequestPtr& request,
        const StopApplicationResponsePtr& response,
        const DoneCallback& done);

    void list(const ConstListApplicationsRequestPtr& request,
        const ListApplicationsResponsePtr& response,
        const DoneCallback& done);

    void remove(const ConstRemoveApplicationsRequestPtr& request,
        const RemoveApplicationsResponsePtr& response,
        const DoneCallback& done);

    // kill all applications started by #startApp() and self
    void stopAll();

private:
    void startApp(const Application&, ApplicationStatus* out);
    void onProcessExit(const ProcessPtr&, int status, const struct rusage&);

private:
    typedef std::map<std::string, Application> ApplicationMap;

private:
    ProcessManager* processManager;
    ApplicationMap applications;
};

}
}
}

#endif /* APPLICATIONMANAGER_H_ */
