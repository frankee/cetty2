#ifndef MUDUO_PROTORPC_ZURG_CHILDMANAGER_H
#define MUDUO_PROTORPC_ZURG_CHILDMANAGER_H

#include <map>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/asio/signal_set.hpp>

#include <cetty/channel/EventLoopPtr.h>

struct rusage;

namespace cetty {
namespace zurg {

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
    int zombieInterval;
    const EventLoopPtr& loop;
    boost::asio::signal_set signals;

    std::multimap<pid_t, Callback> callbacks;
};

}
}

#endif  // MUDUO_PROTORPC_ZURG_CHILDMANAGER_H
