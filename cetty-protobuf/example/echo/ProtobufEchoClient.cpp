#include <stdio.h>
#include <boost/bind.hpp>
#include <cetty/protobuf/service/builder/ProtobufClientBuilder.h>
#include "echo.pb.h"

using namespace cetty::channel;
using namespace cetty::bootstrap;
using namespace cetty::service;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::builder;

class EchoClient {
public:
    EchoClient(const ChannelPtr& s) : stub(s), count(0) {
        future = new echo::echoServiceFuture();
        future->addListener(boost::bind(&EchoClient::replied, this, _1, _2));
    }
    virtual ~EchoClient() {}

    void sendRequest() {
        echo::EchoRequest* request = new echo::EchoRequest;
        request->set_payload("0123456789ABCDEF");

        stub.echo(request, future);
    }

    void replied(const echo::echoServiceFuture& f, const echo::EchoResponsePtr& resp) {
        ++count;
        if (count < 10000) {
            sendRequest();
        }
    }

	echo::EchoService_Stub* getStub() { return &stub; }

private:
    echo::echoServiceFuturePtr future;
    echo::EchoService_Stub stub;

    int count;
};

int main(int argc, char* argv[]) {
    ProtobufClientBuilder builder(1);
    builder.addConnection("127.0.0.1", 1980);

    ChannelPtr service = builder.build();
    EchoClient client(service);
    client.sendRequest();

    service->closeFuture()->awaitUninterruptibly();
}
