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

#include "cetty/bootstrap/ServerBootstrap.h"
#include "cetty/channel/socket/asio/AsioServerSocketChannelFactory.h"
#include "TelnetServerPipelineFactory.h"

using namespace cetty::bootstrap;
using namespace cetty::channel;
using namespace cetty::channel::socket::asio;

/**
 * Simplistic telnet server.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

int main(int argc, char* argv[]) {
    // Configure the server.
    ServerBootstrap bootstrap(
        ChannelFactoryPtr(new AsioServerSocketChannelFactory(-1)));

    // Configure the pipeline factory.
    bootstrap.setPipelineFactory(
        ChannelPipelineFactoryPtr(new TelnetServerPipelineFactory()));

    // Bind and start to accept incoming connections.
    Channel* c = bootstrap.bind(SocketAddress(IpAddress::IPv4, 8080));

    if (c->isBound()) {
        printf("Server is running...\n");
        printf("To quit server, press 'q'.\n");

        char input;
        do {
            input = getchar();
            if (input == 'q') {
                c->close()->awaitUninterruptibly();
                bootstrap.releaseExternalResources();
                return 0;
            }
        } while (true);
    }

    bootstrap.releaseExternalResources();
    return -1;
}
