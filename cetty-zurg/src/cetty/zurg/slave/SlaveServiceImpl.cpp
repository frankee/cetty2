
#include <cetty/zurg/slave/SlaveServiceImpl.h>
#include <cetty/zurg/slave/Process.h>
#include <cetty/zurg/slave/ApplicationManager.h>
#include <cetty/zurg/slave/ProcessManager.h>
#include <cetty/zurg/slave/GetHardwareTask.h>
#include <cetty/util/SmallFile.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/zurg/Util.h>
#include <cetty/zurg/slave/ZurgSlave.h>

#include <boost/weak_ptr.hpp>

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::zurg;
using namespace cetty::util;
using namespace cetty::zurg::slave;

SlaveServiceImpl::SlaveServiceImpl(const EventLoopPtr& loop, int zombieInterval)
    : loop_(loop),
      psManager_(new ProcessManager(loop)),
      apps_(new ApplicationManager(loop, psManager_.get())) {
}

SlaveServiceImpl::~SlaveServiceImpl() {
}

void SlaveServiceImpl::start() {
    psManager_->start();
}

void SlaveServiceImpl::getHardware(
    const ConstGetHardwareRequestPtr& request,
    const GetHardwareResponsePtr& response,
    const DoneCallback& done) {

    LOG_INFO << "SlaveServiceImpl::getHardware - lshw = " 
             << request->lshw();

    GetHardwareTaskPtr task(new GetHardwareTask(request, done));
    task->start(this);
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
    int err = SmallFile::readFile(
                  request->file_name(),
                  request->max_size(),
                  response->mutable_content(),
                  &file_size,
                  &modify_time,
                  &create_time
              );
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
            boost::bind(
                &SlaveServiceImpl::getFileChecksumDone,
                this,
                request,
                _1, // response
                done
            )
        );
    } else {
        GetFileChecksumResponse response;
        done(&response);
    }
}

void SlaveServiceImpl::getFileChecksumDone(
    const ConstGetFileChecksumRequestPtr& request,
    const google::protobuf::Message* message,
    const DoneCallback& done){

    const RunCommandResponse* runCommandResp =
          google::protobuf::down_cast<const RunCommandResponse*>(message);

    const std::string& lines = runCommandResp->std_output();
    std::map<StringPiece, StringPiece> md5sums;
    parseMd5sum(lines, &md5sums);

    GetFileChecksumResponse response;
    for (int i = 0; i < request->files_size(); ++i){
        md5sums[request->files(i)].copy_to(response.add_md5sums());
    }
    done(&response);
}

void SlaveServiceImpl::runCommand(
    const ConstRunCommandRequestPtr& request,
    const RunCommandResponsePtr& response,
    const DoneCallback& done) {

    LOG_INFO << "SlaveServiceImpl::runCommand - " << request->command();

    ProcessPtr process(new Process(loop_,
                          const_cast<RunCommandRequestPtr &>(request),
                          response,
                          done));
    int err = 12; // ENOMEM;

    try {
        err = process->start();
    } catch (...) {}

    if (err) {
        response->set_error_code(err);
        done(response);
    } else {
        psManager_->runAtExit(
            process->pid(),  // bind strong ptr
            boost::bind(
                &Process::onCommandExit,
                process,
                _1,
                _2
            )
        );

        boost::weak_ptr<Process> weakProcessPtr(process);
        TimeoutPtr timerId = loop_->runAfter(
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
}

void SlaveServiceImpl::addApplication(
    const ConstAddApplicationRequestPtr& request,
    const AddApplicationResponsePtr& response,
    const DoneCallback& done) {

    apps_->add(request, response, done);
}

void SlaveServiceImpl::startApplications(
    const ConstStartApplicationsRequestPtr& request,
    const StartApplicationsResponsePtr& response,
    const DoneCallback& done) {
    apps_->start(request, response, done);
}

void SlaveServiceImpl::stopApplication(
    const ConstStopApplicationRequestPtr& request,
    const StopApplicationResponsePtr& response,
    const DoneCallback& done) {
    apps_->stop(request, response, done);
}

void SlaveServiceImpl::getApplications(
    const ConstGetApplicationsRequestPtr& request,
    const GetApplicationsResponsePtr& response,
    const DoneCallback& done) {
    apps_->get(request, response, done);
}

void SlaveServiceImpl::listApplications(
    const ConstListApplicationsRequestPtr& request,
    const ListApplicationsResponsePtr& response,
    const DoneCallback& done) {
    apps_->list(request, response, done);
}

void SlaveServiceImpl::removeApplications(
    const ConstRemoveApplicationsRequestPtr& request,
    const RemoveApplicationsResponsePtr& response,
    const DoneCallback& done) {
    apps_->remove(request, response, done);
}

}
}
}
