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

#include "cetty/bootstrap/ClientBootstrap.h"
#include "cetty/channel/socket/asio/AsioClientSocketChannelFactory.h"
#include "cetty/handler/timeout/ReadTimeoutHandler.h"
#include "cetty/util/Integer.h"

#include "UptimeClientHandler.h"

using namespace cetty::bootstrap;
using namespace cetty::channel::socket::asio;
using namespace cetty::handler::timeout;
using namespace cetty::util;


/**
 * Connects to a server periodically to measure and print the uptime of the
 * server.  This example demonstrates how to implement reliable reconnection
 * mechanism in Cetty.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

// Sleep 5 seconds before a reconnection attempt.
static const int RECONNECT_DELAY = 5;

class UptimeClient {
public:
    UptimeClient(int argc, char* argv[]) {
        // Print usage if no argument is specified.
        if (argc != 3) {
            printf(
                    "Usage: UptimeClient <host> <port>");
            return;
        }

        // Parse options.
        std::string host = argv[1];
        int port = Integer::parse(argv[2]);

        // Initialize the timer that schedules subsequent reconnection attempts.
        //final Timer timer = new HashedWheelTimer();

        // Configure the client.
        ClientBootstrap bootstrap(ChannelFactoryPtr(
                new AsioClientSocketChannelFactory(0)));

        // Configure the pipeline factory.
        bootstrap.getPipeline()->addLast(
            "ReadTimeoutHandler", ChannelHandlerPtr(new ReadTimeoutHandler(READ_TIMEOUT)));
        bootstrap.getPipeline()->addLast(
            "UptimeClientHandler", ChannelHandlerPtr(new UptimeClientHandler(bootstrap)));

        bootstrap.setOption("remoteAddress", SocketAddress(host, port));

        // Initiate the first connection attempt - the rest is handled by
        // UptimeClientHandler.
        bootstrap.connect();
    }

private:
    // Reconnect when the server sends nothing for 10 seconds.
    static const int READ_TIMEOUT = 10;

};


void main(int argc, char* argv[]) {
    UptimeClient(argc, argv);
}
