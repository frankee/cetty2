
#include <stdio.h>
#include <cetty/gearman/GearmanMessage.h>
#include <cetty/gearman/builder/GearmanWorkerBuilder.h>

using namespace cetty::gearman;
using namespace cetty::gearman::builder;

GearmanMessagePtr echo(const GearmanMessagePtr& message) {
    return GearmanMessagePtr();
}

int main(int argc, char* argv[]) {
    GearmanWorkerBuilder builder;
    builder.addConnection("192.168.1.162", 4730);
    builder.registerWorker("test", &echo);
    builder.buildWorkers();
    return 0;
}
