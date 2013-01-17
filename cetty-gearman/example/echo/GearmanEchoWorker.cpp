#include <cstdio>
#include <string>
#include <vector>

#include <cetty/buffer/ChannelBufferUtil.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/gearman/protocol/GearmanMessage.h>
#include <cetty/gearman/protocol/commands/Worker.h>
#include <cetty/gearman/builder/GearmanWorkerBuilder.h>

using namespace cetty::buffer;
using namespace cetty::gearman;
using namespace cetty::gearman::protocol;
using namespace cetty::gearman::builder;

GearmanMessagePtr echo(const GearmanMessagePtr& message) {
    LOG_DEBUG << "the result is " << ChannelBufferUtil::hexDump(message->data());

    GearmanMessagePtr msgRet = commands::createWorkCompleteMessage(
        message->parameters().front(),
        message->data());

    return msgRet;
}

int main(int argc, char* argv[]) {
    GearmanWorkerBuilder builder(1);
    builder.addConnection("192.168.1.162", 4730);
    builder.registerWorker("echo", &echo);
    builder.buildWorkers();

    getchar();
    return 0;
}
