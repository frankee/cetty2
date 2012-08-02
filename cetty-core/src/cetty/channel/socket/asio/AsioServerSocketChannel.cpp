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

#include <cetty/channel/socket/asio/AsioServerSocketChannel.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/logging/InternalLoggerFactory.h>

#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelines.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/AbstractChannelSink.h>
#include <cetty/channel/socket/asio/AsioSocketChannel.h>
#include <cetty/channel/socket/asio/AsioService.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioSocketAddressImpl.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;

InternalLogger* AsioServerSocketChannel::logger = NULL;

AsioServerSocketChannel::AsioServerSocketChannel(
    const EventLoopPtr& eventLoop,
    const ChannelFactoryPtr& factory,
    const ChannelPipelinePtr& pipeline,
    const ChannelPipelinePtr& childPipeline,
    const EventLoopPoolPtr& childEventLoopPool)
    : ServerSocketChannel(eventLoop, factory, pipeline),
      ioService(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      sink(),
      childPipeline(childPipeline),
      childServicePool(boost::dynamic_pointer_cast<AsioServicePool>(childEventLoopPool)),
      acceptor(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      config(acceptor) {
    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("AsioServerSocketChannel");
    }

    sink = new AbstractChannelSink(*this);
    AbstractChannel::setPipeline(pipeline);
}

AsioServerSocketChannel::~AsioServerSocketChannel() {
    if (sink) {
        delete sink;
    }
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
                       new AsioTcpSocketAddressImpl(ioService->service(), endpoint));

    return localAddress;
}

const SocketAddress& AsioServerSocketChannel::getRemoteAddress() const {
    return SocketAddress::NULL_ADDRESS;
}

bool AsioServerSocketChannel::setClosed() {
    LOG_INFO(logger, "Server Channel set closed.");
    return AbstractChannel::setClosed();
}

void AsioServerSocketChannel::doBind(const SocketAddress& localAddress) {
    bool bound = false;
    bool bossStarted = false;

    try {
        boost::asio::ip::tcp::endpoint ep(
            boost::asio::ip::address::from_string(localAddress.address()),
            localAddress.port());

        if (localAddress.family() == IpAddress::IPv4) {
            acceptor.open(boost::asio::ip::tcp::v4());
            LOG_INFO(logger, "the server channel (acceptor) opened in IPV4 mode.");
        }
        else {
            acceptor.open(boost::asio::ip::tcp::v6());
            LOG_INFO(logger, "the server channel (acceptor) opened in IPV6 mode.");
        }

        config.setReuseAddress(acceptor);
        config.setReceiveBufferSize(acceptor);

        acceptor.bind(ep);

        const boost::optional<int> backlog = config.getBacklog();

        if (backlog) {
            acceptor.listen(*backlog);
        }
        else {
            acceptor.listen();
        }

        bound = true;
        //Channels::fireChannelBound(channel, channel->getLocalAddress());

        accept();
        bossStarted = true;

        this->active = true;
        //future->setSuccess();
    }
    catch (const std::exception& e) {
        //Exception exception(e.what());
        //future->setFailure(exception);
        //Channels::fireExceptionCaught(channel, exception);
    }

    if (!bossStarted && bound) {
        doClose();
    }
}

void AsioServerSocketChannel::accept() {
    ChannelPipelinePtr pipeline =
        ChannelPipelines::pipeline(childPipeline);

    const AsioServicePtr& ioService = childServicePool->getNextService();

    AsioSocketChannelPtr c =
        new AsioSocketChannel(shared_from_this(),
                              ioService,
                              factory,
                              pipeline);

    acceptor.async_accept(c->getSocket(),
                          makeCustomAllocHandler(acceptAllocator,
                                  boost::bind(&AsioServerSocketChannel::handleAccept,
                                          this,
                                          boost::asio::placeholders::error,
                                          c)));
}

void AsioServerSocketChannel::handleAccept(const boost::system::error_code& error,
        AsioSocketChannelPtr channel) {
    BOOST_ASSERT(channel);

    if (!error) {
        //pipeline->
        // create the socket add it to the buffer and fire the event
        //channel.pipeline().inboundMessageBuffer().add(
        //    new AioSocketChannel(channel, null, channel.eventLoop, ch));
        pipeline->addInboundMessage<ChannelPtr>(boost::static_pointer_cast<Channel>(channel));
        pipeline->fireMessageUpdated();

        channel->beginRead();

        childChannels.insert(
            std::make_pair<int, ChannelPtr>(channel->getId(), channel));

        ChannelPipelinePtr pipeline =
            ChannelPipelines::pipeline(childPipeline);

        const AsioServicePtr& ioService = childServicePool->getNextService();
        AsioSocketChannelPtr newChannel = new AsioSocketChannel(
            shared_from_this(),
            ioService,
            factory,
            pipeline);

        acceptor.async_accept(newChannel->getSocket(),
                              makeCustomAllocHandler(acceptAllocator,
                                      boost::bind(&AsioServerSocketChannel::handleAccept,
                                              this,
                                              boost::asio::placeholders::error,
                                              newChannel)));
    }
    else {
        LOG_WARN(logger, "Failed to accept a connection any more. ErrorCode: %d.", error.value());
    }
}

void AsioServerSocketChannel::doClose() {
    boost::system::error_code error;

    if (acceptor.is_open()) {
        acceptor.close(error);

        if (error) {
            LOG_ERROR(logger, "failed to close acceptor, error code:%d, msg:%s.", error.value(), error.message().c_str());
            ChannelException e(error.message(), error.value());
            //future->setFailure(e);

            LOG_INFO(logger, "firing the above exception event.");
            //Channels::fireExceptionCaught(channel, e);
        }
    }

    //future->setSuccess();

    if (getCloseFuture()->isDone()) {
        LOG_INFO(logger, "closing the server channel, but the channel has been closed yet.");
    }

    //close all children Channels
    while (!childChannels.empty()) {
        const ChannelPtr& child = childChannels.begin()->second;

        if (child) {
            child->close(child->getCloseFuture());
        }
        else {
            childChannels.erase(childChannels.begin());
        }
    }
}

void AsioServerSocketChannel::doDisconnect() {

}

ChannelSink& AsioServerSocketChannel::getSink() {
    BOOST_ASSERT(sink);
    return *sink;
}

bool AsioServerSocketChannel::isActive() const {
    return this->active;
}

}
}
}
}
