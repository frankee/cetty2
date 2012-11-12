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

ApplicationManager::ApplicationManager(ProcessManager *psManager)
    :   processManager_(psManager) {
}

ApplicationManager::~ApplicationManager() {

}

void ApplicationManager::add(
    const ConstAddApplicationRequestPtr& request,
    const AddApplicationResponsePtr& response,
    const DoneCallback& done) {

    assert(request->name().find('/') == std::string::npos); // FIXME

    AddApplicationRequest &requestRef = apps_[request->name()].request;
    requestRef.CopyFrom(*request);
    AddApplicationRequestPtr prev_request(&requestRef);

    ApplicationStatus& status = apps_[request->name()].status;
    status.set_name(request->name());
    if (!status.has_state()) {
        LOG_INFO << "new app";
        status.set_state(kNewApp);
    }

    response->mutable_status()->CopyFrom(status);
    if (prev_request) {
        response->mutable_prev_request()->CopyFrom(*prev_request);
    }

    done(response);
}

void ApplicationManager::start(
    const ConstStartApplicationsRequestPtr& request,
    const StartApplicationsResponsePtr& response,
    const DoneCallback& done) {

    for (int i = 0; i < request->names_size(); ++i) {
        const std::string& appName = request->names(i);
        ApplicationMap::iterator it = apps_.find(appName);

        if (it != apps_.end()) {
            startApp(it->second, response->add_status());
        }
        else {
            // application not found
            ApplicationStatus* status = response->add_status();
            status->set_state(kUnknown);
            status->set_name(appName);
            status->set_message("Application is unknown.");
        }
    }

    done(response);
}

void ApplicationManager::startApp(const Application &app, ApplicationStatus* out) {
    const AddApplicationRequestPtr appRequest = const_cast<AddApplicationRequest *>(&(app.request)) ;
    ApplicationStatus* status = const_cast<ApplicationStatus*>(&app.status);

    if (status->state() != kRunning) {
        ProcessPtr process(new Process(appRequest));
        int err = 12; // ENOMEM;

        try {
            err = process->start();
        } catch (...) {}

        if (err) {
            status->set_state(kError);
            // FIXME
        }else {
            status->set_state(kRunning);
            status->set_pid(process->pid());

            processManager_->runAtExit(
                process->pid(),
                boost::bind(
                    &ApplicationManager::onProcessExit,
                    this,
                    process,
                    _1, // status
                    _2 // rusage
                )
            );
        }

        out->CopyFrom(*status);
    } else {
        out->CopyFrom(*status);
        out->set_message("Already running.");
    }
}

void ApplicationManager::stop(
    const ConstStopApplicationRequestPtr& request,
    const StopApplicationResponsePtr& response,
    const DoneCallback& done) {

}

void ApplicationManager::onProcessExit(
    const ProcessPtr& process,
    int status,
    const struct rusage&) {

    const std::string& appName = process->name();
    LOG_WARN << "AppManager[" << appName << "] onProcessExit";

    ApplicationMap::iterator it = apps_.find(appName);

    if (it != apps_.end()) {
        Application& app = it->second;
        app.status.set_state(kExited);

        //todo: notify master
    } else {
        LOG_ERROR << "AppManager[" << appName << "] - Unknown app ";
    }
}

void ApplicationManager::list(
    const ConstListApplicationsRequestPtr& request,
    const ListApplicationsResponsePtr& response,
    const DoneCallback& done) {

}

void ApplicationManager::remove(
    const ConstRemoveApplicationsRequestPtr& request,
    const RemoveApplicationsResponsePtr& response,
    const DoneCallback& done) {

}

}
}
}
