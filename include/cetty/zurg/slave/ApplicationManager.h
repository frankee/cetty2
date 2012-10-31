#if !defined(CETTY_ZURG_SLAVE_APPLICATIONMANAGER_H)
#define CETTY_ZURG_SLAVE_APPLICATIONMANAGER_H

#include <cetty/zurg/proto/slave.pb.h>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <string>

struct rusage;


namespace cetty { namespace zurg { namespace slave { 

using namespace cetty::zurg::proto;

struct Application;
class ProcessManager;

//typedef boost::shared_ptr<Application> ApplicationPtr;
class Process;
typedef boost::shared_ptr<Process> ProcessPtr;

class ApplicationManager : boost::noncopyable {
public:
    ApplicationManager(const EventLoopPtr&, ProcessManager*);
    ~ApplicationManager();

    virtual void add(const ConstAddApplicationRequestPtr& request,
                     const AddApplicationResponsePtr& response,
                     const SlaveService::DoneCallback& done);

    virtual void start(const ConstStartApplicationsRequestPtr& request,
                       const StartApplicationsResponsePtr& response,
                       const SlaveService::DoneCallback& done);

    virtual void stop(const ConstStopApplicationRequestPtr& request,
                      const StopApplicationResponsePtr& response,
                      const SlaveService::DoneCallback& done);

    virtual void get(const ConstGetApplicationsRequestPtr& request,
                     const GetApplicationsResponsePtr& response,
                     const SlaveService::DoneCallback& done);

    virtual void list(const ConstListApplicationsRequestPtr& request,
                      const ListApplicationsResponsePtr& response,
                      const SlaveService::DoneCallback& done);

    virtual void remove(const ConstRemoveApplicationsRequestPtr& request,
                        const RemoveApplicationsResponsePtr& response,
                        const SlaveService::DoneCallback& done);

private:
    void startApp(Application* app, ApplicationStatus* out);
    void onProcessExit(const ProcessPtr&, int status, const struct rusage&);

private:
    typedef std::map<std::string, Application> ApplicationMap;

private:
    EventLoopPtr loop_;
    ProcessManager* processManager;

    ApplicationMap apps_;
};


}}}

#endif //#if !defined(CETTY_ZURG_SLAVE_APPLICATIONMANAGER_H)

// Local Variables:
// mode: c++
// End:
