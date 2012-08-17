#include <cetty/protobuf/service/builder/ProtobufClientBuilder.h>
#include <cetty/protobuf/service/builder/ProtobufServerBuilder.h>
#include <cetty/zurg/slave/SlaveServiceImpl.h>
#include <cetty/zurg/slave/Heartbeat.h>

#include <cetty/zurg/proto/master.pb.h>


#include <cetty/logging/LoggerHelper.h>

#include <assert.h>

using namespace cetty::service;
using namespace cetty::protobuf::service::builder;
using namespace cetty::zurg::slave;
using namespace cetty::zurg;

int main(int argc, char* argv[])
{
    if (ProcessInfo::uid() == 0 || ProcessInfo::euid() == 0)
    {
        fprintf(stderr, "Cannot run as root.\n");
        return 1;
    }

    // we can't setrlimit as it will be inherited by child processes.

    //google::protobuf::SetLogHandler(zurgLogHandler);
    //SlaveConfig config(parseCommandLine(argc, argv));

    ProtobufServerBuilder serverBuilder(1, 0);
    EventLoopPtr loop = serverBuilder.getParentPool()->getNextLoop();

    ProtobufClientBuilder clientBuilder(loop);
    clientBuilder.addConnection("192.168.1.1", 1900);

    MasterService_Stub masterClient(clientBuilder.build());
    //Heartbeat heartbeat();

    ProtobufServicePtr service(new SlaveServiceImpl(loop, 4));
    serverBuilder.registerService(service);

    serverBuilder.buildRpc(1098);

    serverBuilder.waitingForExit();

    LOG_INFO << "Start zurg_slave";

    //std::string cwd = prefix()+'/'+name();
    //setupWorkingDir(cwd);

    if (config.valid())
    {
        return 0;
    }
    else
    {
        printf("Usage\n%s [-d listen_port] -n instance_name master_ip:master_port\n", argv[0]);
        return 1;
    }
}
