#include <cetty/config/ConfigCenter.h>
#include <cetty/craft/builder/CraftServerBuilder.h>
#include "echo.pb.h"

using namespace cetty::config;
using namespace cetty::craft::builder;

namespace echo {

class EchoServiceImpl : public EchoService {
public:
    EchoServiceImpl() {}
    virtual ~EchoServiceImpl() {}

    virtual void echo(const ConstEchoRequestPtr& request,
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

    CraftServerBuilder builder;
    builder.registerService(new echo::EchoServiceImpl);
    builder.buildAll();

    builder.waitingForExit();

    return 0;
}
