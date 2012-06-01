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

#include "cetty/channel/socket/asio/AsioServerSocketChannelFactory.h"
#include "cetty/channel/socket/asio/AsioClientSocketChannelFactory.h"
#include "cetty/bootstrap/ServerBootstrap.h"

#include "cetty/util/Integer.h"

#include "HexDumpProxyPipelineFactory.h"

using namespace cetty::bootstrap;
using namespace cetty::util;
using namespace cetty::channel::socket::asio;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

int main(int argc, char* argv[]) {
    // Validate command line options.
    if ( argc != 4) {
        printf(
                "Usage: HexDumpProxy"
                " <local port> <remote host> <remote port>\n");
        return -1;
    }

    // Parse command line options.
    int localPort = Integer::parse(argv[1]);
    std::string remoteHost = argv[2];
    int remotePort = Integer::parse(argv[3]);

    printf("Proxying *:%d to %s:%d ...\n", localPort, remoteHost.c_str(), remotePort);

    // Configure the bootstrap.
    
    ServerBootstrap sb(
            ChannelFactoryPtr(new AsioServerSocketChannelFactory()));

    // Set up the event pipeline factory.
    ChannelFactoryPtr cf =
            ChannelFactoryPtr(new AsioClientSocketChannelFactory());

    sb.setPipelineFactory(
        ChannelPipelineFactoryPtr(
            new HexDumpProxyPipelineFactory(cf, remoteHost, remotePort)));

    // Start up the server.
    Channel* c = sb.bind(SocketAddress(IpAddress::IPv4, localPort));
    if (c->isBound()) {
        printf("Server is running...\n");
        printf("To quit server, press 'q'.\n");

        char input;
        do {
            input = getchar();
            if (input == 'q') {
                c->close()->awaitUninterruptibly();
                sb.releaseExternalResources();
                return 0;
            }
        } while (true);
    }

    sb.releaseExternalResources();
    return -1;
}
