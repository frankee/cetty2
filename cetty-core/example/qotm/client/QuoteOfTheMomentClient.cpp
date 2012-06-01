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

#include "cetty/bootstrap/ConnectionlessBootstrap.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/Channels.h"
#include "cetty/channel/IpAddress.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/socket/asio/AsioDatagramChannelFactory.h"
#include "cetty/handler/codec/string/StringDecoder.h"
#include "cetty/handler/codec/string/StringEncoder.h"

#include "QuoteOfTheMomentClientHandler.h"

using namespace cetty::bootstrap;
using namespace cetty::channel;
using namespace cetty::channel::socket::asio;
using namespace cetty::handler::codec::string;

/**
 * A UDP broadcast client that asks for a quote of the moment (QOTM) to
 * {@link QuoteOfTheMomentServer}.
 *
 * Inspired by <a href="http://java.sun.com/docs/books/tutorial/networking/datagrams/clientServer.html">the official Java tutorial</a>.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2104 $, $Date: 2010-01-28 16:50:28 +0900 (Thu, 28 Jan 2010) $
 */
int main(int argc, char* argv[]) {
    ConnectionlessBootstrap b(ChannelFactoryPtr(new AsioDatagramChannelFactory()));

    // Configure the pipeline factory.
    b.setPipeline(Channels::pipeline(
        ChannelHandlerPtr(new StringEncoder()),
        ChannelHandlerPtr(new StringDecoder()),
        ChannelHandlerPtr(new QuoteOfTheMomentClientHandler())));

    // Enable broadcast
    b.setOption("broadcast", boost::any(true));

    // Allow packets as large as up to 1024 bytes (default is 768).
    // You could increase or decrease this value to avoid truncated packets
    // or to improve memory footprint respectively.
    //
    // Please also note that a large UDP packet might be truncated or
    // dropped by your router no matter how you configured this option.
    // In UDP, a packet is truncated or dropped if it is larger than a
    // certain size, depending on router configuration.  IPv4 routers
    // truncate and IPv6 routers drop a large packet.  That's why it is
    // safe to send small packets in UDP.
    //b.setOption(
    //        "receiveBufferSizePredictorFactory",
    //        new FixedReceiveBufferSizePredictorFactory(1024));

    Channel* c = b.bind(SocketAddress(IpAddress::IPv4));

    // Broadcast the QOTM request to port 8080.
    c->write(std::string("QOTM?"), SocketAddress("127.0.0.1"/*"255.255.255.255"*/, 8080));

    // QuoteOfTheMomentClientHandler will close the DatagramChannel when a
    // response is received.  If the channel is not closed within 5 seconds,
    // print an error message and quit.
    if (!c->getCloseFuture()->awaitUninterruptibly(5000)) {
        printf("QOTM request timed out.");
        c->close()->awaitUninterruptibly();
    }

    b.releaseExternalResources();
}
