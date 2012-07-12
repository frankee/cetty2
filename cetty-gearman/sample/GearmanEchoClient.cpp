#include <stdio.h>

#include <boost/bind.hpp>
#include <cetty/service/ClientService.h>
#include <cetty/gearman/builder/GearmanClientBuilder.h>
#include <cetty/gearman/GearmanMessage.h>

using namespace cetty::channel;
using namespace cetty::gearman;
using namespace cetty::bootstrap;
using namespace cetty::service;
using namespace cetty::gearman::builder;

class GearmanEchoClient {
public:
    GearmanEchoClient(const ClientServicePtr& s): service(s),count(0) {

    }
    virtual ~GearmanEchoClient() {}

    void sendRequest() {
        ChannelBufferPtr buf = ChannelBuffers::buffer(10);
        buf->writeBytes("hello");

        GearmanMessagePtr request(GearmanMessage::createsubmitJobMessage("test","1234",buf));
        service->write(ChannelMessage(request));
    }

    //void replied(const echo::EchoServiceFuture& f, const echo::EchoResponsePtr& resp) {
    //    ++count;
    //    if (count < 10000) {
    //        sendRequest();
    //    }
    //}
private:
    int count;
    ClientServicePtr service;
};

int main(int argc, char* argv[]) {
    GearmanClientBuilder builder;
    builder.addConnection("127.0.0.1", 4730);

    ClientServicePtr service = builder.build();
    GearmanEchoClient client(service);
    client.sendRequest();
    service->getCloseFuture()->awaitUninterruptibly();
}
