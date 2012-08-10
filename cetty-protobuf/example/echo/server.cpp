#include <cetty/config/ConfigCenter.h>
#include <cetty/protobuf/service/builder/ProtobufServerBuilder.h>
#include "echo.pb.h"


using namespace cetty::config;
using namespace cetty::protobuf::service::builder;

namespace echo {

class EchoServiceImpl : public EchoService {
public:
    EchoServiceImpl() {}
    virtual ~EchoServiceImpl() {}

    virtual void Echo(const ConstEchoRequestPtr& request,
                      const EchoResponsePtr& response,
                      const DoneCallback& done) {
        EchoResponsePtr rep(response);
        if (!response) {
            rep = new EchoResponse;
        }

        rep->set_payload(request->payload());
        if (done) {
            done(rep);
        }
    }
};

}

int main(int argc, char* argv[]) {
    ConfigCenter::instance().load(argc, argv);
    
    ProtobufServerBuilder builder;
    builder.registerService(new echo::EchoServiceImpl);
    builder.buildRpc(1980);
    builder.buildHttp(8080);

    //builder.buildAll();
    builder.waitingForExit();

    return 0;
}
