#include <cetty/zurg/slave/ProcessManager.h>

#include <assert.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>

#include <boost/bind.hpp>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/channel/asio/AsioService.h>
#include <cetty/zurg/slave/SlaveServiceConfig.cnf.h>

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::channel;
using namespace cetty::channel::asio;
using namespace cetty::config;

ProcessManager::ProcessManager(const EventLoopPtr& loop)
    : signals_(boost::dynamic_pointer_cast<AsioService>(loop)->service()),
      loop_(loop) {
    ConfigCenter::instance().configure(&config_);

    signals_.add(SIGCHLD);
    signals_.add(SIGTERM);
    signals_.add(SIGQUIT);
    signals_.add(SIGINT);

    startSignalWait();

    //loop_->runEvery(config_.zombieInterval_, boost::bind(&ProcessManager::onTimer, this));
}

ProcessManager::~ProcessManager() {
    boost::system::error_code code;
    signals_.clear(code);

    if (code) {
        LOG_ERROR << "stop and clear signal_set service has error: "
                  << code.value();
    }
}

void ProcessManager::start() {

}

void ProcessManager::runAtExit(pid_t pid, const Callback& cb) {
    assert(callbacks_.find(pid) == callbacks_.end());
    callbacks_.insert(std::pair<pid_t, Callback>(pid, cb));
}

void ProcessManager::startSignalWait() {
    signals_.async_wait(boost::bind(&ProcessManager::handleSignalWait,
                                    this,
                                    _1,
                                    _2));
}

void ProcessManager::handleSignalWait(const boost::system::error_code& error, int signal) {
    LOG_INFO << "Receive a signal. [" << signal << "]";

    if (!error) {
        if (signal == SIGINT || signal == SIGTERM || signal == SIGQUIT) {
            LOG_INFO << "Received signal "
                     << signal
                     << " and terminating all process.";

            stopAll();
        }
        else if (signal == SIGCHLD) {
            int status = 0;
            struct rusage resourceUsage;
            bzero(&resourceUsage, sizeof(resourceUsage));

            pid_t pid = ::wait4(-1, &status, WNOHANG, &resourceUsage);

            if (pid > 0) {
                onExit(pid, status, resourceUsage);
            }
            else {
                LOG_FATAL << "ProcessManager::onChildProcessExit - wait4 ";
            }

            startSignalWait();
        }
        else {
            LOG_ERROR << "Unexceptional signal :" << signal;
        }
    }
    else {
        LOG_WARN << "Waiting the SIGCHLD signal has an error : "
                 << error.value()
                 << " : " << error.message();
    }
}

void ProcessManager::onTimer() {
    int status = 0;
    struct rusage resourceUsage;
    bzero(&resourceUsage, sizeof(resourceUsage));
    pid_t pid = 0;

    while ((pid = ::wait4(-1, &status, WNOHANG, &resourceUsage)) > 0) {
        LOG_INFO << "ChildManager::onTimer - child process " << pid << " exited.";
        onExit(pid, status, resourceUsage);
    }
}

void ProcessManager::onExit(pid_t pid, int status, const struct rusage& resourceUsage) {
    LOG_INFO << "Process [" << pid << "] has exited";
    std::map<pid_t, Callback>::iterator it = callbacks_.find(pid);

    if (it != callbacks_.end()) {
        loop_->post(boost::bind(it->second,
                                status,
                                resourceUsage));

        callbacks_.erase(it);
    }
    else {
        LOG_ERROR << "No callback is found for process [" << pid << "].";
    }
}

}
}
}
