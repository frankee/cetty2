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

#include <cetty/logging/LoggerHelper.h>
#include <cetty/logging/InternalLoggerFactory.h>

#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioServerSocketChannel.h>
#include <cetty/channel/socket/asio/AsioSocketAddressImpl.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

InternalLogger* AsioServerSocketChannel::logger = NULL;

AsioServerSocketChannel::AsioServerSocketChannel(
        boost::asio::ip::tcp::acceptor& acceptor,
        const AsioServicePtr& ioService,
        const ChannelFactoryPtr& factory,
        const ChannelPipelinePtr& pipeline,
        const ChannelSinkPtr& sink)
    : ServerSocketChannel(factory, pipeline, sink),
      ioService(ioService),
      acceptor(acceptor),
      config(acceptor) {
    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("AsioServerSocketChannel");
    }

    LOG_INFO(logger, "Server Channel firing the Channel Open Event.");
    Channels::fireChannelOpen(this);
}

AsioServerSocketChannel::~AsioServerSocketChannel() {

}

ChannelConfig& AsioServerSocketChannel::getConfig() {
    return config;
}

const ChannelConfig& AsioServerSocketChannel::getConfig() const {
    return config;
}

const SocketAddress& AsioServerSocketChannel::getLocalAddress() const {
    if (localAddress != SocketAddress::NULL_ADDRESS) {
        return localAddress;
    }

    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint endpoint = acceptor.local_endpoint(ec);

    if (ec) {
        return SocketAddress::NULL_ADDRESS;
    }

    localAddress = SocketAddress(
        SocketAddress::SmartPointer(new AsioTcpSocketAddressImpl(ioService->service(), endpoint)));

    return localAddress;
}

const SocketAddress& AsioServerSocketChannel::getRemoteAddress() const {
    return SocketAddress::NULL_ADDRESS;
}

bool AsioServerSocketChannel::isBound() const {
    return isOpen() && acceptor.is_open();
}

bool AsioServerSocketChannel::setClosed() {
    if (isBound()) {
        LOG_INFO(logger, "firing the server channel unbound event.");
        Channels::fireChannelUnbound(this);
    }

    LOG_INFO(logger, "firing the server channel closed event.");
    Channels::fireChannelClosed(this);

    LOG_INFO(logger, "Server Channel set closed.");
    return AbstractChannel::setClosed();
}

}
}
}
}