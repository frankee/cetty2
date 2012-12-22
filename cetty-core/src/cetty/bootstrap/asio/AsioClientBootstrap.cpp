
/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/bootstrap/asio/AsioClientBootstrap.h>
#include <cetty/channel/asio/AsioService.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/channel/asio/AsioSocketChannel.h>

namespace cetty {
namespace bootstrap {
namespace asio {

using namespace cetty::channel::asio;

AsioClientBootstrap::AsioClientBootstrap(
    const EventLoopPtr& eventLoop)
    : eventLoop_(eventLoop),
      socketAddressFactory(NULL),
      ipAddressFactory(NULL) {
    init();
}

AsioClientBootstrap::AsioClientBootstrap(
    const EventLoopPoolPtr& eventLoopPool)
    : ClientBootstrap(eventLoopPool),
      socketAddressFactory(NULL),
      ipAddressFactory(NULL) {
    init();
}

AsioClientBootstrap::AsioClientBootstrap(int threadCnt)
    : socketAddressFactory(NULL),
      ipAddressFactory(NULL) {
    if (threadCnt < 1) {
        LOG_WARN << "client bootstrap thread count should no less than 1";
        threadCnt = 1;
    }

    EventLoopPoolPtr pool = new AsioServicePool(threadCnt);
    setEventLoopPool(pool);

    init();
}

AsioClientBootstrap::~AsioClientBootstrap() {
    deinit();
}

void AsioClientBootstrap::init() {
    if (!SocketAddress::hasFactory()) {
        const EventLoopPoolPtr& pool = eventLoopPool();
        EventLoopPtr loop = pool ? pool->getNextLoop() : eventLoop_;

        AsioServicePtr service = boost::dynamic_pointer_cast<AsioService>(loop);
        BOOST_ASSERT(service && "AsioClientBootstrap only can init with AsioService");

        socketAddressFactory =
            new AsioTcpSocketAddressImplFactory(service->service());

        SocketAddress::setFacotry(socketAddressFactory);
    }

    if (!IpAddress::hasFactory()) {
        ipAddressFactory = new AsioIpAddressImplFactory();
        IpAddress::setFactory(ipAddressFactory);
    }
}

void AsioClientBootstrap::deinit() {
    if (socketAddressFactory) {
        SocketAddress::resetFactory();

        delete socketAddressFactory;
        socketAddressFactory = NULL;
    }

    if (ipAddressFactory) {
        IpAddress::resetFactory();

        delete ipAddressFactory;
        ipAddressFactory = NULL;
    }
}

ChannelPtr AsioClientBootstrap::newChannel() {
    const EventLoopPoolPtr& pool = eventLoopPool();
    const EventLoopPtr& eventLoop = pool ? pool->getNextLoop()
                                    : eventLoop_;

    return ChannelPtr(new AsioSocketChannel(eventLoop));
}

}
}
}
