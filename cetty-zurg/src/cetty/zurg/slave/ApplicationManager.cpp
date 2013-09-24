#include <cetty/zurg/slave/ApplicationManager.h>
#include <cetty/zurg/slave/ProcessManager.h>
#include <cetty/zurg/slave/Process.h>
#include <cetty/logging/LoggerHelper.h>

#include <stdio.h>
#include <sys/resource.h>
#include <sys/wait.h>

namespace cetty {
namespace zurg {
namespace slave {

ApplicationManager::ApplicationManager(ProcessManager& psManager)
    :   processManager_(psManager) {
    psManager.setStopAll(boost::bind(&ApplicationManager::stopAll, this));
}

ApplicationManager::~ApplicationManager() {
    stopAll();
}

void ApplicationManager::add(const ConstAddApplicationRequestPtr& request,
                             const AddApplicationResponsePtr& response,
                             const DoneCallback& done) {

    assert(request->name().find('/') == std::string::npos); // FIXME

    std::pair<Applications::iterator, bool> insertRet =
        applications_.insert(
            std::pair<std::string, Application>(request->name(), Application()));

    AddApplicationRequest& requestRef = (*(insertRet.first)).second.request;
    requestRef.CopyFrom(*request);
    AddApplicationRequestPtr prev_request(insertRet.second ? NULL : &requestRef);

    ApplicationStatus& status = applications_[request->name()].status;
    status.set_name(request->name());

    if (!status.has_state()) {
        LOG_INFO << "new app";
        status.set_state(kNewApp);
    }

    response->mutable_status()->CopyFrom(status);

    if (prev_request) {
        response->mutable_prev_request()->CopyFrom(*prev_request);
    }

    if (done) { done(response); }
}

void ApplicationManager::start(const ConstStartApplicationsRequestPtr& request,
                               const StartApplicationsResponsePtr& response,
                               const DoneCallback& done) {

    for (int i = 0; i < request->names_size(); ++i) {
        const std::string& appName = request->names(i);
        Applications::iterator it = applications_.find(appName);

        if (it != applications_.end()) {
            startApp((*it).second, response->add_status());
        }
        else {
            ApplicationStatus* status = response->add_status();
            status->set_state(kUnknown);
            status->set_name(appName);
            status->set_message("Application need add first before start.");
        }
    }

    if (done) { done(response); }
}

void ApplicationManager::startApp(const Application& app,
                                  ApplicationStatus* out) {
    const AddApplicationRequestPtr appRequest =
        const_cast<AddApplicationRequest*>(&(app.request));
    ApplicationStatus* status = const_cast<ApplicationStatus*>(&app.status);

    if (status->state() != kRunning) {
        ProcessPtr process(new Process(appRequest,
                                       processManager_.config().slaveName,
                                       processManager_.config().prefix));

        int err = 12; // ENOMEM;

        try {
            err = process->start();
        }
        catch (...) {}

        if (err) {
            status->set_state(kError);
            // FIXME
        }
        else {
            status->set_state(kRunning);
            status->set_pid(process->pid());

            // without guarantee of child process is running when
            // execute flow come here.
            processManager_.runAtExit(
                process->pid(),
                boost::bind(
                    &ApplicationManager::onProcessExit,
                    this, process, _1, _2
                )
            );
        }

        if (out != NULL) {
            out->CopyFrom(*status);
        }
    }
    else {
        if (out != NULL) {
            out->CopyFrom(*status);
            out->set_message("Already running.");
        }
    }
}

void ApplicationManager::stop(const ConstStopApplicationRequestPtr& request,
                              const StopApplicationResponsePtr& response,
                              const DoneCallback& done) {

    ApplicationStatus responseStatus = response->status();
    const std::string& appName = request->names();

    Applications::iterator it = applications_.find(appName);

    if (it != applications_.end()) {
        pid_t pid = (*it).second.status.pid();

        if (pid > 0 && !kill(pid, SIGSTOP)) {
            // just send signal to process
            responseStatus.set_state(kExited);
            responseStatus.set_name(appName);
            responseStatus.set_message("Send SIGSTOP to stop application successfully.");
        }
        else {
            responseStatus.set_state(kError);
            responseStatus.set_name(appName);
            responseStatus.set_message("Send SIGSTOP to stop application failed.");
        }
    }
    else {
        responseStatus.set_state(kUnknown);
        responseStatus.set_name(appName);
        responseStatus.set_message("Application is unknown.");
    }

    if (done) { done(response); }
}

void ApplicationManager::onProcessExit(const ProcessPtr& process,
                                       int status,
                                       const struct rusage&) {

    const std::string& appName = process->name();
    LOG_WARN << "AppManager[" << appName << "] onProcessExit";

    bool isStart = false;

    if (WIFSIGNALED(status)) {
        if (WTERMSIG(status) != SIGINT &&
                WTERMSIG(status) != SIGQUIT &&
                WTERMSIG(status) != SIGTERM &&
                WTERMSIG(status) != SIGSTOP) {
            isStart = true;
        }
    }

    Applications::iterator it = applications_.find(appName);

    if (it != applications_.end()) {
        Application& app = (*it).second;
        app.status.set_state(kExited);

        if (isStart) {
            startApp((*it).second, NULL);
        }
    }
    else {
        LOG_ERROR << "AppManager[" << appName << "] - Unknown app ";
    }
}

void ApplicationManager::list(const ConstListApplicationsRequestPtr& request,
                              const ListApplicationsResponsePtr& response,
                              const DoneCallback& done) {

    Applications::iterator it = applications_.begin();

    for (; it != applications_.end(); ++ it) {
        ApplicationStatus* as = response->add_status();
        as->set_name((*it).second.status.name());
        as->set_pid((*it).second.status.pid());

        if ((*it).second.status.state() == kNewApp) {
            as->set_message("NEW");
        }
        else if ((*it).second.status.state() == kRunning) {
            as->set_message("RUNNING");
        }
        else if ((*it).second.status.state() == kExited) {
            as->set_message("EXITED");
        }
        else if ((*it).second.status.state() == kError) {
            as->set_message("ERROR");
        }

        if ((*it).second.status.state() == kRunning) {
            pid_t pid = (*it).second.status.pid();

            if (pid > 0) {
                kill(pid, SIGSTOP);
            }
        }
    }

    if (done) {
        done(response);
    }
}

void ApplicationManager::remove(const ConstRemoveApplicationsRequestPtr& request,
                                const RemoveApplicationsResponsePtr& response,
                                const DoneCallback& done) {

    int i = 0;
    std::string name;

    for (; i < request->name_size(); ++i) {
        name = request->name(i);
        applications_.erase(name);
    }

    if (done) { done(response); }
}

void ApplicationManager::stopAll() {
    Applications::iterator it = applications_.begin();

    for (; it != applications_.end(); ++ it) {
        if ((*it).second.status.state() == kRunning) {
            pid_t pid = (*it).second.status.pid();
            LOG_INFO << "Kill process [" << pid << "]";

            if (pid > 0) {
                kill(pid, SIGSTOP);
            }
        }
    }

    LOG_INFO << "Main process [" << getpid() << "]exit.";
    //exit(0);
}

}
}
}
