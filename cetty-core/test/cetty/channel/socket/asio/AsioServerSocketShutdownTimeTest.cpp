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
#include "boost/asio.hpp"

#include "boost/thread.hpp"

#include "cetty/bootstrap/ServerBootstrap.h"
#include "cetty/channel/socket/asio/DummyHandler.h"
#include "cetty/channel/socket/asio/AsioServerSocketChannelFactory.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/ChannelPipeline.h"
#include "cetty/channel/SocketAddress.h"

#include "cetty/util/Exception.h"


/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2119 $, $Date: 2010-02-01 20:46:09 +0900 (Mon, 01 Feb 2010) $
 *
 */

using namespace boost::asio;
using namespace cetty::channel;
using namespace cetty::bootstrap;
using namespace cetty::util;
using namespace cetty::channel::socket;
using namespace cetty::channel::socket::asio;

TEST(ASioServerSocketShutdownTimeTest, testSuccessfulBindAttempt) {
    ServerBootstrap bootstrap(new AsioServerSocketChannelFactory());

    bootstrap.setOption("localAddress", boost::any(SocketAddress(1980)));
    bootstrap.setOption("child.receiveBufferSize", boost::any(9753));
    bootstrap.setOption("child.sendBufferSize", boost::any(8642));

    DummyHandlerPtr dummy = DummyHandlerPtr(new DummyHandler);
    bootstrap.getPipeline()->addLast("dummy", dummy);

    Channel* channel = bootstrap.bind();

    boost::system_time startTime;

    io_service ioService;
    ip::tcp::socket socket(ioService);

    try {
        socket.connect(ip::tcp::endpoint(ip::tcp::v4(), 1980));

        while (!dummy->connected) {
            boost::thread::yield();
        }

        socket.close();

        while (!dummy->connected) {
            boost::thread::yield();
        }
    }
    catch (...) {
	}

	startTime = boost::get_system_time();
    channel->close()->awaitUninterruptibly();
    bootstrap.releaseExternalResources();

	long shutdownTime = (boost::get_system_time() - startTime).total_milliseconds();
    ASSERT_TRUE(shutdownTime < 500 /*"Shutdown takes too long: " + shutdownTime + " ms"*/);
};

