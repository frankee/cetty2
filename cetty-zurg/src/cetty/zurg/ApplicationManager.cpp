#include <cetty/zurg/ApplicationManager.h>
#include <cetty/zurg/ProcessManager.h>

#include <muduo/base/Logging.h>

#include <stdio.h>
#include <sys/resource.h>
#include <sys/wait.h>

namespace cetty {
namespace zurg
{

struct Application
{
  AddApplicationRequest request;
  ApplicationStatus status;
};

ApplicationManager::ApplicationManager(muduo::net::EventLoop* loop, ProcessManager* children)
  : loop_(loop),
    processManager(children)
{
}

ApplicationManager::~ApplicationManager()
{
}

void ApplicationManager::add(const ConstAddApplicationRequestPtr& request,
    const AddApplicationResponsePtr& response,
    const DoneCallback& done) {
    assert(request->name().find('/') == std::string::npos); // FIXME
    AddApplicationRequestPtr& requestRef = apps_[request->name()].request;
    AddApplicationRequestPtr prev_request(requestRef);
    requestRef = request;
    ApplicationStatus& status = apps_[request->name()].status;

    status.set_name(request->name());
    if (!status.has_state())
    {
        LOG_INFO << "new app";
        status.set_state(kNewApp);
    }

    AddApplicationResponse response;
    response.mutable_status()->CopyFrom(status);
    if (prev_request)
    {
        response.mutable_prev_request()->CopyFrom(*prev_request);
    }
    done(&response);
}

void ApplicationManager::start(const ConstStartApplicationsRequestPtr& request,
    const StartApplicationsResponsePtr& response,
    const DoneCallback& done) {
    for (int i = 0; i < request->names_size(); ++i)
    {
        const std::string& appName = request->names(i);
        AppMap::iterator it = apps_.find(appName);
        if (it != apps_.end())
        {
            startApp(&it->second, response.add_status());
        }
        else
        {
            // application not found
            ApplicationStatus* status = response.add_status();
            status->set_state(kUnknown);
            status->set_name(appName);
            status->set_message("Application is unknown.");
        }
    }
    done(&response);
}

void ApplicationManager::startApp(Application* app, ApplicationStatus* out)
{
  const AddApplicationRequestPtr& appRequest(app->request);
  ApplicationStatus* status = &app->status;

  if (status->state() != kRunning)
  {
    ProcessPtr process(new Process(appRequest));
    int err = 12; // ENOMEM;
    try
    {
      err = process->start();
    }
    catch (...)
    {
    }

    if (err)
    {
      status->set_state(kError);
      // FIXME
    }
    else
    {
      status->set_state(kRunning);
      status->set_pid(process->pid());
      // FIXME
      processManager->runAtExit(process->pid(),  // bind strong ptr
          boost::bind(&ApplicationManager::onProcessExit, this, process, _1, _2));
    }
    out->CopyFrom(*status);
  }
  else
  {
    out->CopyFrom(*status);
    out->set_message("Already running.");
  }
}

void ApplicationManager::stop(const ConstStopApplicationRequestPtr& request,
    const StopApplicationResponsePtr& response,
    const DoneCallback& done) {

}

#pragma GCC diagnostic ignored "-Wold-style-cast"
// FIXME: dup Process::onCommandExit
void ApplicationManager::onProcessExit(const ProcessPtr& process, int status, const struct rusage&)
{
  const std::string& appName = process->name();

  char buf[256];
  if (WIFEXITED(status))
  {
    snprintf(buf, sizeof buf, "exit status %d", WEXITSTATUS(status));
    // response.set_exit_status(WEXITSTATUS(status));
  }
  else if (WIFSIGNALED(status))
  {
    snprintf(buf, sizeof buf, "signaled %d%s",
             WTERMSIG(status), WCOREDUMP(status) ? " (core dump)" : "");
    // response.set_signaled(WTERMSIG(status));
    // response.set_coredump(WCOREDUMP(status));
  }

  LOG_WARN << "AppManager[" << appName << "] onProcessExit - " << buf;

  AppMap::iterator it = apps_.find(appName);
  if (it != apps_.end())
  {
    Application& app = it->second;
    app.status.set_state(kExited);


    // FIXME: notify master
  }
  else
  {
    LOG_ERROR << "AppManager[" << appName << "] - Unknown app ";
  }
}

void ApplicationManager::get(const ConstGetApplicationsRequestPtr& request,
    const GetApplicationsResponsePtr& response,
    const DoneCallback& done) {

}

void ApplicationManager::list(const ConstListApplicationsRequestPtr& request,
    const ListApplicationsResponsePtr& response,
    const DoneCallback& done) {

}

void ApplicationManager::remove(const ConstRemoveApplicationsRequestPtr& request,
    const RemoveApplicationsResponsePtr& response,
    const DoneCallback& done) {

}

}}

