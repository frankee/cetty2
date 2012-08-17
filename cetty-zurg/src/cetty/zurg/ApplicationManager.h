#ifndef MUDUO_PROTORPC_ZURG_APPMANAGER_H
#define MUDUO_PROTORPC_ZURG_APPMANAGER_H

#include <examples/zurg/proto/slave.pb.h>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <string>

namespace muduo
{
namespace net
{
class EventLoop;
}
}

struct rusage;

namespace cetty {
namespace zurg {

struct Application;
class ProcessManager;

//typedef boost::shared_ptr<Application> ApplicationPtr;
class Process;
typedef boost::shared_ptr<Process> ProcessPtr;

class ApplicationManager : boost::noncopyable
{
 public:
     ApplicationManager(muduo::net::EventLoop*, ProcessManager*);
     ~ApplicationManager();

     virtual void add(const ConstAddApplicationRequestPtr& request,
         const AddApplicationResponsePtr& response,
         const DoneCallback& done);

     virtual void start(const ConstStartApplicationsRequestPtr& request,
         const StartApplicationsResponsePtr& response,
         const DoneCallback& done);

     virtual void stop(const ConstStopApplicationRequestPtr& request,
         const StopApplicationResponsePtr& response,
         const DoneCallback& done);

     virtual void get(const ConstGetApplicationsRequestPtr& request,
         const GetApplicationsResponsePtr& response,
         const DoneCallback& done);

     virtual void list(const ConstListApplicationsRequestPtr& request,
         const ListApplicationsResponsePtr& response,
         const DoneCallback& done);

     virtual void remove(const ConstRemoveApplicationsRequestPtr& request,
         const RemoveApplicationsResponsePtr& response,
         const DoneCallback& done);

 private:
  void startApp(Application* app, ApplicationStatus* out);
  void onProcessExit(const ProcessPtr&, int status, const struct rusage&);

  typedef std::map<std::string, Application> AppMap;
  muduo::net::EventLoop* loop_;
  ProcessManager* processManager;
  AppMap apps_;
};

}
}

#endif  // MUDUO_PROTORPC_ZURG_APPMANAGER_H
