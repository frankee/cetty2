/*
 * Process.h
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#include <cetty/zurg/slave/slave.pb.h>
#include <cetty/protobuf/service/ProtobufService.h>
#include <cetty/channel/EventLoopPtr.h>
#include <cetty/channel/EventLoop.h>
#include <cetty/channel/TimeoutPtr.h>
#include <cetty/channel/Timeout.h>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <string>

struct rusage;

namespace cetty {
namespace zurg {
namespace slave {

class Pipe;
class Process;

using namespace cetty::protobuf::service;
using namespace cetty::channel;
using namespace boost::posix_time;

typedef boost::shared_ptr<Process> ProcessPtr;
typedef boost::function1<void, const MessagePtr&> DoneCallback;

class Process : public boost::enable_shared_from_this<Process>,
                boost::noncopyable {
public:
    Process(
        const ConstRunCommandRequestPtr& request,
        const RunCommandResponsePtr& response,
        const DoneCallback& done
    );

    Process(const AddApplicationRequestPtr& request);

    ~Process();

    int start(); // may throw

    pid_t pid() const { return pid_; }
    const std::string& name() const { return name_; }

    void setTimerId(const cetty::channel::TimeoutPtr timerId) {
        timerId_ = timerId;
    }

    void onCommandExit(int status, const struct ::rusage&);
    void onTimeout();

    static void onTimeoutWeak(const boost::weak_ptr<Process>& wkPtr);
private:
    void execChild(Pipe& execError, int stdOutput, int stdError)
    __attribute__((__noreturn__));

    int afterFork(Pipe& execError, Pipe& stdOutput, Pipe& stdError);

private:
    ConstRunCommandRequestPtr request_;
    RunCommandResponsePtr response_;
    DoneCallback doneCallback_;

    pid_t pid_;
    std::string name_;
    std::string exe_file_;

    boost::posix_time::ptime startTime_;
    int64_t startTimeInJiffies_;
    cetty::channel::TimeoutPtr timerId_;

    //boost::shared_ptr<Sink> stdoutSink_;
    //boost::shared_ptr<Sink> stderrSink_;

    bool redirectStdout_;
    bool redirectStderr_;
    bool runCommand_;
};

}
}
}


#endif /* PROCESS_H_ */
