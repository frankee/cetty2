#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_

#include <cetty/channel/EventLoopPtr.h>
#include <cetty/channel/EventLoop.h>
#include <cetty/zurg/slave/ZurgSlaveConfig.h>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>

namespace cetty {
namespace zurg {
namespace master {
class MasterService_Stub;
}
}
}

namespace cetty {
namespace zurg {
namespace slave {
class ProcFs;

using namespace cetty::zurg::master;
using namespace cetty::channel;

class Heartbeat : boost::noncopyable {
public:
    Heartbeat(const EventLoopPtr& loop,
              MasterService_Stub* stub);
    ~Heartbeat();
    void start();
    void stop();

private:
    void init();
    void onTimer();

    void beat(bool showStatic);

private:
    int heartbeatInterval_;
    EventLoopPtr loop_;
    std::string name_;
    int port_;
    MasterService_Stub* stub_;
    boost::scoped_ptr<ProcFs> procFs_;
    bool beating_;
};

}
}
}
#endif  // #ifndef HEARTBEAT_H_
