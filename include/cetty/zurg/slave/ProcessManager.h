/*
 * ProcessManager.h
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#ifndef PROCESSMANAGER_H_
#define PROCESSMANAGER_H_

#include <cetty/channel/EventLoopPtr.h>

#include <map>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/asio/signal_set.hpp>

struct rusage;

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::channel;

class ProcessManager : boost::noncopyable {
public:
    typedef boost::function<void (int status, const struct rusage&)> Callback;

public:
    ProcessManager(const EventLoopPtr& loop, int zombieInterval);
    ~ProcessManager();

    void start();
    void runAtExit(pid_t pid, const Callback&);

private:
    void startSignalWait();
    void handleSignalWait(const boost::system::error_code& error, int signal);
    void onTimer();
    void onExit(pid_t pid, int status, const struct rusage&);

private:
    int zombieInterval_;
    EventLoopPtr loop_;
    boost::asio::signal_set signals_;
    std::multimap<pid_t, Callback> callbacks_;
};

}
}
}


#endif /* PROCESSMANAGER_H_ */
