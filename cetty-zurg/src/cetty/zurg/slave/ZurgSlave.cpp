#include <cetty/protobuf/service/builder/ProtobufClientBuilder.h>
#include <cetty/protobuf/service/builder/ProtobufServerBuilder.h>
#include <cetty/zurg/slave/SlaveServiceImpl.h>
#include <cetty/zurg/slave/Heartbeat.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/zurg/master/master.pb.h>
#include <cetty/zurg/slave/ZurgSlave.h>
#include <cetty/config/ConfigCenter.h>

#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::service;
using namespace cetty::protobuf::service::builder;
using namespace cetty::zurg::slave;
using namespace cetty::zurg::master;
using namespace cetty::zurg;

ZurgSlave::ZurgSlave(){
    ConfigCenter::instance().configure(&config_);
    init();
}

void ZurgSlave::init(){
    if (config_.parentThreadCnt_ < 0)  config_.parentThreadCnt_ = 1;
    if (config_.childThreadCnt_ < 0) config_.childThreadCnt_ = 0;
    if (config_.masterAddress_.empty()){
        LOG_ERROR << "Master address is empty.";
        exit(0);
    }
    if(config_.masterPort_ <= 0){
        LOG_ERROR << "Master listen port is error.";
        exit(0);
    }

    if(config_.listenPort_ <= 0){
        LOG_ERROR << "Slave listen port is error.";
        exit(0);
    }
}

void ZurgSlave::start() {
    ProtobufServerBuilder serverBuilder(config_.parentThreadCnt_, config_.childThreadCnt_);
    EventLoopPtr loop = serverBuilder.getParentPool()->getNextLoop();
    ProtobufServicePtr service(new SlaveServiceImpl(loop));
    serverBuilder.registerService(service);

    ProtobufClientBuilder clientBuilder(serverBuilder.getChildPool());
    clientBuilder.addConnection(config_.masterAddress_, config_.masterPort_);

    // todo what's mean
    MasterService_Stub masterClient(clientBuilder.build());
    Heartbeat hb(loop, &masterClient);
    hb.start();

    serverBuilder.buildRpc(config_.listenPort_);
    serverBuilder.waitingForExit();

    LOG_INFO << "Start zurg_slave";
}

}
}
}

