
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

#include <cetty/channel/asio/AsioService.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/channel/asio/AsioServerSocketChannel.h>

namespace cetty {
namespace bootstrap {
namespace asio {

    using namespace cetty::channel::asio;

AsioServerBootstrap::AsioServerBootstrap(const EventLoopPoolPtr& pool)
    : parentPool(pool),
      childPool(pool),
      socketAddressFactory(),
      ipAddressFactory() {
    BOOST_ASSERT(pool && "ioServicePool SHOULD NOT BE NULL.");
    init();
}

AsioServerBootstrap::AsioServerBootstrap(
    int parentThreadCnt,
    int childThreadCnt /*= 0*/)
    : socketAddressFactory(),
      ipAddressFactory() {

    parentPool = new AsioServicePool(parentThreadCnt);

    if (0 == childThreadCnt) {
        childPool = parentPool;
    }
    else {
        childPool = new AsioServicePool(childThreadCnt);
    }

    BOOST_ASSERT(parentPool && childPool && "ioServicePool SHOULD NOT BE NULL.");
    init();
}

AsioServerBootstrap::AsioServerBootstrap(
    const EventLoopPoolPtr& parentPool,
    const EventLoopPoolPtr& childPool)
    : parentPool(parentPool),
      childPool(childPool),
      socketAddressFactory(),
      ipAddressFactory() {

    BOOST_ASSERT(parentPool && childPool && "ioServicePool SHOULD NOT BE NULL.");
    init();
}

AsioServerBootstrap::~AsioServerBootstrap() {
    deinit();
}

ChannelPtr AsioServerBootstrap::newChannel() {
    ChannelPtr channel(
        new AsioServerSocketChannel(parentPool->getNextLoop(),
                                    childPool));

//     if (channel->isOpen()) {
//         LOG_INFO << "Created the AsioServerSocketChannel, firing the Channel Created Event.";
//         channel->pipeline().fireChannelOpen();
// 
//         serverChannels.push_back(channel);
//     }
//     else {
//         channel.reset();
//     }

    return channel;
}

void AsioServerBootstrap::shutdown() {
    childPool->stop();
    childPool->waitForStop();

    parentPool->stop();
    parentPool->waitForStop();

    serverChannels.clear();
}

void AsioServerBootstrap::init() {
    if (!SocketAddress::hasFactory()) {
        EventLoopPtr loop = parentPool->getNextLoop();
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
