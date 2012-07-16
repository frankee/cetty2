#include <cstdio>
#include <string>
#include <vector>

#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/gearman/GearmanMessage.h>
#include <cetty/gearman/builder/GearmanWorkerBuilder.h>

using namespace cetty::gearman;
using namespace cetty::gearman::builder;
//using namespace cetty::buffer;

GearmanMessagePtr echo(const GearmanMessagePtr& message) {
    std::string ret;
    ret = ChannelBuffers::hexDump(message->getData());
    std::cout<<"the ret is  "<< ret <<std::endl;

    GearmanMessagePtr msgRet = GearmanMessage::createWorkCompleteMessage(
        message->getParameters().front(),
        message->getData());
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

