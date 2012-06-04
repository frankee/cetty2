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
#include "gtest/gtest.h"

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author Daniel Bevenius (dbevenius@jboss.com)
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */
class AsioDatagramChannelTest : public testing::Test {
    private static Channel sc;

    private static SocketAddress inetSocketAddress;

    void Setup() {
        final NioDatagramChannelFactory channelFactory = new NioDatagramChannelFactory(
                Executors.newCachedThreadPool());
        final ConnectionlessBootstrap sb = new ConnectionlessBootstrap(channelFactory);
        inetSocketAddress = new InetSocketAddress("localhost", 9999);
        sc = sb.bind(inetSocketAddress);
        final SimpleHandler handler = new SimpleHandler();
        sc.getPipeline().addFirst("handler", handler);
    }


	void TearDown() {
			if (sc != null) {
				final ChannelFuture future = sc.close();
				if (future != null) {
					future.awaitUninterruptibly();
				}
			}
	}

    void checkBoundPort() {
        final InetSocketAddress socketAddress = (InetSocketAddress) sc
                .getLocalAddress();
        assertEquals(9999, socketAddress.getPort());
    }

    void sendReciveOne() {
        final String expectedPayload = "some payload";
        sendRecive(expectedPayload);
    }

    void sendReciveMultiple() {
        final String expectedPayload = "some payload";
        for (int i = 0; i < 1000; i ++) {
            sendRecive(expectedPayload);
        }
    }

    void clientBootstrap() {
        ConnectionlessBootstrap bootstrap(new AsioDatagramChannelFactory());
        bootstrap.getPipeline()->addLast("test", ChannelHandlerPtr(new SimpleHandler()));
		bootstrap.setOption("tcpNoDelay", boost::any(true));
		bootstrap.setOption("keepAlive", boost::any(true));

        SocketAddress clientAddress("localhost", 8888);
		bootstrap.setOption("localAddress", boost::any(clientAddress));

        ChannelFuturePtr ccf = bootstrap.connect(inetSocketAddress);
        ccf->awaitUninterruptibly();

        Channel& cc = ccf->getChannel();
		ChannelFuturePtr write = cc.write(ChannelBuffers::copiedBuffer("client payload"));
        write->awaitUninterruptibly();
    }

private:
	void sendRecive(const std::string& expectedPayload) {
        UdpClient udpClient = new UdpClient(inetSocketAddress
                .getAddress(), inetSocketAddress.getPort());
        final DatagramPacket dp = udpClient.send(expectedPayload.getBytes());

        dp.setData(new byte[expectedPayload.length()]);
        assertFalse("The payload should have been cleared", expectedPayload
                .equals(new String(dp.getData())));

        udpClient.receive(dp);

        assertEquals(expectedPayload, new String(dp.getData()));
        udpClient.close();
    }
};
