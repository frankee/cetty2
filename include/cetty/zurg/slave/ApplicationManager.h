/*
 * ApplicationManager.h
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#ifndef APPLICATIONMANAGER_H_
#define APPLICATIONMANAGER_H_

#include <cetty/zurg/slave/slave.pb.h>
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

struct Application{
    AddApplicationRequestPtr request;
    ApplicationStatus status;
};
typedef boost::shared_ptr<Application> ApplicationPtr;

class ApplicationManager : boost::noncopyable {
public:
    ApplicationManager(const EventLoopPtr&, ProcessManager*);
    virtual ~ApplicationManager();

    virtual void add(
        const ConstAddApplicationRequestPtr& request,
        const AddApplicationResponsePtr& response,
        const DoneCallback& done
    );

    virtual void start(
        const ConstStartApplicationsRequestPtr& request,
        const StartApplicationsResponsePtr& response,
        const DoneCallback& done
    );

    virtual void stop(
        const ConstStopApplicationRequestPtr& request,
        const StopApplicationResponsePtr& response,
        const DoneCallback& done
    );

    virtual void get(
        const ConstGetApplicationsRequestPtr& request,
        const GetApplicationsResponsePtr& response,
        const DoneCallback& done
    );

    virtual void list(
        const ConstListApplicationsRequestPtr& request,
        const ListApplicationsResponsePtr& response,
        const DoneCallback& done
    );

    virtual void remove(
        const ConstRemoveApplicationsRequestPtr& request,
        const RemoveApplicationsResponsePtr& response,
        const DoneCallback& done
    );

private:
    void startApp(const Application &, ApplicationStatus* out);
    void onProcessExit(const ProcessPtr&, int status, const struct rusage&);

private:
    typedef std::map<std::string, Application> ApplicationMap;

private:
    EventLoopPtr loop_;
    ProcessManager* processManager_;

    ApplicationMap apps_;
};


}}}

#endif /* APPLICATIONMANAGER_H_ */
