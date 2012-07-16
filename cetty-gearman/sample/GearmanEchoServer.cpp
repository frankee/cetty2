#include <cstdio>
#include <cetty/gearman/GearmanMessage.h>
#include <cetty/gearman/builder/GearmanWorkerBuilder.h>

using namespace cetty::gearman;
using namespace cetty::gearman::builder;

GearmanMessagePtr echo(const GearmanMessagePtr& message) {
    return message;
}

int main(int argc, char* argv[]) {
    GearmanWorkerBuilder builder(1);
    builder.addConnection("192.168.1.162", 4730);
    builder.registerWorker("echo", &echo);
    builder.buildWorkers();

    getchar();
    return 0;
}

