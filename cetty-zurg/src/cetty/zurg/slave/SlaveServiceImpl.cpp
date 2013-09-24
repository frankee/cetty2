#include <cetty/zurg/slave/SlaveServiceImpl.h>

#include <boost/weak_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <cetty/util/SmallFile.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/channel/EventLoopPool.h>

#include <cetty/zurg/Util.h>
#include <cetty/zurg/slave/Process.h>
#include <cetty/zurg/slave/ApplicationManager.h>
#include <cetty/zurg/slave/ProcessManager.h>
#include <cetty/zurg/slave/GetHardwareTask.h>

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::zurg;
using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::config;

SlaveServiceImpl::SlaveServiceImpl(const EventLoopPtr& loop) {
    ConfigCenter::instance().configure(&config_);
    processManager_.reset(new ProcessManager(loop));
    applicationManager_.reset(new ApplicationManager(processManager_.get()));

    // start all configured applications
    init();
}

SlaveServiceImpl::~SlaveServiceImpl() {

}

void SlaveServiceImpl::getHardware(const ConstGetHardwareRequestPtr& request,
                                   const GetHardwareResponsePtr& response,
                                   const DoneCallback& done) {

    LOG_INFO << "SlaveServiceImpl::getHardware - lshw = "
             << request->lshw();

    GetHardwareTaskPtr task(new GetHardwareTask(request, response, done, *this));
}

void SlaveServiceImpl::getFileContent(const ConstGetFileContentRequestPtr& request,
                                      const GetFileContentResponsePtr& response,
                                      const DoneCallback& done) {

    LOG_INFO << "SlaveServiceImpl::getFileContent - " << request->file_name()
             << " maxSize = " << request->max_size();

    int64_t file_size = 0;
    int64_t modify_time = 0;
    int64_t create_time = 0;
    int err = SmallFile::readFile(request->file_name(),
                                  request->max_size(),
                                  response->mutable_content(),
                                  &file_size,
                                  &modify_time,
                                  &create_time);
    response->set_error_code(err);
    response->set_file_size(file_size);
    response->set_modify_time(modify_time);
    response->set_create_time(create_time);

    LOG_DEBUG << "SlaveServiceImpl::getFileContent - err " << err;
    done(response);
}

void SlaveServiceImpl::getFileChecksum(const ConstGetFileChecksumRequestPtr& request,
                                       const GetFileChecksumResponsePtr& response,
                                       const DoneCallback& done) {

    DLOG_DEBUG ;

    if (request->files_size() > 0) {
        RunCommandRequestPtr runCommandReq(new RunCommandRequest);
        runCommandReq->set_command("md5sum");

        for (int i = 0; i < request->files_size(); ++i) {
            assert(request->files(i).find('\n') == std::string::npos);
            runCommandReq->add_args(request->files(i));
        }

        runCommand(
            runCommandReq,
            NULL,
            boost::bind(&SlaveServiceImpl::getFileChecksumDone,
                        this, request, _1, done)
        );
    }
    else {
        GetFileChecksumResponse response;
        done(&response);
    }
}

void SlaveServiceImpl::getFileChecksumDone(const ConstGetFileChecksumRequestPtr& request,
        const google::protobuf::Message* message,
        const DoneCallback& done) {

    const RunCommandResponse* runCommandResp =
        google::protobuf::down_cast<const RunCommandResponse*>(message);

    const std::string& lines = runCommandResp->std_output();
    std::map<StringPiece, StringPiece> md5sums;
    parseMd5sum(lines, &md5sums);

    GetFileChecksumResponse response;

    for (int i = 0; i < request->files_size(); ++i) {
        md5sums[request->files(i)].copy_to(response.add_md5sums());
    }

    done(&response);
}

void SlaveServiceImpl::runCommand(const ConstRunCommandRequestPtr& request,
                                  const RunCommandResponsePtr& response,
                                  const DoneCallback& done) {

    LOG_INFO << "SlaveServiceImpl::runCommand - " << request->command();

    ProcessPtr process(new Process(request, response, done, true, true,
                                   config_.slaveName,
                                   config_.prefix));

    int err = 12; // ENOMEM;

    try {
        err = process->start();
    }
    catch (...) {}

    if (err) {
        response->set_error_code(err);
        done(response);
    }
    else {
        processManager_->runAtExit(
            process->pid(),  // bind strong ptr
            boost::bind(&Process::onCommandExit,
                        process, _1, _2)
        );

        EventLoopPtr elp = EventLoopPool::current();
        assert(elp);
        boost::weak_ptr<Process> weakProcessPtr(process);
        TimeoutPtr timerId = elp->runAfter(
                                 request->timeout(),
                                 boost::bind(&Process::onTimeoutWeak, weakProcessPtr)
                             );

        process->setTimerId(timerId);
    }
}

void SlaveServiceImpl::runScript(const ConstRunScriptRequestPtr& request,
                                 const RunCommandResponsePtr& response,
                                 const DoneCallback& done) {

    /*
    RunCommandRequestPtr runCommandReq(new RunCommandRequest);
    std::string scriptFile = writeTempFile(ZurgSlave::instance().getName(), request->script());

    if (!scriptFile.empty()) {
        LOG_INFO << "runScript - write to " << scriptFile;

        // FIXME: interpreter
        runCommandReq->set_command(scriptFile);

        // FIXME: others
        runCommand(runCommandReq, NULL, done);
    }else {
        LOG_ERROR << "runScript - failed to write script file";
        // FIXME: done
    }
    */
}

void SlaveServiceImpl::listProcesses(const ConstListProcessesRequestPtr& request,
                                     const ListProcessesResponsePtr& response,
                                     const DoneCallback& done) {

}

void SlaveServiceImpl::addApplication(const ConstAddApplicationRequestPtr& request,
                                      const AddApplicationResponsePtr& response,
                                      const DoneCallback& done) {
    applicationManager_->add(request, response, done);
}

void SlaveServiceImpl::startApplications(const ConstStartApplicationsRequestPtr& request,
        const StartApplicationsResponsePtr& response,
        const DoneCallback& done) {
    applicationManager_->start(request, response, done);
}

void SlaveServiceImpl::stopApplications(const ConstStopApplicationRequestPtr& request,
                                        const StopApplicationResponsePtr& response,
                                        const DoneCallback& done) {
    applicationManager_->stop(request, response, done);
}

void SlaveServiceImpl::listApplications(const ConstListApplicationsRequestPtr& request,
                                        const ListApplicationsResponsePtr& response,
                                        const DoneCallback& done) {
    applicationManager_->list(request, response, done);
}

void SlaveServiceImpl::removeApplications(const ConstRemoveApplicationsRequestPtr& request,
        const RemoveApplicationsResponsePtr& response,
        const DoneCallback& done) {
    applicationManager_->remove(request, response, done);
}

void SlaveServiceImpl::init() {
    for (size_t i = 0; i < config_.applications.size(); ++i) {
        SlaveServiceConfig::Application* application = config_.applications[i];
        addApplicationFromConfig(application);

        if (application->autoStart) {
            startApplication(application->name);
        }
    }
}

void SlaveServiceImpl::addApplicationFromConfig(SlaveServiceConfig::Application* application) {
    std::string name = application->name;
    boost::scoped_ptr<AddApplicationRequest> request(new AddApplicationRequest);

    request->set_name(name);
    request->set_binary(application->binary);
    request->set_cwd(application->cwd);

    request->set_redirect_stdout(application->redirectStdout);
    request->set_redirect_stderr(application->redirectStderr);
    request->set_auto_recover(application->autoRecover);
    request->set_log_file(application->logFile);

    std::string args;

    for (std::size_t j; j < application->args.size(); ++ j) {
        request->add_args(application->args[j]);
        args += application->args[j];
        args += " ";
    }

    for (std::size_t j = 0; j < application->envs.size(); ++j) {
        request->add_envs(application->envs.at(j));
    }

    boost::scoped_ptr<AddApplicationResponse> response(new AddApplicationResponse);
    addApplication(request.get(),
                   response.get(),
                   DoneCallback());
    LOG_INFO << "add application: " << name
             << " bin: " << application->binary
             << " cwd: " << application->cwd
             << " args: " << args;
}

void SlaveServiceImpl::startApplication(const std::string& name) {
    boost::scoped_ptr<StartApplicationsRequest> request(new StartApplicationsRequest);
    boost::scoped_ptr<StartApplicationsResponse> response(new StartApplicationsResponse);

    request->add_names()->assign(name);
    startApplications(request.get(),
                      response.get(),
                      DoneCallback());

    if (response->status(0).state() == kError) {
        LOG_ERROR << "start application " << name << " failed";
        removeApplication(name);
    }
    else {
        LOG_INFO << "application [" << name << "] "
                 << response->status(0).message();
    }
}

void SlaveServiceImpl::removeApplication(const std::string& name) {
    boost::scoped_ptr<RemoveApplicationsRequest> request(new RemoveApplicationsRequest);
    boost::scoped_ptr<RemoveApplicationsResponse> response(new RemoveApplicationsResponse);
    request->add_name(name);

    removeApplications(request.get(),
                       response.get(),
                       DoneCallback());
}

}
}
}
