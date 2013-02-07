#include <cetty/zurg/slave/SlaveServiceImpl.h>

#include <boost/weak_ptr.hpp>
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

void SlaveServiceImpl::init() {
	AddApplicationRequestPtr request = NULL;
	AddApplicationResponsePtr response = NULL;

	StartApplicationsRequestPtr startRequest = NULL;
	StartApplicationsResponsePtr startResponse = NULL;

	RemoveApplicationsRequestPtr removeRequest = NULL;
	RemoveApplicationsResponsePtr removeResponse = NULL;

	DoneCallback emptyCallback;

	size_t i = 0;
	for(; i < config_.applications.size(); ++i) {
		std::string name = config_.applications.at(i)->name;

        request = new AddApplicationRequest();
        request->set_name(name);
        request->set_binary(config_.applications.at(i)->binary);
        request->set_cwd(config_.applications.at(i)->cwd);

        size_t j = 0;
        for(; j < config_.applications.at(i)->args.size(); ++ j){
        	request->add_args(config_.applications.at(i)->args.at(j));
        }

        for(j = 0; j < config_.applications.at(i)->envs.size(); ++j)
        	request->add_envs(config_.applications.at(i)->envs.at(j));

        if (config_.applications.at(i)->redirectStdout)
            request->set_redirect_stdout(config_.applications.at(i)->redirectStdout.get());
        if(config_.applications.at(i)->redirectStderr)
        	request->set_redirect_stderr(config_.applications.at(i)->redirectStderr.get());

        if(config_.applications.at(i)->autoRecover)
        	request->set_auto_recover(config_.applications.at(i)->autoRecover.get());


        response = new AddApplicationResponse();

        addApplication(request, response, emptyCallback);
        LOG_INFO << "add application " << name;

        delete request;
        delete response;

        startRequest = new StartApplicationsRequest();
        startResponse = new StartApplicationsResponse();
        startRequest->add_names()->assign(name);
        startApplications(startRequest, startResponse, emptyCallback);

        if(startResponse->status(0).state() == kError){
        	LOG_ERROR << "start application " << name << " failed";

        	removeRequest = new RemoveApplicationsRequest();
        	removeResponse = new RemoveApplicationsResponse();
            removeRequest->add_name(name);
            removeApplications(removeRequest, removeResponse, emptyCallback);

            delete removeRequest;
            delete removeResponse;
        }

        LOG_INFO << "application [" << name << "] "
        		 << startResponse->status(0).message();

        delete startRequest;
        delete startResponse;
	}
}

SlaveServiceImpl::~SlaveServiceImpl() {

}

void SlaveServiceImpl::getHardware(
    const ConstGetHardwareRequestPtr& request,
    const GetHardwareResponsePtr& response,
    const DoneCallback& done) {

    LOG_INFO << "SlaveServiceImpl::getHardware - lshw = "
             << request->lshw();

    GetHardwareTaskPtr task(new GetHardwareTask(request, response, done, *this));
}

void SlaveServiceImpl::getFileContent(
    const ConstGetFileContentRequestPtr& request,
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

void SlaveServiceImpl::getFileChecksum(
    const ConstGetFileChecksumRequestPtr& request,
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

void SlaveServiceImpl::getFileChecksumDone(
    const ConstGetFileChecksumRequestPtr& request,
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

void SlaveServiceImpl::runCommand(
    const ConstRunCommandRequestPtr& request,
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

void SlaveServiceImpl::runScript(
    const ConstRunScriptRequestPtr& request,
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

void SlaveServiceImpl::listProcesses(
    const ConstListProcessesRequestPtr& request,
    const ListProcessesResponsePtr& response,
    const DoneCallback& done) {

}

void SlaveServiceImpl::addApplication(
    const ConstAddApplicationRequestPtr& request,
    const AddApplicationResponsePtr& response,
    const DoneCallback& done) {

    applicationManager_->add(request, response, done);
}

void SlaveServiceImpl::startApplications(
    const ConstStartApplicationsRequestPtr& request,
    const StartApplicationsResponsePtr& response,
    const DoneCallback& done) {
    applicationManager_->start(request, response, done);
}

void SlaveServiceImpl::stopApplications(
    const ConstStopApplicationRequestPtr& request,
    const StopApplicationResponsePtr& response,
    const DoneCallback& done) {
    applicationManager_->stop(request, response, done);
}

void SlaveServiceImpl::listApplications(
    const ConstListApplicationsRequestPtr& request,
    const ListApplicationsResponsePtr& response,
    const DoneCallback& done) {
    applicationManager_->list(request, response, done);
}

void SlaveServiceImpl::removeApplications(
    const ConstRemoveApplicationsRequestPtr& request,
    const RemoveApplicationsResponsePtr& response,
    const DoneCallback& done) {
    applicationManager_->remove(request, response, done);
}

}
}
}
