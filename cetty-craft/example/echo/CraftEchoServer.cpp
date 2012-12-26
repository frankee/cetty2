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

        for (int i = 0; i < 100; ++i) {
            rep->mutable_payload()->append(request->payload());
        }

        //boost::this_thread::sleep(boost::posix_time::microseconds(100));

        if (done) {
            done(rep);
        }
    }
};

}

int main(int argc, char* argv[]) {
    ConfigCenter::instance().load(argc, argv);

    CraftServerBuilder()
        .registerService(new echo::EchoServiceImpl)
        .buildAll()
        .waitingForExit();

    return 0;
}
