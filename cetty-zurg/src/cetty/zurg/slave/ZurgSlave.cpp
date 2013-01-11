#include <cetty/config/ConfigCenter.h>
#include <cetty/craft/builder/CraftServerBuilder.h>

#include <cetty/zurg/slave/SlaveServiceImpl.h>

using namespace cetty::config;
using namespace cetty::zurg::slave;
using namespace cetty::craft::builder;

int main(int argc, char* argv[]) {
    ConfigCenter::instance().load(argc, argv);

    CraftServerBuilder builder;
    builder.registerService(
        new SlaveServiceImpl(builder.parentPool()->getNextLoop()));

    builder.buildAll().waitingForExit();
    return 0;
}
