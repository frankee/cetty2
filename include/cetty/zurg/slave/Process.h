/*
 * Process.h
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <cetty/channel/Timeout.h>
#include <cetty/channel/EventLoopPtr.h>

#include <cetty/zurg/slave/slave.pb.h>
#include <cetty/zurg/slave/slave_service.pb.h>
#include <cetty/protobuf/service/ProtobufService.h>

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

/**
 * @brief start a new process
 *  We can start a new process by calling start, and
 *  get child process's run state.
 */
class Process : public boost::enable_shared_from_this<Process>, boost::noncopyable {
public:
    Process(const ConstRunCommandRequestPtr& request,
            const RunCommandResponsePtr& response,
            const DoneCallback& done,
            bool redirectStdout,
            bool redirectStderr,
            const std::string& slaveName,
            const std::string& slavePrefix);

    Process(const AddApplicationRequestPtr& request,
            const std::string& slaveName,
            const std::string& slavePrefix);

    ~Process();

    // start a new process
    int start();

    pid_t pid() const { return childPid_; }
    const std::string& name() const { return name_; }

    void setTimerId(const cetty::channel::TimeoutPtr& timerId) {
        timerId_ = timerId;
    }

    void onCommandExit(int status, const struct ::rusage&);
    void onTimeout();

    static void onTimeoutWeak(const boost::weak_ptr<Process>& wkPtr);

private:
    /*
     * @brief launch new process image
     * @param execError A pipe used for communicating with parent process
     *        Just use writing point.
     * @param stdOutput Redirect standard output of child process to #stdOutput
     * @param stdError Redirect standard error of child process to #stdError
     */
    void execChild(Pipe& execError, int stdOutput, int stdError)
    __attribute__((__noreturn__));

    // get new process's run information
    int afterFork(Pipe& execError, Pipe& stdOutput, Pipe& stdError);

    std::string getCommandOutput();
    std::string getCommandError();

private:
    static const std::string STDOUT_PREFIX_;
    static const std::string STDERR_PREFIX_;
    static const int CHILD_SLEEP_SECS_;

private:
    ConstRunCommandRequestPtr request_;
    RunCommandResponsePtr response_;
    DoneCallback doneCallback_;

    pid_t childPid_;
    std::string name_;
    std::string exeFile_;

    boost::posix_time::ptime startTime_;
    int64_t startTimeInJiffies_;
    cetty::channel::TimeoutPtr timerId_;

    bool redirectStdout_;
    bool redirectStderr_;
    std::string slaveName_;
    std::string slavePrefix_;

    bool runCommand_;
};

}
}
}


#endif /* PROCESS_H_ */
