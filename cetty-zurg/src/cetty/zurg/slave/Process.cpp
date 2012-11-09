#include <cetty/zurg/slave/Process.h>
#include <cetty/util/Process.h>
#include <cetty/zurg/slave/ZurgSlave.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/zurg/slave/slave.pb.h>
#include <cetty/zurg/Util.h>
#include <cetty/zurg/slave/Pipe.h>
#include <cetty/zurg/slave/ProcStatFile.h>

#include <boost/weak_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

namespace cetty{
namespace zurg {
namespace slave {

using namespace boost::posix_time;

const int kSleepAfterExec = 0; // for strace child

float getSeconds(const struct timeval& tv) {
    int64_t microSeconds = tv.tv_sec*1000000 + tv.tv_usec;
    double seconds = static_cast<double>(microSeconds)/1000000.0;
    return static_cast<float>(seconds);
}

int redirect(bool toFile, const std::string& prefix, const char* postfix) {
    int fd = -1;

    if (toFile) {
        char buf[256];
        ::snprintf(buf, sizeof buf, "%s.%d.%s",
                   prefix.c_str(), cetty::util::Process::id(), postfix);
        fd = ::open(buf, O_WRONLY | O_CREAT | O_CLOEXEC, 0644);
    } else {
        fd = ::open("/dev/null", O_WRONLY | O_CREAT | O_CLOEXEC, 0644);
    }

    return fd;
}

Process::Process(
    const ConstRunCommandRequestPtr& request,
    const RunCommandResponsePtr& response,
    const DoneCallback& done
): request_(request),
   response_(response),
   doneCallback_(done),
   pid_(0),
   startTimeInJiffies_(0),
   redirectStdout_(false),
   redirectStderr_(false),
   runCommand_(true) {

}

Process::Process(const AddApplicationRequestPtr& appRequest)
    : request_(),
      response_(new RunCommandResponse()),
      doneCallback_(),
      pid_(0),
      name_(appRequest->name()),
      startTimeInJiffies_(0),
      redirectStdout_(appRequest->redirect_stdout()),
      redirectStderr_(appRequest->redirect_stderr()),
      runCommand_(false) {

    // Transform application request to run command request.
    RunCommandRequestPtr request = new RunCommandRequest;
    request->set_command(appRequest->binary());

    char dir[256];
    snprintf(dir, sizeof dir, "%s/%s/%s",
             ZurgSlave::instance().getPrefix().c_str(),
             ZurgSlave::instance().getName().c_str(),
             appRequest->name().c_str());

    request->set_cwd(dir);

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
    LOG_DEBUG << "Process[" << pid_ << "] dtor";

    //todo complete destructor like below

    /*
    if (stdoutSink_ || stderrSink_) {
        // todo how to judge if the cetty's event loop is working.
        assert(!loop_->eventHandling());
    }
    */
}

int Process::start() {
    // todo what's mean
    //assert(numThreads() == 1);
    int availabltFds = maxOpenFiles() - openedFiles();

    if (availabltFds < 20) {
        // to fork() and capture stdout/stderr, we need new file descriptors.
        return EMFILE;
    }

    // The self-pipe trick
    // http://cr.yp.to/docs/selfpipe.html
    Pipe execError;

    Pipe stdOutput;
    Pipe stdError;

    startTime_ = boost::posix_time::microsec_clock::universal_time();

    const pid_t result = ::fork(); // FORKED HERE

    if (result == 0) {
        // child process

        int ret = ::mkdir(request_->cwd().c_str(), 0755);
        if(ret < 0){
            char buf[512];
            strerror_r(errno, buf, sizeof(buf));
            LOG_INFO << "Change process's work directory to "
                     << request_->cwd() << " error :" << buf;
        }

        int stdoutFd = -1;
        int stderrFd = -1;

        if (runCommand_) {
            stdoutFd = stdOutput.writeFd();
            stderrFd = stdError.writeFd();
        } else {
            std::string startTimeStr = to_iso_string(startTime_);
            // FIXME: capture errno
            stdoutFd = redirect(redirectStdout_, request_->cwd()+"/stdout", startTimeStr.c_str());
            stderrFd = redirect(redirectStderr_, request_->cwd()+"/stderr", startTimeStr.c_str());
        }

        execChild(execError, stdoutFd, stderrFd); // never return
    } else if (result > 0) {
        // parent
        pid_ = result;
        return afterFork(execError, stdOutput, stdError);
    } else {
        // failed
        return errno;
    }
}

void Process::execChild(Pipe& execError, int stdOutput, int stdError) {
    if (kSleepAfterExec > 0) {
        ::fprintf(stderr, "Child pid = %d", ::getpid());
        ::sleep(kSleepAfterExec);
    }

    try {
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

        // FIXME: new process group, new session
       // ::sigprocmask(SIG_SETMASK, &oldSigmask, NULL);
        if (::chdir(request_->cwd().c_str()) < 0) {
            LOG_INFO << "Set process's work directory failed.";
            throw static_cast<int>(errno);
        }

        // FIXME: max_memory_mb
        // FIXME: environ with execvpe
        int stdInput = ::open("/dev/null", O_RDONLY);

        if (stdInput < 0) {
            LOG_INFO << "Open /dev/null failed.";
            throw static_cast<int>(errno);
        }

        if (stdOutput < 0 || stdError < 0) {
            if (stdOutput >= 0) { ::close(stdOutput); }

            if (stdError >= 0) { ::close(stdError); }

            throw static_cast<int>(EACCES);
        }

        if (::dup2(stdInput, STDIN_FILENO) < 0) {
            LOG_INFO << "Duplicate stdin failed.";
            throw static_cast<int>(errno);
        }

        ::close(stdInput);

        if (::dup2(stdOutput, STDOUT_FILENO) < 0) {
            LOG_INFO << "Duplicate stdout failed.";
            throw static_cast<int>(errno);
        }

        if (::dup2(stdError, STDERR_FILENO) < 0) {
            LOG_INFO << "Duplicate stderr failed.";
            throw static_cast<int>(errno);
        }

        LOG_INFO << "Run command "<< request_->command();
        const char* cmd = request_->command().c_str();
        ::execvp(cmd, const_cast<char**>(&*argv.begin()));

        LOG_INFO << "Execute new process image failed.";
        throw static_cast<int>(errno);
    } catch (int error) {
        execError.write(error);
        char buf[512];
        fprintf(stderr, "CHILD %s (errno=%d)\n",
                strerror_r(error, buf, sizeof buf),
                error);
    } catch (...) {
        int error = EINVAL;
        execError.write(error);
    }
    ::exit(1);
}

int Process::afterFork(Pipe& execError, Pipe& stdOutput, Pipe& stdError) {
    LOG_DEBUG << "PARENT child pid " << pid_;
    int64_t childStartTime = 0;
    int64_t childErrno = 0;
    execError.closeWrite();
    ssize_t n = execError.read(&childStartTime);
    assert(n == sizeof(childStartTime));
    startTimeInJiffies_ = childStartTime;
    LOG_DEBUG << "PARENT child start time " << childStartTime;
    n = execError.read(&childErrno);

    if (n == 0) {
        LOG_INFO << "started child pid " << pid_;

        char filename[64];
        ::snprintf(filename, sizeof filename, "/proc/%d/exe", pid_);
        char buf[1024];
        ssize_t len = ::readlink(filename, buf, sizeof buf);

        if (len >= 0) {
            exe_file_.assign(buf, len);
            LOG_INFO << filename << " -> " << exe_file_;
        } else {
            LOG_ERROR << "Fail to read link " << filename;
        }

        // read nothing, child process exec successfully.
        if (runCommand_) {
            int stdoutFd = ::dup(stdOutput.readFd());
            int stderrFd = ::dup(stdError.readFd());

            if (stdoutFd < 0 || stderrFd < 0) {
                if (stdoutFd >= 0) { ::close(stdoutFd); }

                if (stderrFd >= 0) { ::close(stderrFd); }

                return errno;
            }

            setNonBlockAndCloseOnExec(stdoutFd);
            setNonBlockAndCloseOnExec(stderrFd);
            //stdoutSink_.reset(new Sink(loop_, stdoutFd, request_->max_stdout(), "stdout"));
            //stderrSink_.reset(new Sink(loop_, stderrFd, request_->max_stderr(), "stderr"));
            //stdoutSink_->start();
            //stderrSink_->start();
        } else {
            // FIXME: runApp
        }

        return 0;
    } else if (n == sizeof(childErrno)) {
        int err = static_cast<int>(childErrno);
        char buf[512];
        LOG_ERROR << "PARENT child error " << strerror_r(err, buf, sizeof buf)
                  << " (errno=" << childErrno << ")";
        return err;
    } else if (n < 0) {
        int err = errno;
        LOG_ERROR << "PARENT";
        return err;
    } else {
        LOG_ERROR << "PARENT read " << n;
        return EINVAL;
    }
}

void Process::onTimeoutWeak(const boost::weak_ptr<Process>& wkPtr) {
    ProcessPtr ptr(wkPtr.lock());

    if (ptr) {
        ptr->onTimeout();
    }
}

void Process::onTimeout() {
    // kill process
    LOG_INFO << "Process[" << pid_ << "] onTimeout";

    const ProcStatFile stat(pid_);

    if (stat.valid_
        && stat.ppid_ == cetty::util::Process::id()
        && stat.startTime_ == startTimeInJiffies_) {
        ::kill(pid_, SIGINT);
    }
}

#pragma GCC diagnostic ignored "-Wold-style-cast"
void Process::onCommandExit(const int status, const struct rusage& ru) {
    assert(response_);

    char buf[256];

    if (WIFEXITED(status)) {
        snprintf(buf, sizeof buf, "exit status %d", WEXITSTATUS(status));
        response_->set_exit_status(WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        snprintf(buf, sizeof buf, "signaled %d%s",
                 WTERMSIG(status), WCOREDUMP(status) ? " (core dump)" : "");
        response_->set_signaled(WTERMSIG(status));
        response_->set_coredump(WCOREDUMP(status));
    }

    LOG_INFO << "Process[" << pid_ << "] onCommandExit " << buf;

    // todo how to replace below code by cetty's event loop
    //assert(!loop_->eventHandling());

    // FIXME: defer 100ms or blocking read to capture all outputs.
    //stdoutSink_->stop(pid_);
    //stderrSink_->stop(pid_);

    response_->set_error_code(0);
    response_->set_pid(pid_);
    response_->set_status(status);
    //response_->set_std_output(stdoutSink_->bufferAsStdString());
    //response_->set_std_error(stderrSink_->bufferAsStdString());
    response_->set_executable_file(exe_file_);
    response_->set_start_time_us(getMicroSecs(startTime_));
    response_->set_finish_time_us(getMicroSecs(microsec_clock::local_time()));
    response_->set_user_time(getSeconds(ru.ru_utime));
    response_->set_system_time(getSeconds(ru.ru_stime));
    response_->set_memory_maxrss_kb(ru.ru_maxrss);

    doneCallback_(response_);
}

}
}
}
