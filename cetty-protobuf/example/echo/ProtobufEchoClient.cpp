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
    EchoClient(const ChannelPtr& s) : stub_(s), count_(0) {
        future_ = new echo::EchoServiceFuture();
        future_->addListener(boost::bind(&EchoClient::replied, this, _1, _2));

        buffer_.resize(1024*1024);
    }
    virtual ~EchoClient() {}

    void sendRequest() {
        echo::EchoRequest* request = new echo::EchoRequest;
        request->set_payload(buffer_);

        stub_.echo(request, future_);
    }

    void replied(const echo::EchoServiceFuture& f, const echo::EchoResponsePtr& resp) {
        ++count_;
        if (count_ < 10000) {
            sendRequest();
        }
    }

	echo::EchoService_Stub* getStub() { return &stub_; }

private:
    std::string buffer_;
    echo::EchoServiceFuturePtr future_;
    echo::EchoService_Stub stub_;

    int count_;
};

int main(int argc, char* argv[]) {
    ProtobufClientBuilder builder(1);
    builder.addConnection("127.0.0.1", 1980);

    ChannelPtr service = builder.build();
    EchoClient client(service);
    client.sendRequest();


    printf("To quit server, press 'q'.\n");

    char input;
    do {
        input = getchar();

        if (input == 'q') {
            //builder.
            break;
        }
    }
    while (true);

    //service->closeFuture()->awaitUninterruptibly();
}
