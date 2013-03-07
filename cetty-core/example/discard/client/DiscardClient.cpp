/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/bootstrap/ClientBootstrap.h>
#include <cetty/channel/asio/AsioClientSocketChannelFactory.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelines.h>
#include <cetty/channel/IpAddress.h>
#include <cetty/channel/InetAddress.h>
#include <cetty/channel/ChannelFuture.h>

#include "DiscardClientHandler.h"


using namespace cetty::channel;
using namespace cetty::channel::socket::asio;

using namespace cetty::bootstrap;
using namespace cetty::buffer;

using namespace cetty::util;

/**
 * Keeps sending random data to the specified address.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

int main(int argc, const char* argv[]) {
    // Print usage if no argument is specified.
    if (argc < 3 || argc > 4) {
        printf("Usage: DiscardClient"
               " <host> <port> [<first message size>]");
        return -1;
    }

    // Parse options.
    std::string host = argv[1];
    int port = (int)StringUtil::atoi(argv[2]);
    int firstMessageSize;

    if (argc == 4) {
        firstMessageSize = (int)StringUtil::atoi(argv[3]);
    }
    else {
        firstMessageSize = 256;
    }

    // Configure the client.
    ClientBootstrap bootstrap(
        ChannelFactoryPtr(new AsioClientSocketChannelFactory(1)));

    // Set up the pipeline factory.
    bootstrap.setPipeline(
        ChannelPipelines::pipeline(
            new DiscardClientHandler(firstMessageSize)));

    // Start the connection attempt.
    ChannelFuturePtr future = bootstrap.connect(host, port);
    future->awaitUninterruptibly();

    ChannelPtr channel = future->channel();

    // Wait until the connection is closed or the connection attempt fails.
    channel->closeFuture()->awaitUninterruptibly();

    // Shut down thread pools to exit.
    bootstrap.shutdown();

    return 0;
}
