
#include "echo.pb.h"
#include "cetty/bootstrap/ServerBootstrap.h"

#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioServerSocketChannelFactory.h>

#include <cetty/handler/codec/frame/LengthFieldBasedFrameDecoder.h>
#include <cetty/handler/codec/frame/LengthFieldPrepender.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageHandler.h>

#include <cetty/protobuf/service/ProtobufServiceRegister.h>

using namespace cetty::util;
using namespace cetty::bootstrap;
using namespace cetty::channel;
using namespace cetty::channel::socket::asio;
using namespace cetty::handler::codec::frame;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::handler;

namespace echo {

class EchoServiceImpl : public EchoService {
public:
    virtual void Echo(const ConstEchoRequestPtr& request,
                      const EchoResponsePtr& response,
                      const DoneCallback& done) {
        EchoResponsePtr rep(response);
        if (!response) {
            rep = new EchoResponse;
        }

        rep->set_payload(request->payload());
        if (done) {
            done(static_pointer_cast<google::protobuf::Message>(rep));
        }
    }
};

}

class EchoChannelPipelineFactory : public cetty::channel::ChannelPipelineFactory {
public:
    EchoChannelPipelineFactory() {}
    virtual ~EchoChannelPipelineFactory() {}

    virtual ChannelPipelinePtr getPipeline() {
        ChannelPipelinePtr pipeline = Channels::pipeline();

        pipeline->addLast("frameDecoder", new LengthFieldBasedFrameDecoder(16 * 1024 * 1024, 0, 4, 0, 4));
        pipeline->addLast("frameEncoder", new LengthFieldPrepender(4));

        pipeline->addLast("protobufDecoder", new ProtobufServiceMessageDecoder());
        pipeline->addLast("protobufEncoder", new ProtobufServiceMessageEncoder());

        pipeline->addLast("messageHandler", new ProtobufServiceMessageHandler());

        return pipeline;
    }
};

int main(int argc, char* argv[]) {
    //ConfigureCenter::instance().load();
    ProtobufServiceRegister::instance().registerService(ProtobufServicePtr(new echo::EchoServiceImpl));

    int threadCount = 1;

    if (argc == 2) {
        threadCount = atoi(argv[1]);
    }

    ChannelFactoryPtr factory = new AsioServerSocketChannelFactory(
        new AsioServicePool(threadCount));
    ServerBootstrap bootstrap(factory);

    bootstrap.setOption("child.tcpNoDelay", boost::any(true));
    bootstrap.setOption("reuseAddress", boost::any(true));
    bootstrap.setOption("backlog", boost::any(4096));
    bootstrap.setPipelineFactory(ChannelPipelineFactoryPtr(new EchoChannelPipelineFactory));

    // Bind and start to accept incoming connections.
    ChannelPtr c = bootstrap.bind(1980);

    if (c && c->isBound()) {
        printf("Server is running...\n");
        printf("To quit server, press 'q'.\n");

        char input;

        do {
            input = getchar();

            if (input == 'q') {
                c->close()->awaitUninterruptibly();
                bootstrap.releaseExternalResources();
                return 0;
            }
        }
        while (true);
    }

    bootstrap.releaseExternalResources();
    return -1;

}

