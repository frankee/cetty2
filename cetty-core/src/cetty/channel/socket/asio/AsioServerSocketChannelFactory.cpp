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
#include <cetty/channel/socket/asio/AsioServerSocketPipelineSink.h>
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
    : ioServicePool(ioServicePool),
      socketAddressFactory(NULL),
      ipAddressFactory(NULL) {
    BOOST_ASSERT(ioServicePool && "ioServicePool SHOULD NOT BE NULL.");
    init();
}

AsioServerSocketChannelFactory::AsioServerSocketChannelFactory(int threadCnt)
    : ioServicePool(new AsioServicePool(threadCnt)),
      ipAddressFactory(NULL),
      socketAddressFactory(NULL) {
    BOOST_ASSERT(ioServicePool && "ioServicePool SHOULD NOT BE NULL.");
    init();
}

AsioServerSocketChannelFactory::~AsioServerSocketChannelFactory() {
    deinit();
}

ChannelPtr AsioServerSocketChannelFactory::newChannel(const ChannelPipelinePtr& pipeline) {
    ChannelPtr channel;
    AsioServerSocketPipelineSink* sink =
        new AsioServerSocketPipelineSink(ioServicePool);

    channel = new AsioServerSocketChannel(sink->getAcceptor(),
                                          sink->getAcceptorService(),
                                          shared_from_this(),
                                          pipeline,
                                          sink);
    LOG_INFO(logger, "Created the AsioServerSocketChannel.");

    if (ioServicePool->onlyMainThread()) {
        LOG_INFO(logger, "the asio service pool starting to run in main thread.");

        if (!ioServicePool->run()) {
            LOG_ERROR(logger, "start the boost asio service error, stop service pool and close channel.");
            ioServicePool->stop();
            Channels::fireExceptionCaught(channel, ChannelException("failed to start the asio service."));
            channel->close();
            return ChannelPtr();
        }
        else {
            LOG_INFO(logger, "the asio service pool started to running.");
        }
    }

    channels.push_back(channel);
    return channel;
}

void AsioServerSocketChannelFactory::releaseExternalResources() {
    ioServicePool->stop();
    ioServicePool->waitForExit();

    channels.clear();
}

void AsioServerSocketChannelFactory::init() {
    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("AsioServerSocketChannelFactory");
    }

    if (!TimerFactory::hasFactory()) {
        timerFactory = new AsioDeadlineTimerFactory(ioServicePool);
        TimerFactory::setFactory(timerFactory);
    }
    else {
        // log info here.
        LOG_WARN(logger, "the timer factory has already been set.");
    }

    if (!SocketAddress::hasFactory()) {
        socketAddressFactory = new AsioTcpSocketAddressImplFactory(
            ioServicePool->getService(AsioServicePool::PRIORITY_BOSS)->service());
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




}
}
}
}
