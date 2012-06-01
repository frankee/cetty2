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

#include <iostream>
#include "cetty/bootstrap/ClientBootstrap.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/socket/asio/AsioClientSocketChannelFactory.h"

#include "cetty/util/Integer.h"
#include "cetty/util/Exception.h"

#include "TelnetClientPipelineFactory.h"

using namespace cetty::bootstrap;
using namespace cetty::util;
using namespace cetty::channel::socket::asio;

/**
 * Simplistic telnet client.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */
int main(int argc, char* argv[]) {
    // Print usage if no argument is specified.
    if (argc != 3) {
        printf("Usage: TelnetClient <host> <port>");
        return -1;
    }

    // Parse options.
    std::string host = argv[1];
    int port = Integer::parse(argv[2]);

    // Configure the client.
    ClientBootstrap bootstrap(
        ChannelFactoryPtr(new AsioClientSocketChannelFactory()));

    // Configure the pipeline factory.
    bootstrap.setPipelineFactory(
        ChannelPipelineFactoryPtr(new TelnetClientPipelineFactory()));

    // Start the connection attempt.
    ChannelFuturePtr future = bootstrap.connect(SocketAddress(host, port));

    // Wait until the connection attempt succeeds or fails.
    Channel& channel = future->awaitUninterruptibly().getChannel();
    if (!future->isSuccess()) {
        printf("Exception happened, %s.", future->getCause()->what());
        bootstrap.releaseExternalResources();
        return -1;
    }

    // Read commands from the stdin.
    ChannelFuturePtr lastWriteFuture;
    std::string line;
    while (true) {
        line.clear();
        std::cout << "->";
        getline(std::cin, line);
//         if (line.size() == 0) {
//             break;
//         }

        line.append("\r\n");

        // Sends the received line to the server.
        lastWriteFuture = channel.write(ChannelBuffers::copiedBuffer(line));

        // this method will happend two content copies.
        //lastWriteFuture = channel.write(line);

        // If user typed the 'bye' command, wait until the server closes
        // the connection.
        if (line.compare("bye\r\n") == 0) {
            channel.getCloseFuture()->awaitUninterruptibly();
            break;
        }
    }

    // Wait until all messages are flushed before closing the channel.
    if (lastWriteFuture) {
        lastWriteFuture->awaitUninterruptibly();
    }

    // Close the connection.  Make sure the close operation ends because
    // all I/O operations are asynchronous in Netty.
    channel.close()->awaitUninterruptibly();

    // Shut down all thread pools to exit.
    bootstrap.releaseExternalResources();
}
