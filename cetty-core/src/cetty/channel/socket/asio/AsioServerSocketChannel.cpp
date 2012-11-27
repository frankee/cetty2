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

#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelines.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/socket/asio/AsioSocketChannel.h>
#include <cetty/channel/socket/asio/AsioService.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioSocketAddressImpl.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;

AsioServerSocketChannel::AsioServerSocketChannel(
    const EventLoopPtr& eventLoop,
    const ChannelFactoryPtr& factory,
    const ChannelPipelinePtr& pipeline,
    const ChannelPipelinePtr& childPipeline,
    const EventLoopPoolPtr& childEventLoopPool)
    : ServerSocketChannel(eventLoop, factory, pipeline),
      ioService(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      acceptor(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      childPipeline(childPipeline),
      childServicePool(boost::dynamic_pointer_cast<AsioServicePool>(childEventLoopPool)),
      config(acceptor),
      ipFamily(IpAddress::IPv4) {

    AbstractChannel::setPipeline(pipeline);

    boost::system::error_code ec;

    if (ipFamily == IpAddress::IPv4) {
        acceptor.open(boost::asio::ip::tcp::v4(), ec);

        if (ec) {
            LOG_ERROR << "the server channel (acceptor) opened in IPV4 mode failed.";
        }
        else {
            LOG_INFO << "the server channel (acceptor) opened in IPV4 mode default.";
        }
    }
    else {
        acceptor.open(boost::asio::ip::tcp::v6(), ec);

        if (ec) {
            LOG_ERROR << "the server channel (acceptor) opened in IPV6 mode failed.";
        }
        else {
            LOG_INFO << "the server channel (acceptor) opened in IPV6 mode default.";
        }
    }
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
                       new AsioTcpSocketAddressImpl(ioService->service(),
                               endpoint));

    return localAddress;
}

const SocketAddress& AsioServerSocketChannel::getRemoteAddress() const {
    return SocketAddress::NULL_ADDRESS;
}

bool AsioServerSocketChannel::setClosed() {
    LOG_INFO << "Server Channel set closed.";
    return AbstractChannel::setClosed();
}

void AsioServerSocketChannel::doBind(const SocketAddress& localAddress) {
    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string(localAddress.address()),
        localAddress.port());

    if (localAddress.family() != ipFamily) {
        acceptor.close(ec);

        if (ec) {
            LOG_ERROR << "close the acceptor before change ip family, but skip it.";
        }

        acceptor.open(ep.protocol(), ec);

        if (ec) {
            LOG_ERROR << "failed to reopen the acceptor in different ip family.";
            return doClose();
        }
        else {
            LOG_INFO << "the server channel (acceptor) changed to open in IPV6 mode.";
        }
    }

    config.setReuseAddress(acceptor);
    config.setReceiveBufferSize(acceptor);

    acceptor.bind(ep, ec);

    if (ec) {
        LOG_ERROR << "the server channel (acceptor) can not bind to the " << localAddress.toString();
        return doClose();
    }

    const boost::optional<int> backlog = config.getBacklog();

    if (backlog) {
        acceptor.listen(*backlog, ec);
    }
    else {
        acceptor.listen(tcp::acceptor::max_connections, ec);
    }

    if (ec) {
        LOG_ERROR << "the server channel (acceptor) can not listen the " << localAddress.toString();
        return doClose();
    }

    accept();

    // start the event loop pool if in main thread mode.
    const EventLoopPoolPtr& loop = ioService->getEventLoopPool();

    if (loop && loop->isMainThread()) {
        LOG_INFO << "the asio service pool starting to run in main thread.";

        if (!loop->start()) {
            LOG_ERROR << "start the boost asio service error, stop service pool and close channel.";
            loop->stop();
            getPipeline()->fireExceptionCaught(
                ChannelException("failed to start the asio service in main thread."));

            return doClose();
        }
        else {
            LOG_INFO << "the asio service pool started to running.";
        }
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

    LOG_INFO << "AsioSocketChannel firing the Channel Open Event.";
    c->getPipeline()->fireChannelOpen();

    acceptor.async_accept(c->getSocket(),
                          makeCustomAllocHandler(acceptAllocator,
                                  boost::bind(&AsioServerSocketChannel::handleAccept,
                                          this,
                                          boost::asio::placeholders::error,
                                          c)));
}

void AsioServerSocketChannel::handleAccept(const boost::system::error_code& error,
        const AsioSocketChannelPtr& channel) {
    BOOST_ASSERT(channel);

    if (!error) {
        // create the socket add it to the buffer and fire the event
        pipeline->addInboundMessage<ChannelPtr>(
            boost::static_pointer_cast<Channel>(channel));

        pipeline->fireMessageUpdated();

        //childChannels.insert(
        //    std::make_pair<int, ChannelPtr>(channel->getId(), channel));

        //channel->getCloseFuture()->addListener(boost::bind(
        //    &AsioServerSocketChannel::handleChildClosed,
        //    this,
        //    _1),
        //    100);

        channel->getPipeline()->fireChannelActive();
        channel->beginRead();

        ChannelPipelinePtr pipeline =
            ChannelPipelines::pipeline(childPipeline);

        const AsioServicePtr& ioService = childServicePool->getNextService();
        AsioSocketChannelPtr newChannel = new AsioSocketChannel(
            shared_from_this(),
            ioService,
            factory,
            pipeline);

        LOG_INFO << "AsioSocketChannel firing the Channel Open Event.";
        newChannel->getPipeline()->fireChannelOpen();

        acceptor.async_accept(newChannel->getSocket(),
                              makeCustomAllocHandler(acceptAllocator,
                                      boost::bind(&AsioServerSocketChannel::handleAccept,
                                              this,
                                              boost::asio::placeholders::error,
                                              newChannel)));
    }
    else {
        LOG_ERROR << "Failed to accept a connection any more. ErrorCode:" << error.value();
    }
}

void AsioServerSocketChannel::doClose() {
    boost::system::error_code error;

    if (acceptor.is_open()) {
        acceptor.close(error);

        if (error) {
            LOG_ERROR << "failed to close acceptor, error:"
                      << error.value() << ":" << error.message();
        }
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
    // NOOP
}

bool AsioServerSocketChannel::isActive() const {
    return acceptor.is_open() && localAddress.validated();
}

bool AsioServerSocketChannel::isOpen() const {
    return acceptor.is_open();
}

void AsioServerSocketChannel::handleChildClosed(const ChannelFuture& future) {
    if (eventLoop_->inLoopThread()) {
        childChannels.erase(future.getChannel()->getId());
    }
    else {
        eventLoop_->post(boost::bind(&AsioServerSocketChannel::handleChildClosed,
                                    this,
                                    boost::cref(future)));
    }
}

}
}
}
}
