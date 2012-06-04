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

#include "cetty/bootstrap/ClientBootstrap.h"
#include "cetty/channel/socket/asio/DummyHandler.h"
#include "cetty/channel/socket/asio/AsioClientSocketChannelFactory.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/ChannelPipeline.h"
#include "cetty/channel/SocketAddress.h"

#include "cetty/util/Exception.h"

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 *
 */

using namespace boost::asio;
using namespace cetty::channel;
using namespace cetty::bootstrap;
using namespace cetty::util;
using namespace cetty::channel::socket;
using namespace cetty::channel::socket::asio;

TEST(ASioClientSocketShutdownTimeTest, testShutdownTime) {
    io_service ioService;
    ip::tcp::acceptor acceptor(ioService, ip::tcp::endpoint(ip::tcp::v4(), 1980));
    ip::tcp::socket socket(ioService);

    ClientBootstrap b(new AsioClientSocketChannelFactory());
    b.getPipeline()->addLast("handler", ChannelHandlerPtr(new DummyHandler()));

    boost::system_time startTime;
    boost::system_time stopTime;

    try {
        ChannelFuturePtr f = b.connect(SocketAddress("127.0.0.1", 1980));

        acceptor.accept(socket);
        f->awaitUninterruptibly();

        if (f->getCause() != NULL) {
            f->getCause()->rethrow();
        }
        ASSERT_TRUE(f->isSuccess());

        startTime = boost::get_system_time();

        f->getChannel().close()->awaitUninterruptibly();
    }
    catch(...) {
    }

    b.getFactory()->releaseExternalResources();

    stopTime = boost::get_system_time();

    try {
        acceptor.close();
    }
    catch (const std::exception& ex) {
        // Ignore.
    }

    boost::system_time::time_duration_type shutdownTime = stopTime - startTime;
    ASSERT_TRUE(shutdownTime < boost::posix_time::milliseconds(500));
};

