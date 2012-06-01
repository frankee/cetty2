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

#include "cetty/bootstrap/ServerBootstrap.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/socket/asio/AsioServerSocketChannelFactory.h"
#include "HttpServerPipelineFactory.h"

using namespace cetty::bootstrap;
using namespace cetty::channel;
using namespace cetty::channel::socket::asio;

/**
 * An HTTP server that sends back the content of the received HTTP request
 * in a pretty plaintext form.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

int main(int argc, const char* argv[]) {
    // Configure the server.
    ServerBootstrap bootstrap (
        ChannelFactoryPtr(new AsioServerSocketChannelFactory(0)));

    bootstrap.setOption("child.tcpNoDelay", boost::any(true));
    bootstrap.setOption("reuseAddress", boost::any(true));
    bootstrap.setOption("backlog", boost::any(4096));

    // Set up the event pipeline factory.
    bootstrap.setPipelineFactory(
        ChannelPipelineFactoryPtr(new HttpServerPipelineFactory()));

    // Bind and start to accept incoming connections.
    bootstrap.bind(SocketAddress(IpAddress::IPv4, 8080));
};