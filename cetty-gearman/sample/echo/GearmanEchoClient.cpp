#include <stdio.h>

#include <boost/bind.hpp>
#include <cetty/buffer/Unpooled.h>
#include <cetty/service/ClientService.h>
#include <cetty/gearman/builder/GearmanClientBuilder.h>
#include <cetty/gearman/protocol/GearmanMessage.h>

using namespace cetty::buffer;
using namespace cetty::channel;
using namespace cetty::gearman;
using namespace cetty::bootstrap;
using namespace cetty::service;
using namespace cetty::gearman::builder;
using namespace cetty::gearman::protocol;

class GearmanEchoClient {
private:
    typedef ::cetty::service::ServiceFuture<GearmanMessagePtr> GearmanServiceFuture;
    typedef boost::intrusive_ptr<GearmanServiceFuture> GearmanServiceFuturePtr;

public:
    GearmanEchoClient(const ClientServicePtr& s): service(s),count(0) {
        future = new GearmanServiceFuture();
        future->addListener(boost::bind(&GearmanEchoClient::replied, this, _1, _2));
    }
    virtual ~GearmanEchoClient() {}

    void sendRequest() {
        ChannelBufferPtr buf = Unpooled::buffer(10);
        buf->writeBytes("hello");

        callMethod(service,GearmanMessage::createsubmitJobMessage("echo","1234",buf),future);
    }

    void replied(const GearmanServiceFuture& f, const GearmanMessagePtr& resp) {
        std::string ret;
        resp->data()->readBytes(&ret);
        std::cout<<"the msg is  "<<ret<<std::endl;
        ++count;
        if (count < 10000) {
            sendRequest();
        }
    }

private:
    int count;
    ClientServicePtr service;
    GearmanServiceFuturePtr future;
};

int main(int argc, char* argv[]) {
    GearmanClientBuilder builder;
    builder.addConnection("192.168.1.162", 4730);

    ClientServicePtr service = builder.build();
    GearmanEchoClient client(service);
    client.sendRequest();
    service->closeFuture()->awaitUninterruptibly();
}
