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

#include <cetty/service/asio/AsioClientServiceFactory.h>

#include <cetty/channel/IpAddress.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/channel/asio/AsioService.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/channel/asio/AsioIpAddressImplFactory.h>
#include <cetty/channel/asio/AsioSocketAddressImplFactory.h>

#include <cetty/service/ClientService.h>

namespace cetty {
namespace service {
namespace asio {

using namespace cetty::channel;
using namespace cetty::channel::socket::asio;
using namespace cetty::util;

AsioClientServiceFactory::AsioClientServiceFactory(int threadCnt)
    : eventLoop(),
      eventLoopPool(new AsioServicePool(threadCnt)),
      socketAddressFactory(),
      ipAddressFactory() {
    init();
}

AsioClientServiceFactory::AsioClientServiceFactory(const EventLoopPtr& eventLoop)
    : eventLoop(eventLoop),
      eventLoopPool(),
      socketAddressFactory(),
      ipAddressFactory() {
    init();
}

AsioClientServiceFactory::AsioClientServiceFactory(
    const EventLoopPoolPtr& eventLoopPool)
    : eventLoop(),
      eventLoopPool(eventLoopPool),
      socketAddressFactory(),
      ipAddressFactory() {
    init();
}

AsioClientServiceFactory::~AsioClientServiceFactory() {
    deinit();
}

ChannelPtr AsioClientServiceFactory::newChannel(const ChannelPipelinePtr& pipeline) {
    return newChannel(pipeline, this->eventLoop);
}

ChannelPtr AsioClientServiceFactory::newChannel(const ChannelPipelinePtr& pipeline,
        const EventLoopPtr& eventLoop) {
    if (eventLoop) {
        return ChannelPtr(new ClientService(eventLoop,
                                 shared_from_this(),
                                 pipeline));
    }
    else {
        return ChannelPtr();
    }
}

void AsioClientServiceFactory::shutdown() {
    if (eventLoopPool) {
        eventLoopPool->stop();
        eventLoopPool->waitForStop();
    }

    channels.clear();
}

void AsioClientServiceFactory::init() {
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

void AsioClientServiceFactory::deinit() {
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

EventLoopPtr AsioClientServiceFactory::getEventLoop() {
    if (eventLoopPool) {
        return eventLoopPool->getNextLoop();
    }
    else {
        return eventLoop;
    }
}

}
}
}
