#include <stdio.h>

#include <boost/bind.hpp>
#include <cetty/buffer/Unpooled.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/service/ClientService.h>
#include <cetty/gearman/builder/GearmanClientBuilder.h>
#include <cetty/gearman/GearmanClient.h>
#include <cetty/gearman/protocol/GearmanMessage.h>
#include <cetty/gearman/protocol/commands/Client.h>

using namespace cetty::buffer;
using namespace cetty::channel;
using namespace cetty::bootstrap;
using namespace cetty::service;
using namespace cetty::gearman;
using namespace cetty::gearman::builder;
using namespace cetty::gearman::protocol;

class GearmanEchoClient {
private:
    typedef ::cetty::service::ServiceFuture<GearmanMessagePtr> GearmanServiceFuture;
    typedef boost::intrusive_ptr<GearmanServiceFuture> GearmanServiceFuturePtr;

public:
    GearmanEchoClient(const ChannelPtr& s)
        : count(0),
          service(s) {
        future = new GearmanServiceFuture();
        future->addListener(boost::bind(&GearmanEchoClient::replied,
                                        this,
                                        _1,
                                        _2));
    }

    virtual ~GearmanEchoClient() {}

    void sendRequest() {
        ChannelBufferPtr buf = Unpooled::buffer(10);
        buf->writeBytes("hello");

        callMethod(service,
                   commands::submitJobMessage("echo", "1234", buf),
                   future);
    }

    void replied(const GearmanServiceFuture& f, const GearmanMessagePtr& resp) {
        std::string ret;
        resp->data()->readBytes(&ret);
        LOG_INFO << "the msg is  " << ret;
        ++count;

        if (count < 10000) {
            sendRequest();
        }
    }

private:
    int count;
    ChannelPtr service;
    GearmanServiceFuturePtr future;
};

int main(int argc, char* argv[]) {
    GearmanClientBuilder builder;
    builder.addConnection("192.168.1.108", 4730);

    GearmanClientPtr gearman = builder.build();

    GearmanEchoClient client(gearman->channel());
    client.sendRequest();

    getchar();
    //service->closeFuture()->awaitUninterruptibly();
}
