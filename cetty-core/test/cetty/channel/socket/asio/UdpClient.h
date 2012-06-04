#if !defined(CETTY_CHANNEL_SOCKET_ASIO_UDPCLIENT_H)
#define CETTY_CHANNEL_SOCKET_ASIO_UDPCLIENT_H

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
#include "boost/asio.hpp"

using namespace boost::asio;

/**
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author Daniel Bevenius (dbevenius@jboss.com)
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */
class UdpClient {
public:
    UdpClient(const address, int port) {
        this.address = address;
        this.port = port;
        clientSocket = new DatagramSocket();
        clientSocket.setReuseAddress(true);
    }

    DatagramPacket send(final byte[] payload) {
        final DatagramPacket dp =
                new DatagramPacket(payload, payload.length, address, port);
        clientSocket.send(dp);
        return dp;
    }

	void receive(boost::asio::buffer() dp) {
        clientSocket.receive(dp);
    }

    void close() {
        clientSocket.close();
    }

private:
	SocketAddress address;
	ip::udp::socket clientSocket;
	int port;
};

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_UDPCLIENT_H)
