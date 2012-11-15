#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include <cetty/config/ConfigCenter.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/craft/builder/CraftServerBuilder.h>
#include <cetty/channel/EventLoopPoolPtr.h>
#include <cetty/channel/EventLoopPool.h>

#include <cetty/zurg/master/master.pb.h>
#include <cetty/zurg/slave/Heartbeat.h>
#include <cetty/zurg/slave/ZurgSlave.h>
#include <cetty/zurg/slave/SlaveServiceImpl.h>

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::service;
using namespace cetty::zurg::master;

ZurgSlave::ZurgSlave(){
    ConfigCenter::instance().configure(&config_);
    init();
}

void ZurgSlave::init(){
    if (config_.masterAddress.empty()){
        LOG_WARN << "Master address is empty.";
        config_.masterAddress ="127.0.0.1";
    }
    if(config_.masterPort <= 0){
        LOG_ERROR << "Master listen port is error.";
        config_.masterPort = 6636;
    }

    if(config_.listenPort <= 0){
        LOG_ERROR << "Slave listen port is error.";
        config_.listenPort = 6637;
    }

   if(config_.prefix.empty()){
       config_.prefix = "/tmp";
       LOG_WARN << "Zurg slave profix is not set, set to "
                << config_.prefix;
   }

   if(config_.name.empty()){
       LOG_WARN << "Zurg slave name is empty";
   }
}

void ZurgSlave::start() {
	/*
    ProtobufServerBuilder serverBuilder(1, 0);
    EventLoopPtr loop = serverBuilder.getParentPool()->getNextLoop();
    ProtobufServicePtr service(new SlaveServiceImpl(loop));
    serverBuilder.registerService(service);

    ProtobufClientBuilder clientBuilder(serverBuilder.getChildPool());
    clientBuilder.addConnection(config_.masterAddress_, config_.masterPort_);

    MasterService_Stub masterClient(clientBuilder.build());
    Heartbeat hb(loop, &masterClient);
    hb.start();

    serverBuilder.buildRpc(config_.listenPort_);
    serverBuilder.waitingForExit();

    LOG_INFO << "Start zurg_slave";
    */
}

}
}
}


using namespace cetty::zurg;
using namespace cetty::zurg::slave;
using namespace cetty::craft::builder;

int main(int argc, char* argv[]) {
    ConfigCenter::instance().load(argc, argv);

    CraftServerBuilder builder;
    builder.registerService(
        new SlaveServiceImpl(builder.getParentPool()->getNextLoop()));

    //builder.buildHttp(8080);
    builder.buildAll();
    builder.waitingForExit();

    return 0;
}

