/*
 * ProcessManager.h
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#ifndef PROCESSMANAGER_H_
#define PROCESSMANAGER_H_

#include <cetty/channel/EventLoopPtr.h>
#include <cetty/channel/EventLoop.h>
#include <cetty/zurg/slave/ZurgSlaveConfig.h>

#include <map>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/asio/signal_set.hpp>

struct rusage;

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::channel;

/**
 * @brief Manage processes belongs to this slave
 *    Register process exit call back, wait for process exiting
 *  and call call back.
 */
class ProcessManager : boost::noncopyable {
public:
    typedef boost::function<void (int status, const struct rusage&)> Callback;
    typedef boost::function<void ()> stopAllCallback;

public:
    ProcessManager(const EventLoopPtr& loop);
    ~ProcessManager();

    void start();

    // @brief register callback which will be called after process exited.
    void runAtExit(pid_t pid, const Callback&);

    void setStopAll(stopAllCallback stopAll){
    	stopAll_ = stopAll;
    }

    /*
     * @brief stop all child process and self
     *  Deal with SIGINT SIGTERM SIGQUIT
     */
    void stopAll(){ if(stopAll_) stopAll_();}

private:
    void onTimer();
    void startSignalWait();
    void handleSignalWait(const boost::system::error_code& error, int signal);
    void onExit(pid_t pid, int status, const struct rusage&);

private:
    ZurgSlaveConfig config_;
    boost::asio::signal_set signals_;
    EventLoopPtr loop_;
    std::map<pid_t, Callback> callbacks_;
    stopAllCallback stopAll_;
};

}
}
}


#endif /* PROCESSMANAGER_H_ */
