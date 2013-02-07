#include <cetty/zurg/slave/Process.h>

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <boost/weak_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <cetty/util/Process.h>
#include <cetty/util/SmallFile.h>
#include <cetty/util/StringUtil.h>
#include <cetty/logging/LoggerHelper.h>

#include <cetty/zurg/Util.h>
#include <cetty/zurg/slave/Pipe.h>
#include <cetty/zurg/slave/slave.pb.h>
#include <cetty/zurg/slave/ProcStatFile.h>

namespace cetty {
namespace zurg {
namespace slave {

using namespace boost::posix_time;
using namespace cetty::util;

const std::string Process::STDOUT_PREFIX_ = "stdout";
const std::string Process::STDERR_PREFIX_ = "stderr";

const int Process::CHILD_SLEEP_SECS_ = 1;

float getSeconds(const struct timeval& tv) {
    int64_t microSeconds = tv.tv_sec*1000000 + tv.tv_usec;
    double seconds = static_cast<double>(microSeconds)/1000000.0;
    return static_cast<float>(seconds);
}

/**
 * @brief Create a file and return file description
 *     If #toFile is true, open a file consist of #prefix.pid.#postfix
 *  (create if not exist) for reading and writing, else open "/dev/null".
 * @param toFile Hint if use a normal file
 */
int redirect(bool toFile, const std::string& prefix, const char* postfix) {
    int fd = -1;

    if (toFile) {
        char buf[256];
        ::snprintf(buf, sizeof buf, "%s.%d.%s", prefix.c_str(),
                   cetty::util::Process::id(), postfix);
        fd = ::open(buf, O_WRONLY | O_CREAT, 0644);
    }
    else {
        fd = ::open("/dev/null", O_WRONLY | O_CREAT, 0644);
    }

    if (!setFd(fd, FD_CLOEXEC)) {
        LOG_ERROR << "Set FD_CLOEXEC failed";
    }

    return fd;
}

std::string getFileConent(const std::string& fileName, bool remove) {
    SmallFile sf(fileName);
    int size = -1;
    int err = sf.readToBuffer(&size);

    if (remove) { unlink(fileName.c_str()); }

    if (!err) { return std::string().assign(sf.buffer(), size); }
    else { return std::string(); }
}

Process::Process(
    const ConstRunCommandRequestPtr& request,
    const RunCommandResponsePtr& response,
    const DoneCallback& done,
    bool redirectStdout,
    bool redirectStderr,
    const std::string& slaveName,
    const std::string& slavePrefix)
    : request_(request),
      response_(response),
      doneCallback_(done),
      childPid_(0),
      startTimeInJiffies_(0),
      redirectStdout_(redirectStdout),
      redirectStderr_(redirectStdout),
      slaveName_(slaveName),
      slavePrefix_(slavePrefix),
      runCommand_(true) {

}

Process::Process(const AddApplicationRequestPtr& appRequest,
    const std::string& slaveName,
    const std::string& slavePrefix)
    : request_(),
      response_(new RunCommandResponse()),
      doneCallback_(),
      childPid_(0),
      name_(appRequest->name()),
      startTimeInJiffies_(0),
      redirectStdout_(appRequest->redirect_stdout()),
      redirectStderr_(appRequest->redirect_stderr()),
      slaveName_(slaveName),
      slavePrefix_(slavePrefix),
      runCommand_(false) {

    // Transform application request to run command request.
    RunCommandRequestPtr request = new RunCommandRequest;
    request->set_command(appRequest->binary());
    if(!appRequest->cwd().empty()) {
    	request->set_cwd(appRequest->cwd());
    }
    else {
        StringUtil::printf(request->mutable_cwd(), "%s/%s/%s",
            slavePrefix_.c_str(),
            slaveName_.c_str(),
            appRequest->name().c_str());
    }

    request->mutable_args()->CopyFrom(appRequest->args());
    request->mutable_envs()->CopyFrom(appRequest->envs());
    request->set_envs_only(appRequest->envs_only());
    request->set_max_stdout(0);
    request->set_max_stderr(0);
    request->set_timeout(-1);
    request->set_max_memory_mb(appRequest->max_memory_mb());

    request_ = request;
}

Process::~Process() {
    LOG_DEBUG << "Process[" << childPid_ << "] dtor";
}

int Process::start() {
    startTime_ = boost::posix_time::microsec_clock::universal_time();

    Pipe execError;
    Pipe stdOutput;
    Pipe stdError;

    pid_t result = ::fork();

    if (result == 0) {
        int ret = ::mkdir(request_->cwd().c_str(), 0755);

        if (ret < 0) {
            if (errno == EEXIST) {
                LOG_INFO << request_->cwd()
                         << " is already exist.";
            }
            else {
                char buf[512];
                char* error = strerror_r(errno, buf, sizeof(buf));
                LOG_INFO << "Make process's work directory : "
                         << request_->cwd() << " error :" << error;
                exit(1);
            }
        }

        int stdoutFd = -1;
        int stderrFd = -1;

        std::string startTimeStr = to_iso_string(startTime_);
        stdoutFd = redirect(redirectStdout_,
                            request_->cwd() + "/" + STDOUT_PREFIX_,
                            startTimeStr.c_str());

        stderrFd = redirect(redirectStderr_,
                            request_->cwd() + "/" + STDERR_PREFIX_,
                            startTimeStr.c_str());

        execChild(execError, stdoutFd, stderrFd);
    }
    else if (result > 0) {
        childPid_ = result;
        return afterFork(execError, stdOutput, stdError);
    }
    else {
        return errno;
    }
}

void Process::execChild(Pipe& execError, int stdOutput, int stdError) {
    if (CHILD_SLEEP_SECS_ > 0) {
        ::fprintf(stderr, "Child pid = %d", ::getpid());
        ::sleep(CHILD_SLEEP_SECS_);
    }

    try {
        execError.closeRead();

        if (::dup2(stdError, STDERR_FILENO) < 0) {
            LOG_ERROR << "Duplicate stderr failed.";
            throw static_cast<int>(errno);
        }
        close(stdError);

        ProcStatFile stat(cetty::util::Process::id());

        if (!stat.valid_) {
            LOG_INFO << "Process [pid = "
                     << cetty::util::Process::id()
                     << "]state is not valid.";
            stat.startTime_ = 0;
        }

        execError.write(stat.startTime_);

        std::vector<const char*> argv;
        argv.reserve(request_->args_size() + 2);
        argv.push_back(request_->command().c_str());

        for (int i = 0; i < request_->args_size(); ++i) {
            argv.push_back(request_->args(i).c_str());
        }

        argv.push_back(NULL);

        if (::chdir(request_->cwd().c_str()) < 0) {
            LOG_ERROR << "Set process's work directory failed.";
            throw static_cast<int>(errno);
        }

        int stdInput = ::open("/dev/null", O_RDONLY);

        if (stdInput < 0) {
            LOG_ERROR << "Open /dev/null failed.";
            throw static_cast<int>(errno);
        }

        if (stdOutput < 0 || stdError < 0) {
            if (stdOutput >= 0) { ::close(stdOutput); }

            if (stdError >= 0) { ::close(stdError); }

            throw static_cast<int>(EACCES);
        }

        if (::dup2(stdInput, STDIN_FILENO) < 0) {
            LOG_ERROR << "Duplicate stdin failed.";
            throw static_cast<int>(errno);
        }
        ::close(stdInput);

        if (::dup2(stdOutput, STDOUT_FILENO) < 0) {
            LOG_ERROR << "Duplicate stdout failed.";
            throw static_cast<int>(errno);
        }
        ::close(stdOutput);

        if (!setFl(STDOUT_FILENO, O_SYNC)) {
            LOG_ERROR << "Set file O_SYNC failed.";
        }

        const char* cmd = request_->command().c_str();
        ::execvp(cmd, const_cast<char**>(&*argv.begin()));

        LOG_ERROR << "Execute new process image failed.";
        throw static_cast<int>(errno);
    }
    catch (int error) {
        execError.write(error);
        char buf[512];
        fprintf(stderr, "CHILD %s (errno=%d)\n",
                strerror_r(error, buf, sizeof buf),
                error);
    }
    catch (...) {
        int error = EINVAL;
        execError.write(error);
    }

    ::exit(1);
}

int Process::afterFork(Pipe& execError, Pipe& stdOutput, Pipe& stdError) {
    LOG_INFO << "PARENT child pid " << childPid_;

    execError.closeWrite();
    stdOutput.closeWrite();
    stdError.closeWrite();

    int64_t childStartTime = 0;
    int64_t childErrno = 0;

    ssize_t n = execError.read(&childStartTime);

    if (n != sizeof(childStartTime) || childStartTime == 0) {
        LOG_ERROR << "PARENT read start time from child ["
                  << childPid_
                  << "] error. ";
    }

    startTimeInJiffies_ = childStartTime;

    n = execError.read(&childErrno);

    if (n == 0) {
        LOG_INFO << "PARENT child process ["
                 << childPid_
                 << "] start successfully";

        char filename[64];
        ::snprintf(filename, sizeof filename, "/proc/%d/exe", childPid_);
        char buf[1024];
        ssize_t len = ::readlink(filename, buf, sizeof buf);

        if (len >= 0) {
            exeFile_.assign(buf, len);
            LOG_INFO << filename << " -> " << exeFile_;
        }
        else {
            LOG_ERROR << "Fail to read link " << filename;
        }


        if (runCommand_) {

        }
        else {

        }

        return 0;
    }
    else if (n == sizeof(childErrno)) {
        int err = static_cast<int>(childErrno);
        char buf[512];
        LOG_ERROR << "PARENT child error " << strerror_r(err, buf, sizeof buf)
                  << " (errno=" << childErrno << ")";
        return err;
    }
    else if (n < 0) {
        int err = errno;
        LOG_ERROR << "PARENT child errno: " << err;
        return err;
    }
    else {
        LOG_ERROR << "PARENT read :" << n;
        return EINVAL;
    }

    execError.closeRead();
    stdOutput.closeRead();
    stdError.closeRead();
}

void Process::onTimeoutWeak(const boost::weak_ptr<Process>& wkPtr) {
    ProcessPtr ptr(wkPtr.lock());

    if (ptr) { ptr->onTimeout(); }
}

void Process::onTimeout() {
    LOG_INFO << "Process[" << childPid_ << "] onTimeout";

    const ProcStatFile stat(childPid_);

    if (stat.valid_
        && stat.ppid_ == cetty::util::Process::id()
        && stat.startTime_ == startTimeInJiffies_) {

        int ret = ::kill(childPid_, SIGINT);

        if (ret < 0 && errno == ESRCH) {
            LOG_INFO << "Process [" << childPid_ << " is not exist.";
        }
    }
}

void Process::onCommandExit(const int status, const struct rusage& ru) {
    assert(response_);

    char buf[256];

    if (WIFEXITED(status)) {
        snprintf(buf, sizeof buf, "exit status %d", WEXITSTATUS(status));
        response_->set_exit_status(WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status)) {
        snprintf(buf, sizeof buf, "signaled %d%s",
                 WTERMSIG(status), WCOREDUMP(status) ? " (core dump)" : "");
        response_->set_signaled(WTERMSIG(status));
        response_->set_coredump(WCOREDUMP(status));
    }

    LOG_INFO << "Process[" << childPid_ << "] onCommandExit " << buf;

    response_->set_error_code(0);
    response_->set_pid(childPid_);
    response_->set_status(status);
    response_->set_std_output(getCommandOutput());
    response_->set_std_error(getCommandError());
    response_->set_executable_file(exeFile_);
    response_->set_start_time_us(getMicroSecs(startTime_));
    response_->set_finish_time_us(getMicroSecs(microsec_clock::local_time()));
    response_->set_user_time(getSeconds(ru.ru_utime));
    response_->set_system_time(getSeconds(ru.ru_stime));
    response_->set_memory_maxrss_kb(ru.ru_maxrss);

    doneCallback_(response_);
}

std::string Process::getCommandOutput() {
    if (!redirectStdout_) {
        return std::string("Standard out is redirected to /dev/null");
    }

    std::string startTimeStr = to_iso_string(startTime_);
    char buf[256];
    bzero(buf, sizeof(buf));
    ::snprintf(buf, sizeof buf, "%s.%d.%s",
               (request_->cwd() + "/" + STDOUT_PREFIX_).c_str(),
               childPid_, startTimeStr.c_str());
    std::string fileName(buf);
    return getFileConent(fileName, true);
}

std::string Process::getCommandError() {
    if (!redirectStderr_) {
        return std::string("Standard error is redirected to /dev/null");
    }

    std::string startTimeStr = to_iso_string(startTime_);
    char buf[256];
    bzero(buf, sizeof(buf));
    ::snprintf(buf, sizeof buf, "%s.%d.%s",
               (request_->cwd() + "/" + STDERR_PREFIX_).c_str(),
               childPid_, startTimeStr.c_str());
    std::string fileName(buf);
    return getFileConent(fileName, true);
}

}
}
}
