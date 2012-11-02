#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include <cetty/channel/EventLoopPtr.h>
#include <cetty/channel/EventLoop.h>
#include <string>

namespace cetty {
namespace zurg {
namespace slave {
class MasterService_Stub;
class ProcFs;

using namespace cetty::channel;

class Heartbeat : boost::noncopyable{
 public:
  Heartbeat(const EventLoopPtr &loop,
            MasterService_Stub* stub);
  ~Heartbeat();
  void start();
  void stop();

 private:
  void init();
  void onTimer();

  void beat(bool showStatic);

  EventLoopPtr loop_;
  const std::string name_;
  const int port_;
  MasterService_Stub* stub_;
  boost::scoped_ptr<ProcFs> procFs_;
  bool beating_;

  ZurgSlaveConfig config_;
};

}
}
}
#endif  // #ifndef HEARTBEAT_H_
