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

#include <cetty/channel/asio/AsioServerSocketChannel.h>

#include <cetty/logging/LoggerHelper.h>

#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/asio/AsioService.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/channel/asio/AsioSocketChannel.h>
#include <cetty/channel/asio/AsioSocketAddressImpl.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace cetty::channel;

AsioServerSocketChannel::AsioServerSocketChannel(
    const EventLoopPtr& eventLoop,
    const EventLoopPoolPtr& childEventLoopPool)
    : Channel(ChannelPtr(), eventLoop),
      initialized_(false),
      lastChildId_(0),
      ioService_(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      acceptor_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      socketConfig_(acceptor_),
      childServicePool_(boost::dynamic_pointer_cast<AsioServicePool>(childEventLoopPool)),
      ipFamily(IpAddress::IPv4) {

    boost::system::error_code ec;

    if (ipFamily == IpAddress::IPv4) {
        acceptor_.open(boost::asio::ip::tcp::v4(), ec);

        if (ec) {
            LOG_ERROR << "the server channel (acceptor) opened in IPV4 mode failed.";
        }
        else {
            LOG_INFO << "the server channel (acceptor) opened in IPV4 mode default.";
        }
    }
    else {
        acceptor_.open(boost::asio::ip::tcp::v6(), ec);

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

// const SocketAddress& AsioServerSocketChannel::getLocalAddress() const {
//     if (localAddress != SocketAddress::NULL_ADDRESS) {
//         return localAddress;
//     }
//
//     boost::system::error_code ec;
//     boost::asio::ip::tcp::endpoint endpoint = acceptor_.local_endpoint(ec);
//
//     if (ec) {
//         return SocketAddress::NULL_ADDRESS;
//     }
//
//     localAddress = SocketAddress(
//                        new AsioTcpSocketAddressImpl(ioService_->service(),
//                                endpoint));
//
//     return localAddress;
// }

void AsioServerSocketChannel::doBind(const SocketAddress& localAddress) {
    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string(localAddress.address()),
        localAddress.port());

    if (localAddress.family() != ipFamily) {
        acceptor_.close(ec);

        if (ec) {
            LOG_ERROR << "close the acceptor before change ip family, but skip it.";
        }

        acceptor_.open(ep.protocol(), ec);

        if (ec) {
            LOG_ERROR << "failed to reopen the acceptor in different ip family.";
            return doClose();
        }
        else {
            LOG_INFO << "the server channel (acceptor) changed to open in IPV6 mode.";
        }
    }

    socketConfig_.setReuseAddress(acceptor_);
    socketConfig_.setReceiveBufferSize(acceptor_);

    acceptor_.bind(ep, ec);

    if (ec) {
        LOG_ERROR << "the server channel (acceptor) can not bind to the " << localAddress.toString();
        return doClose();
    }

    const boost::optional<int> backlog = socketConfig_.getBacklog();

    if (backlog) {
        acceptor_.listen(*backlog, ec);
    }
    else {
        acceptor_.listen(tcp::acceptor::max_connections, ec);
    }

    if (ec) {
        LOG_ERROR << "the server channel (acceptor) can not listen the " << localAddress.toString();
        return doClose();
    }

    accept();

    // start the event loop pool if in main thread mode.
    const EventLoopPoolPtr& loop = ioService_->eventLoopPool();

    if (loop && loop->isMainThread()) {
        LOG_INFO << "the asio service pool starting to run in main thread.";

        if (!loop->start()) {
            LOG_ERROR << "start the boost asio service error, stop service pool and close channel.";
            loop->stop();
            pipeline().fireExceptionCaught(
                ChannelException("failed to start the asio service in main thread."));

            return doClose();
        }
        else {
            LOG_INFO << "the asio service pool started to running.";
        }
    }
}

void AsioServerSocketChannel::accept() {
    const AsioServicePtr& ioService = childServicePool_->getNextService();

    AsioSocketChannelPtr c(new AsioSocketChannel(++lastChildId_,
                           shared_from_this(),
                           ioService));

    acceptor_.async_accept(c->tcpSocket(),
                           makeCustomAllocHandler(acceptAllocator_,
                                   boost::bind(&AsioServerSocketChannel::handleAccept,
                                           this,
                                           boost::asio::placeholders::error,
                                           c)));
}

void AsioServerSocketChannel::handleAccept(const boost::system::error_code& error,
        const AsioSocketChannelPtr& channel) {
    BOOST_ASSERT(channel);

    if (!error) {
        ChannelPtr acceptedChannel = boost::static_pointer_cast<Channel>(channel);
        // create the socket add it to the buffer and fire the event
        pipeline().addInboundChannelMessage<ChannelPtr>(acceptedChannel);
        pipeline().fireMessageUpdated();

        LOG_INFO << "AsioSocketChannel firing the Channel Open Event.";
        channel->initialize();

        childChannels_.insert(
            std::make_pair<int, AsioSocketChannelPtr>(channel->id(),
                    channel));

        channel->closeFuture()->addListener(boost::bind(
                                                &AsioServerSocketChannel::handleChildClosed,
                                                this,
                                                _1),
                                            100);

        channel->pipeline().fireChannelActive();
        channel->beginRead();

        AsioSocketChannelPtr newChannel;

        if (!reusableChildChannels_.empty()) {
            newChannel = reusableChildChannels_.front();
            reusableChildChannels_.pop_front();
        }
        else {
            const AsioServicePtr& ioService = childServicePool_->getNextService();
            newChannel = AsioSocketChannelPtr(new AsioSocketChannel(
                                                  ++lastChildId_,
                                                  shared_from_this(),
                                                  ioService));
        }

        acceptor_.async_accept(newChannel->tcpSocket(),
                               makeCustomAllocHandler(acceptAllocator_,
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

    if (acceptor_.is_open()) {
        acceptor_.close(error);

        if (error) {
            LOG_ERROR << "failed to close acceptor, error:"
                      << error.value() << ":" << error.message();
        }
    }

    //close all children Channels
    while (!childChannels_.empty()) {
        const ChannelPtr& child = childChannels_.begin()->second;

        if (child) {
            child->close(child->closeFuture());
        }
        else {
            childChannels_.erase(childChannels_.begin());
        }
    }

    closeFuture()->setSuccess();
}

void AsioServerSocketChannel::doDisconnect() {
    // NOOP
}

bool AsioServerSocketChannel::isActive() const {
    return acceptor_.is_open() && localAddress.validated();
}

bool AsioServerSocketChannel::isOpen() const {
    return acceptor_.is_open();
}

void AsioServerSocketChannel::handleChildClosed(const ChannelFuture& future) {
    if (eventLoop()->inLoopThread()) {
        ChildChannels::const_iterator itr =
            childChannels_.find(future.channel()->id());

#if !defined(NDEBUG)
        AsioSocketChannelPtr ch = itr->second;
        LOG_INFO << "the channel : " << ch->id()
                 << " 's use count is " << ch.use_count();

        if (ch.use_count() > 2) {
            LOG_ERROR << "the channel : " << ch->id()
                      << " has not closed properly.";
        }
#endif

        reusableChildChannels_.push_back(itr->second);
        childChannels_.erase(itr);
    }
    else {
        eventLoop()->post(boost::bind(&AsioServerSocketChannel::handleChildClosed,
                                      this,
                                      boost::cref(future)));
    }
}

void AsioServerSocketChannel::doInitialize() {
    if (!initialized_) {
        Channel::config().setSetOptionCallback(boost::bind(
                &AsioServerSocketChannelConfig::setOption,
                &socketConfig_,
                _1,
                _2));

        pipeline().setHead<AsioServerSocketChannel*>("bridge", this);

        initialized_ = true;
    }
}

}
}
}
