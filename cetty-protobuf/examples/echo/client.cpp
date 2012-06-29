#include "echo.pb.h"


#include <stdio.h>
#include <boost/bind.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioClientSocketChannelFactory.h>
#include <cetty/bootstrap/ClientBootstrap.h>

#include <cetty/handler/codec/frame/LengthFieldBasedFrameDecoder.h>
#include <cetty/handler/codec/frame/LengthFieldPrepender.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageHandler.h>
#include <cetty/protobuf/service/handler/ProtobufServiceRequestHandler.h>

#include <cetty/protobuf/service/ProtobufClientService.h>


using namespace cetty::channel;
using namespace cetty::bootstrap;
using namespace cetty::channel::socket::asio;
using namespace cetty::handler::codec::frame;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::handler;

class EchoClient {
public:
    typedef cetty::service::ServiceFuture<cetty::util::BarePointer<echo::EchoResponse> > ResponseFuture;
public:
    EchoClient(const ProtobufClientServicePtr& s) : stub(s), count(0) {
        future = new cetty::service::ServiceFuture<cetty::util::BarePointer<echo::EchoResponse> >();
        future->addListener(boost::bind(&EchoClient::replied, this, _1, _2));
    }
    virtual ~EchoClient() {}

    void sendRequest() {
        echo::EchoRequest* request = new echo::EchoRequest;
        request->set_payload("0123456789ABCDEF");

        stub.Echo(request, future);
    }

    void replied(const ResponseFuture& f, const echo::EchoResponsePtr& resp) {
        ++count;
        if (count < 10000) {
            sendRequest();
        }
    }

private:
    echo::EchoService_Stub::EchoResponseFuturePtr future;
    echo::EchoService_Stub stub;

    int count;
};

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

        //pipeline->addLast("messageHandler", new ProtobufServiceMessageHandler());
        pipeline->addLast("messageHandler", new ProtobufServiceRequestHandler());
        return pipeline;
    }
};


int main(int argc, char* argv[]) {
    // Configure the client.
    ClientBootstrap bootstrap(new AsioClientSocketChannelFactory(1));

	bootstrap.setPipelineFactory(new EchoChannelPipelineFactory);
    // Set up the pipeline factory.

    // Start the connection attempt.
    ChannelFuturePtr future = bootstrap.connect("127.0.0.1", 1980);

    future->awaitUninterruptibly();
    ChannelPtr c = future->getChannel();



    ProtobufClientServicePtr service(new ProtobufClientService(c));


    EchoClient client(service);
    client.sendRequest();

    // Wait until the connection is closed or the connection attempt fails.
    
    c->getCloseFuture()->awaitUninterruptibly();
}

