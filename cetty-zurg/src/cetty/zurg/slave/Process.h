#ifndef MUDUO_PROTORPC_ZURG_PROCESS_H
#define MUDUO_PROTORPC_ZURG_PROCESS_H

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <cetty/zurg/proto/slave.pb.h>

struct rusage;

namespace cetty {
namespace zurg {

class Pipe;
class Sink;

class Process : public boost::enable_shared_from_this<Process>,
        boost::noncopyable {
public:
    Process(const EventLoopPtr& loop,
            const ConstRunCommandRequestPtr& request,
            const RunCommandResponsePtr& response,
            const DoneCallback& done);

    Process(const AddApplicationRequestPtr& request);

    ~Process();

    int start(); // may throw

    pid_t pid() const { return pid_; }
    const std::string& name() const { return name_; }

    void setTimerId(const muduo::net::TimerId& timerId) {
        timerId_ = timerId;
    }

    void onCommandExit(int status, const struct ::rusage&);
    void onTimeout();

    static void onTimeoutWeak(const boost::weak_ptr<Process>& wkPtr);

private:
    void execChild(Pipe& execError, int stdOutput, int stdError)
    __attribute__((__noreturn__));;

    int afterFork(Pipe& execError, Pipe& stdOutput, Pipe& stdError);

private:
    EventLoopPtr loop_;
    ConstRunCommandRequestPtr request_;
    RunCommandResponsePtr response;
    DoneCallback doneCallback_;

    pid_t pid_;
    std::string name_;
    std::string exe_file_;

    boost::posix_time::ptime startTime_;
    int64_t startTimeInJiffies_;
    cetty::channel::TimeoutPtr timerId_;

    boost::shared_ptr<Sink> stdoutSink_;
    boost::shared_ptr<Sink> stderrSink_;

    bool redirectStdout_;
    bool redirectStderr_;
    bool runCommand_;
};

typedef boost::shared_ptr<Process> ProcessPtr;

}
}

#endif  // MUDUO_PROTORPC_ZURG_PROCESS_H
