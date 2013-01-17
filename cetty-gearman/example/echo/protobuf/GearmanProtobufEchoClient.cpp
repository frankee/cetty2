#include <cstdio>
#include <boost/bind.hpp>
#include <cetty/gearman/protobuf/builder/GearmanProtobufClientBuilder.h>
#include "echo.pb.h"


using namespace cetty::gearman::protobuf::builder;
using namespace cetty::channel;
using namespace cetty::bootstrap;
using namespace cetty::service;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::builder;

class EchoClient {
public:
    EchoClient(const ChannelPtr& s) : stub(s), count(0) {
        future = new echo::EchoServiceFuture();
        future->addListener(boost::bind(&EchoClient::replied, this, _1, _2));
    }
    virtual ~EchoClient() {}

    void sendRequest() {
        echo::EchoRequest* request = new echo::EchoRequest;
        request->set_payload("12345");

        stub.echo(request, future);
    }

    void replied(const echo::EchoServiceFuture& f, const echo::EchoResponsePtr& resp) {
        ++count;

        if (count < 10000) {
            sendRequest();
        }
    }

    echo::EchoService_Stub* getStub() { return &stub; }

private:
    echo::EchoServiceFuturePtr future;
    echo::EchoService_Stub stub;

    int count;
};

int main(int argc, char* argv[]) {
    GearmanProtobufClientBuilder builder(1);
    builder.addConnection("192.168.1.162", 4730);
    EchoClient client(builder.build());
    client.sendRequest();

    getchar();
    //service->getCloseFuture()->awaitUninterruptibly();
}
