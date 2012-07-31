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
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioSocketChannel.h>
#include <cetty/util/internal/asio/AsioDeadlineTimerFactory.h>
#include <cetty/util/NestedDiagnosticContext.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/logging/InternalLoggerFactory.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::logging;
using namespace cetty::util::internal::asio;

InternalLogger* AsioServerSocketChannelFactory::logger = NULL;

AsioServerSocketChannelFactory::AsioServerSocketChannelFactory(const AsioServicePoolPtr& ioServicePool)
    : serverServicePool(ioServicePool),
      childServicePool(ioServicePool),
      socketAddressFactory(NULL),
      ipAddressFactory(NULL) {
    BOOST_ASSERT(ioServicePool && "ioServicePool SHOULD NOT BE NULL.");
    init();
}

AsioServerSocketChannelFactory::AsioServerSocketChannelFactory(int parentThreadCnt,
        int childThreadCnt /*= 0*/)
    : ipAddressFactory(),
      socketAddressFactory() {
    serverServicePool = new AsioServicePool(parentThreadCnt);

    if (0 == childThreadCnt) {
        childServicePool = serverServicePool;
    }
    else {
        childServicePool = new AsioServicePool(childThreadCnt);
    }

    BOOST_ASSERT(serverServicePool && childServicePool && "ioServicePool SHOULD NOT BE NULL.");
    init();
}

AsioServerSocketChannelFactory::AsioServerSocketChannelFactory(const AsioServicePoolPtr& parentPool,
        const AsioServicePoolPtr& childPool)
    : serverServicePool(parentPool),
      childServicePool(childPool),
      ipAddressFactory(),
      socketAddressFactory() {

    BOOST_ASSERT(serverServicePool && childServicePool && "ioServicePool SHOULD NOT BE NULL.");
    init();
}

AsioServerSocketChannelFactory::~AsioServerSocketChannelFactory() {
    deinit();
}

ChannelPtr AsioServerSocketChannelFactory::newChannel(const ChannelPipelinePtr& pipeline) {
    ChannelPtr channel =
        new AsioServerSocketChannel(serverServicePool->getService(),
            shared_from_this(),
            pipeline,
            childPipeline,
            childServicePool);
    LOG_INFO(logger, "Created the AsioServerSocketChannel.");

    if (serverServicePool->onlyMainThread()) {
        LOG_INFO(logger, "the asio service pool starting to run in main thread.");

        if (!serverServicePool->run()) {
            LOG_ERROR(logger, "start the boost asio service error, stop service pool and close channel.");
            serverServicePool->stop();
            channel->getPipeline()->fireExceptionCaught(
                ChannelException("failed to start the asio service."));
            channel->close();
            return ChannelPtr();
        }
        else {
            LOG_INFO(logger, "the asio service pool started to running.");
        }
    }

    serverChannels.push_back(channel);
    return channel;
}

void AsioServerSocketChannelFactory::shutdown() {
    childServicePool->stop();
    childServicePool->waitForExit();

    serverServicePool->stop();
    serverServicePool->waitForExit();

    serverChannels.clear();
}

void AsioServerSocketChannelFactory::init() {
    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("AsioServerSocketChannelFactory");
    }

    if (!TimerFactory::hasFactory()) {
        timerFactory = new AsioDeadlineTimerFactory(serverServicePool);
        TimerFactory::setFactory(timerFactory);
    }
    else {
        // log info here.
        LOG_WARN(logger, "the timer factory has already been set.");
    }

    if (!SocketAddress::hasFactory()) {
        socketAddressFactory = new AsioTcpSocketAddressImplFactory(
            serverServicePool->getService()->service());
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
