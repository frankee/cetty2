#include <stdio.h>
#include <boost/bind.hpp>
#include <cetty/protobuf/service/ProtobufClientService.h>
#include <cetty/protobuf/service/builder/ProtobufClientBuilder.h>
#include "echo.pb.h"

using namespace cetty::channel;
using namespace cetty::bootstrap;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::builder;

class EchoClient {
public:
    EchoClient(const ProtobufClientServicePtr& s) : stub(s), count(0) {
        future = new echo::EchoServiceFuture();
        future->addListener(boost::bind(&EchoClient::replied, this, _1, _2));
    }
    virtual ~EchoClient() {}

    void sendRequest() {
        echo::EchoRequest* request = new echo::EchoRequest;
        request->set_payload("0123456789ABCDEF");

        stub.Echo(request, future);
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
    ProtobufClientBuilder builder(1);
    builder.addConnection("127.0.0.1", 1980);

    ProtobufClientServicePtr service = builder.build();

    EchoClient client(service);
    client.sendRequest();

    service->getCloseFuture()->awaitUninterruptibly();
}

