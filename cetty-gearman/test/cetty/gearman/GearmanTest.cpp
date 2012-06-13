// echo.cpp : Defines the entry point for the console application.
//

#include <cetty/bootstrap/ClientBootstrap.h>
#include <cetty/channel/socket/asio/AsioClientSocketChannelFactory.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/buffer/ChannelBuffers.h>

#include <cetty/gearman/GearmanPipelineFactory.h>
#include <cetty/gearman/GearmanTask.h>

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::channel::socket::asio;

using namespace cetty::bootstrap;

using namespace cetty::util;
using namespace cetty::gearman;

int main(int argc, char* argv[]) {
    // Print usage if no argument is specified.

    // Parse options.
    std::string host = "192.168.2.145";
    int port = 4730;
    int ioThreadCount = 1;

    // Configure the client.
    ClientBootstrap bootstrap(new AsioClientSocketChannelFactory(ioThreadCount));

    bootstrap.setPipelineFactory(new GearmanPipelineFactory);

    // Start the connection attempt.
    std::vector<Channel*> clientChannels;

    ChannelFuturePtr future = bootstrap.connect(host, port);
    future->awaitUninterruptibly();

    ChannelPtr c = future->getChannel();
    GearmanTaskPtr task(new GearmanTask);
    
    ChannelBufferPtr buffer = ChannelBuffers::buffer(1024, 64);
    for (int i = 0; i < 1024; i += 4) {
        buffer->writeInt(4);
    }
    task->request = GearmanMessage::createEchoReqMessage(buffer);
    c->write(ChannelMessage(task));


    system("PAUSE");

    c->getCloseFuture()->awaitUninterruptibly();

    // Shut down thread pools to exit.
    bootstrap.releaseExternalResources();

    return 0;
}

