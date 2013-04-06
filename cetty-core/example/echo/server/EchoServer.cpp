// echo.cpp : Defines the entry point for the console application.
//

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include <cetty/channel/InetAddress.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelInitializer.h>

#include <cetty/bootstrap/ServerBootstrap.h>

#include <cetty/logging/LogLevel.h>
#include <cetty/logging/Logger.h>

#include "EchoServerHandler.h"

using namespace cetty::channel;

using namespace cetty::bootstrap;
using namespace cetty::buffer;

using namespace cetty::util;
using namespace cetty::logging;

int main(int argc, char* argv[]) {
    int threadCount = 1;

    if (argc == 2) {
        threadCount = atoi(argv[1]);
    }

    Logger::logLevel(LogLevel::DEBUG);
    ChannelInitializer1<EchoServerHandler> initializer("echo");

    ServerBootstrap bootstrap(threadCount);

    bootstrap.setChildInitializer(boost::bind<bool>(initializer, _1))
        .setChannelOption(ChannelOption::CO_TCP_NODELAY, true)
        .setChannelOption(ChannelOption::CO_SO_REUSEADDR, true)
        .setChannelOption(ChannelOption::CO_SO_BACKLOG, 4096)
        .bind(1980)->await();

    bootstrap.waitingForExit();
    return 0;
}
