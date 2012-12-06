
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

#include <cetty/bootstrap/asio/AsioServerBootstrap.h>

#include <boost/assert.hpp>
#include <cetty/channel/asio/AsioService.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/channel/asio/AsioServerSocketChannel.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace bootstrap {
namespace asio {

using namespace cetty::channel::asio;

AsioServerBootstrap::AsioServerBootstrap(
    int parentThreadCnt,
    int childThreadCnt /*= 0*/)
    : socketAddressFactory(),
      ipAddressFactory() {

    EventLoopPoolPtr parent = new AsioServicePool(parentThreadCnt);
    EventLoopPoolPtr child;
    if (0 == childThreadCnt) {
        child = parent;
    }
    else {
        child = new AsioServicePool(childThreadCnt);
    }

    BOOST_ASSERT(parent && child && "ioServicePool SHOULD NOT BE NULL.");

    ServerBootstrap::setEventLoopPools(parent, child);

    init();
}

AsioServerBootstrap::AsioServerBootstrap(const EventLoopPoolPtr& pool)
    : ServerBootstrap(pool),
      socketAddressFactory(),
      ipAddressFactory() {
    BOOST_ASSERT(pool && "ioServicePool SHOULD NOT BE NULL.");
    init();
}

AsioServerBootstrap::AsioServerBootstrap(
    const EventLoopPoolPtr& parentPool,
    const EventLoopPoolPtr& childPool)
    : ServerBootstrap(parentPool, childPool),
    socketAddressFactory(),
    ipAddressFactory() {

    BOOST_ASSERT(parentPool && childPool && "ioServicePool SHOULD NOT BE NULL.");
    init();
}

AsioServerBootstrap::~AsioServerBootstrap() {
    deinit();
}

ChannelPtr AsioServerBootstrap::newChannel() {
    const EventLoopPoolPtr& parent = parentPool();
    if (parent) {
        return ChannelPtr(
                   new AsioServerSocketChannel(parent->getNextLoop(),
                                               childPool()));
    }
    else {
        LOG_WARN << "has not set the parent EventLoopPool.";
        return ChannelPtr();
    }
}

void AsioServerBootstrap::init() {
    if (!SocketAddress::hasFactory()) {
        EventLoopPtr loop = parentPool()->getNextLoop();
        AsioServicePtr service = boost::dynamic_pointer_cast<AsioService>(loop);
        BOOST_ASSERT(service && "AsioClientSocketChannelFactory only can init with AsioService");

        socketAddressFactory =
            new AsioTcpSocketAddressImplFactory(service->service());

        SocketAddress::setFacotry(socketAddressFactory);
    }

    if (!IpAddress::hasFactory()) {
        ipAddressFactory = new AsioIpAddressImplFactory();
        IpAddress::setFactory(ipAddressFactory);
    }
}

void AsioServerBootstrap::deinit() {
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

}
}
}
