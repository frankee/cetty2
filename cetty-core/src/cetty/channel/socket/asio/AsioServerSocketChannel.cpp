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

void AsioServerSocketChannel::writeRequested(
    const ChannelPipeline& pipeline,
    const MessageEvent& e) {
        const ChannelPtr& channel = e.getChannel();
        (static_cast<AsioSocketChannel*>(channel))->internalWrite(e);
}

void AsioServerSocketChannel::stateChangeRequested(
    const ChannelPipeline& pipeline,
    const ChannelStateEvent& e) {
        const ChannelPtr& channel = e.getChannel();

        if (channel->getParent()) {
            handleAcceptChannelStateChange(channel, e);
        }
        else {
            handleServerChannelStateChange(channel, e);
        }
}

void AsioServerSocketChannel::handleServerChannelStateChange(
    const ChannelPtr& channel,
    const ChannelStateEvent& evt) {
        const ChannelFuturePtr& future = evt.getFuture();
        const ChannelState& state = evt.getState();
        const boost::any& value = evt.getValue();

        if (state == ChannelState::OPEN) {
            if (value.empty()) {
                closeServerChannel(channel, future);
            }
        }
        else if (state == ChannelState::BOUND) {
            if (value.empty()) {
                closeServerChannel(channel, future);
            }
            else {
                const SocketAddress* address = boost::any_cast<SocketAddress>(&value);

                if (address) {
                    bind(channel, future, *address);
                }
                else {
                    closeServerChannel(channel, future);
                }
            }
        }
}

void AsioServerSocketChannel::handleAcceptChannelStateChange(
    const ChannelPtr& channel,
    const ChannelStateEvent& evt) {
        const ChannelFuturePtr& future = evt.getFuture();
        const ChannelState& state = evt.getState();
        const boost::any& value = evt.getValue();

        if (ChannelState::INTEREST_OPS == state) {
            AsioSocketChannel* socketChannel = static_cast<AsioSocketChannel*>(channel);
            int v = ConversionUtil::toInt(value);
            socketChannel->internalSetInterestOps(future, v);
        }
        else {
            // when AsioAcceptedSocketChannel started, it has connected. So it will has
            // no more OPEN, BOUND, CONNECTED event, but only CLOSE, UNBOUND, DISCONNECTED event.
            if (value.empty()) {
                closeAcceptChannel(channel, future);
            }
        }
}

void AsioServerSocketChannel::bind(const ChannelPtr& channel,
    const ChannelFuturePtr& future,
    const SocketAddress& localAddress) {
        bool bound = false;
        bool bossStarted = false;

        try {
            boost::asio::ip::tcp::endpoint ep(
                boost::asio::ip::address::from_string(localAddress.address()),
                localAddress.port());
            DefaultAsioServerSocketChannelConfig* config =
                dynamic_cast<DefaultAsioServerSocketChannelConfig*>(&channel->getConfig());

            if (localAddress.family() == IpAddress::IPv4) {
                acceptor.open(boost::asio::ip::tcp::v4());
                LOG_INFO(logger, "the server channel (acceptor) opened in IPV4 mode.");
            }
            else {
                acceptor.open(boost::asio::ip::tcp::v6());
                LOG_INFO(logger, "the server channel (acceptor) opened in IPV6 mode.");
            }

            config->setReuseAddress(acceptor);
            config->setReceiveBufferSize(acceptor);

            acceptor.bind(ep);

            const boost::optional<int> backlog = config->getBacklog();

            if (backlog) {
                acceptor.listen(*backlog);
            }
            else {
                acceptor.listen();
            }

            bound = true;
            Channels::fireChannelBound(channel, channel->getLocalAddress());

            accept(channel);
            bossStarted = true;

            future->setSuccess();
        }
        catch (const std::exception& e) {
            Exception exception(e.what());
            future->setFailure(exception);
            Channels::fireExceptionCaught(channel, exception);
        }

        if (!bossStarted && bound) {
            closeServerChannel(channel, future);
        }
}

void AsioServerSocketChannel::accept(const ChannelPtr& channel) {
    const ChannelPipelinePtr& pipeline =
        channel->getConfig().getPipelineFactory()->getPipeline();

    const AsioServicePtr& ioService = ioServicePool->getService();

    AsioAcceptedSocketChannel* c =
        new AsioAcceptedSocketChannel(channel,
        channel->getFactory(),
        pipeline,
        this,
        ioService,
        ioService->getThreadId());

    acceptor.async_accept(c->getSocket(),
        makeCustomAllocHandler(acceptAllocator,
        boost::bind(&AsioServerSocketChannel::handleAccept,
        this,
        boost::asio::placeholders::error,
        ChannelPtr(c),
        channel)));
}

void AsioServerSocketChannel::handleAccept(const boost::system::error_code& error,
    ChannelPtr channel,
    ChannelPtr serverChannel) {
        BOOST_ASSERT(channel && serverChannel);

        if (!error) {
            AsioAcceptedSocketChannel* socketChannel =
                static_cast<AsioAcceptedSocketChannel*>(channel);

            if (!socketChannel->start()) {
                // has no local address or remote address
                // may never happened.
            }

            childChannels.insert(
                std::make_pair<int, ChannelPtr>(channel->getId(), channel));

            const ChannelPipelinePtr& pipeline =
                serverChannel->getConfig().getPipelineFactory()->getPipeline();

            const AsioServicePtr& ioService = ioServicePool->getService();
            AsioAcceptedSocketChannel* newChannel = new AsioAcceptedSocketChannel(
                serverChannel,
                serverChannel->getFactory(),
                pipeline,
                this,
                ioService,
                ioService->getThreadId());

            acceptor.async_accept(newChannel->getSocket(),
                makeCustomAllocHandler(acceptAllocator,
                boost::bind(&AsioServerSocketChannel::handleAccept,
                this,
                boost::asio::placeholders::error,
                ChannelPtr(newChannel),
                serverChannel)));
        }
        else {
            delete channel;
            LOG_WARN(logger, "Failed to accept a connection any more. ErrorCode: %d.", error.value());
        }
}

void AsioServerSocketChannel::closeServerChannel(
    const ChannelPtr& channel,
    const ChannelFuturePtr& future) {
        BOOST_ASSERT(channel);
        boost::system::error_code error;

        if (acceptor.is_open()) {
            acceptor.close(error);

            if (error) {
                LOG_ERROR(logger, "failed to close acceptor, error code:%d, msg:%s.", error.value(), error.message().c_str());
                ChannelException e(error.message(), error.value());
                future->setFailure(e);

                LOG_INFO(logger, "firing the above exception event.");
                Channels::fireExceptionCaught(channel, e);
            }
        }

        future->setSuccess();

        // Make sure the boss thread is not running so that that the future
        // is notified after a new connection cannot be accepted anymore.
        // See NETTY-256 for more information.
        AsioServerSocketChannel* c = static_cast<AsioServerSocketChannel*>(channel);

        if (!c->getCloseFuture()->isDone()) {
            c->setClosed();
        }
        else {
            LOG_INFO(logger, "closing the server channel, but the channel has been closed yet.");
        }

        //close all children Channels
        while (!childChannels.empty()) {
            const ChannelPtr& child = childChannels.begin()->second;

            if (child) {
                closeAcceptChannel(child, child->getCloseFuture());
            }
            else {
                childChannels.erase(childChannels.begin());
            }
        }
}

void AsioServerSocketChannel::closeAcceptChannel(
    const ChannelPtr& channel,
    const ChannelFuturePtr& future) {

        AsioSocketChannel* c = static_cast<AsioSocketChannel*>(channel);
        c->doClose(future);

        //TODO should make sure thread safe, post to accept servicePtr
        //delete the channel.
        ChildChannels::iterator itr = childChannels.find(channel->getId());

        if (itr != childChannels.end()) {
            childChannels.erase(itr);
        }
}

}
}
}
}