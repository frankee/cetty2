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
#include <cetty/channel/socket/asio/AsioServerSocketChannelFactory.h>

#include <cetty/channel/IpAddress.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/channel/socket/asio/AsioServerSocketChannel.h>
#include <cetty/channel/socket/asio/AsioSocketAddressImplFactory.h>
#include <cetty/channel/socket/asio/AsioIpAddressImplFactory.h>
#include <cetty/channel/socket/asio/AsioService.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioSocketChannel.h>
#include <cetty/util/internal/asio/AsioDeadlineTimerFactory.h>
#include <cetty/util/NestedDiagnosticContext.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::logging;
using namespace cetty::util::internal::asio;

AsioServerSocketChannelFactory::AsioServerSocketChannelFactory(
    const EventLoopPoolPtr& pool)
    : parentPool(pool),
      childPool(pool),
      socketAddressFactory(NULL),
      ipAddressFactory(NULL) {
    BOOST_ASSERT(pool && "ioServicePool SHOULD NOT BE NULL.");
    init();
}

AsioServerSocketChannelFactory::AsioServerSocketChannelFactory(int parentThreadCnt,
        int childThreadCnt /*= 0*/)
    : ipAddressFactory(),
      socketAddressFactory() {
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

AsioServerSocketChannelFactory::AsioServerSocketChannelFactory(
    const EventLoopPoolPtr& parentPool,
        const EventLoopPoolPtr& childPool)
    : parentPool(parentPool),
      childPool(childPool),
      ipAddressFactory(),
      socketAddressFactory() {

    BOOST_ASSERT(parentPool && childPool && "ioServicePool SHOULD NOT BE NULL.");
    init();
}

AsioServerSocketChannelFactory::~AsioServerSocketChannelFactory() {
    deinit();
}

ChannelPtr AsioServerSocketChannelFactory::newChannel(const ChannelPipelinePtr& pipeline) {
    ChannelPtr channel =
        new AsioServerSocketChannel(parentPool->getNextLoop(),
            shared_from_this(),
            pipeline,
            childPipeline,
            childPool);
    LOG_INFO() << "Created the AsioServerSocketChannel.";
    serverChannels.push_back(channel);
    return channel;
}

void AsioServerSocketChannelFactory::shutdown() {
    childPool->stop();
    childPool->waitForStop();

    parentPool->stop();
    parentPool->waitForStop();

    serverChannels.clear();
}

void AsioServerSocketChannelFactory::init() {
    if (!TimerFactory::hasFactory()) {
        timerFactory = new AsioDeadlineTimerFactory(parentPool, childPool);
        TimerFactory::setFactory(timerFactory);
    }
    else {
        // log info here.
        LOG_WARN() << "the timer factory has already been set.";
    }

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

void AsioServerSocketChannelFactory::deinit() {
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

    if (timerFactory) {
        TimerFactory::resetFactory();

        delete timerFactory;
        timerFactory = NULL;
    }
}

void AsioServerSocketChannelFactory::setChildChannelPipeline(
    const ChannelPipelinePtr& pipeline) {
    this->childPipeline = pipeline;
}

const ChannelPipelinePtr& AsioServerSocketChannelFactory::getChildChannelPipeline() const {
    return this->childPipeline;
}

}
}
}
}
