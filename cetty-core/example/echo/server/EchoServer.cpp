// echo.cpp : Defines the entry point for the console application.
//

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include "EchoServerHandler.h"

#include "cetty/bootstrap/ServerBootstrap.h"

#include "cetty/channel/ChannelPipelines.h"
#include "cetty/channel/IpAddress.h"
#include "cetty/channel/SocketAddress.h"
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include "cetty/channel/socket/asio/AsioServerSocketChannelFactory.h"

using namespace cetty::channel;
using namespace cetty::channel::socket::asio;

using namespace cetty::bootstrap;
using namespace cetty::buffer;

using namespace cetty::util;

int main(int argc, char* argv[]) {
    int threadCount = 1;

    if (argc == 2) {
        threadCount = atoi(argv[1]);
    }

    ChannelFactoryPtr factory = new AsioServerSocketChannelFactory(threadCount);

    ServerBootstrap bootstrap(factory);

    bootstrap.setPipeline(ChannelPipelines::pipeline(new EchoServerHandler))
    .setOption(ChannelOption::CO_TCP_NODELAY, true)
    .setOption(ChannelOption::CO_SO_REUSEADDR, true)
    .setOption(ChannelOption::CO_SO_BACKLOG, 4096);

    // Bind and start to accept incoming connections.
    ChannelFuturePtr f = bootstrap.bind(1980)->await();

    printf("Server is running...\n");
    printf("To quit server, press 'q'.\n");

    char input;

    do {
        input = getchar();

        if (input == 'q') {
            f->getChannel()->getCloseFuture()->awaitUninterruptibly();
            return 0;
        }
    }
    while (true);

    return -1;
}
