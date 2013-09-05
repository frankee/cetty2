#include <cetty/config/ConfigCenter.h>
#include <cetty/protobuf/service/builder/ProtobufServerBuilder.h>
#include "echo.pb.h"

#include <cetty/channel/EventLoop.h>
#include <cetty/channel/EventLoopPool.h>

using namespace cetty::channel;


using namespace cetty::config;
using namespace cetty::protobuf::service::builder;

namespace echo {

class EchoServiceImpl : public EchoService {
public:
    EchoServiceImpl() {}
    virtual ~EchoServiceImpl() {}

    virtual void echo(const ConstEchoRequestPtr& request,
                      const EchoResponsePtr& response,
                      const DoneCallback& done) {
        response->set_payload(request->payload());
        if (done) {
            done(response);
        }
    }
};

}

int main(int argc, char* argv[]) {
    ConfigCenter::instance().load(argc, argv);
    
    ProtobufServerBuilder builder;
    builder.registerService(new echo::EchoServiceImpl).build(1980);
    builder.waitingForExit();
    return 0;
}
