// echo.cpp : Defines the entry point for the console application.
//
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/smart_ptr/enable_shared_from_this2.hpp>

#include <cetty/bootstrap/asio/AsioClientBootstrap.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelInitializer.h>
#include <cetty/channel/IpAddress.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelFuture.h>

#include "EchoClientHandler.h"

using namespace cetty::channel;

using namespace cetty::bootstrap::asio;
using namespace cetty::buffer;

using namespace cetty::util;

int main(int argc, char* argv[]) {
    // Print usage if no argument is specified.
    if (argc < 3 || argc > 6) {
        printf(
            "Usage: EchoClient \n <host> <port> [<first message size> <client count> <io thread count>]");
        return -1;
    }

    // Parse options.
    std::string host = argv[1];
    int port = atoi(argv[2]);
    int firstMessageSize = 256;

    if (argc >= 4) {
        firstMessageSize = atoi(argv[3]);
    }

    int clientCount = 1;

    if (argc >= 5) {
        clientCount = atoi(argv[4]);
    }

    int ioThreadCount = 1;

    if (argc >= 6) {
        ioThreadCount = atoi(argv[5]);
    }

    // Configure the client.
    AsioClientBootstrap bootstrap(ioThreadCount);
    ChannelInitializer1<EchoClientHandler> initializer;

    // Set up the pipeline factory.
    bootstrap.setChannelInitializer(boost::bind<bool>(initializer, _1, firstMessageSize));

    // Start the connection attempt.
    std::vector<ChannelPtr> clientChannels;

    for (int i = 0; i < clientCount; ++i) {
        ChannelFuturePtr future = bootstrap.connect(host, port);
        future->awaitUninterruptibly();
        //BOOST_ASSERT(future->awaitUninterruptibly()->isSuccess());

        clientChannels.push_back(future->channel());
    }

    // Wait until the connection is closed or the connection attempt fails.
    for (size_t i = 0; i < clientChannels.size(); ++i) {
        clientChannels[i]->closeFuture()->awaitUninterruptibly();
    }

    // Shut down thread pools to exit.
    bootstrap.shutdown();
    return 0;
}
