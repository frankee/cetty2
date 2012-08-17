/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/channel/socket/asio/AsioClientSocketChannelFactory.h>

#include <cetty/channel/socket/asio/AsioService.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioIpAddressImplFactory.h>
#include <cetty/channel/socket/asio/AsioSocketAddressImplFactory.h>
#include <cetty/channel/socket/asio/AsioSocketChannel.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::util;

AsioClientSocketChannelFactory::AsioClientSocketChannelFactory(
    const EventLoopPtr& eventLoop)
    : eventLoop(eventLoop),
      eventLoopPool(),
      socketAddressFactory(NULL),
      ipAddressFactory(NULL) {
    init();
}

AsioClientSocketChannelFactory::AsioClientSocketChannelFactory(
    const EventLoopPoolPtr& eventLoopPool)
    : eventLoop(),
      eventLoopPool(eventLoopPool),
      socketAddressFactory(NULL),
      ipAddressFactory(NULL) {
    init();
}

AsioClientSocketChannelFactory::AsioClientSocketChannelFactory(int threadCnt)
    : eventLoopPool(new AsioServicePool(threadCnt)),
      socketAddressFactory(NULL),
      ipAddressFactory(NULL) {
    init();
}

AsioClientSocketChannelFactory::~AsioClientSocketChannelFactory() {
    deinit();
}

void AsioClientSocketChannelFactory::init() {
    if (!SocketAddress::hasFactory()) {
        EventLoopPtr loop
            = eventLoopPool ? eventLoopPool->getNextLoop() : eventLoop;
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

void AsioClientSocketChannelFactory::deinit() {
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

ChannelPtr AsioClientSocketChannelFactory::newChannel(const ChannelPipelinePtr& pipeline) {
    const EventLoopPtr& eventLoop = eventLoopPool ?
                                    eventLoopPool->getNextLoop()
                                    : this->eventLoop;

    ChannelPtr client =
        new AsioSocketChannel(eventLoop,
                              shared_from_this(),
                              pipeline);

    clientChannels.push_back(client);
    return client;
}

void AsioClientSocketChannelFactory::shutdown() {
    if (eventLoopPool) {
        eventLoopPool->stop();
        eventLoopPool->waitForStop();
    }

    clientChannels.clear();
}

}
}
}
}
