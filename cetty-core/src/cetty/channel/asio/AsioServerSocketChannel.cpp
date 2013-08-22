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
      addressFamily_(InetAddress::FAMILY_IPv4),
      ioService_(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      acceptor_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      serverConfig_(acceptor_),
      childServicePool_(boost::dynamic_pointer_cast<AsioServicePool>(childEventLoopPool)) {
    boost::system::error_code ec;

    if (addressFamily_ == InetAddress::FAMILY_IPv4) {
        LOG_INFO << "server channel (acceptor) opened in IPV4 mode default.";
        acceptor_.open(boost::asio::ip::tcp::v4(), ec);

        if (ec) {
            LOG_ERROR << "the server channel (acceptor) opened in IPV4 mode failed.";
        }
    }
    else {
        LOG_INFO << "the server channel (acceptor) opened in IPV6 mode default.";
        acceptor_.open(boost::asio::ip::tcp::v6(), ec);

        if (ec) {
            LOG_ERROR << "the server channel (acceptor) opened in IPV6 mode failed.";
        }
    }
}

AsioServerSocketChannel::~AsioServerSocketChannel() {
}

bool AsioServerSocketChannel::doBind(const InetAddress& localAddress) {
    const std::string& host = localAddress.host();

    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string(host),
        localAddress.port());

    if (localAddress.family() != addressFamily_) {
        acceptor_.close(ec);

        if (ec) {
            LOG_ERROR << "failed to close the acceptor before change ip family,"
                      " but skip it.";
        }

        acceptor_.open(ep.protocol(), ec);

        if (!ec) {
            LOG_INFO << "the server channel (acceptor) changed to open in IPV6 mode.";
        }
        else {
            LOG_ERROR << "failed to reopen the acceptor in different ip family.";
            doClose();
            return false;
        }

        const boost::optional<bool>& isReuseAddress =
            serverConfig_.isReuseAddress();

        if (isReuseAddress) {
            serverConfig_.setReuseAddress(*isReuseAddress);
        }

        const boost::optional<int>& receiveBufferSize =
            serverConfig_.receiveBufferSize();

        if (receiveBufferSize) {
            serverConfig_.setReceiveBufferSize(*receiveBufferSize);
        }
    }

    acceptor_.bind(ep, ec);

    if (ec) {
        LOG_ERROR << "the server channel (acceptor) can not bind to the "
                  << localAddress.toString();
        doClose();
        return false;
    }

    const boost::optional<int>& backlog = serverConfig_.backlog();

    if (backlog) {
        acceptor_.listen(*backlog, ec);
    }
    else {
        acceptor_.listen(tcp::acceptor::max_connections, ec);
    }

    if (ec) {
        LOG_ERROR << "the server channel (acceptor) can not listen the "
                  << localAddress.toString();
        doClose();
        return false;
    }

    // initialize the reserved child channels, for speeding up accepting
    if (serverConfig_.reservedChildCount()) {
        int reservedChildCount = *serverConfig_.reservedChildCount();

        while (reservedChildCount > 0) {
            reusableChildChannels_.push_back(createChild());
            --reservedChildCount;
        }
    }

    accept();

    // start the event loop pool if in main thread mode.
    const EventLoopPoolPtr& loop = ioService_->eventLoopPool();

    if (loop && loop->isSingleThread()) {
        LOG_INFO << "the asio service pool starting to run in main thread.";

        if (!loop->start()) {
            LOG_ERROR << "start the boost asio service error,"
                      " stop service pool and close channel.";
            loop->stop();
            pipeline().fireExceptionCaught(
                ChannelException("failed to start the asio service in main thread."));

            return doClose();
        }
        else {
            LOG_INFO << "the asio service pool started to running.";
        }
    }

    LOG_INFO << "server channel " << toString()
             << " has bound to "  << localAddress.toString();
    return true;
}

void AsioServerSocketChannel::accept() {
    AsioSocketChannelPtr c = createChild();
    acceptor_.async_accept(c->tcpSocket(),
                           makeCustomAllocHandler(acceptAllocator_,
                                   boost::bind(&AsioServerSocketChannel::handleAccept,
                                           this,
                                           boost::asio::placeholders::error,
                                           c)));
    LOG_INFO << "server channel " << toString()
             << " begin to accepting socket.";
}

void AsioServerSocketChannel::handleAccept(const boost::system::error_code& error,
        const AsioSocketChannelPtr& channel) {
    BOOST_ASSERT(channel);

    if (!error) {
        ChannelPtr acceptedChannel = boost::static_pointer_cast<Channel>(channel);

        // create the socket add it to the buffer and fire the event
        pipeline().addInboundChannelMessage<ChannelPtr>(acceptedChannel);
        pipeline().fireMessageUpdated();

        childChannels_.insert(std::make_pair(channel->id(), channel));
        channel->closeFuture()->addListener(boost::bind(
                                                &AsioServerSocketChannel::handleChildClosed,
                                                this,
                                                _1),
                                            100);
        channel->open();
        channel->setActived();
        LOG_INFO << "server channel " << toString()
                 << "accepted a new channel " << channel->toString();

        channel->beginRead();

        AsioSocketChannelPtr newChannel = createChild();
        acceptor_.async_accept(newChannel->tcpSocket(),
                               makeCustomAllocHandler(acceptAllocator_,
                                       boost::bind(
                                           &AsioServerSocketChannel::handleAccept,
                                           this,
                                           boost::asio::placeholders::error,
                                           newChannel)));
    }
    else {
        LOG_ERROR << "server channel " << toString()
                  << "failed to accept a connection any more. ErrorCode: "
                  << error.value()
                  << ", Message: " << error.message();
    }
}

bool AsioServerSocketChannel::doClose() {
    boost::system::error_code error;

    if (acceptor_.is_open()) {
        acceptor_.close(error);

        if (error) {
            LOG_ERROR << "server channel" << toString()
                      << " failed to close acceptor, error:"
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

    return true;
}

bool AsioServerSocketChannel::doDisconnect() {
    return true; // NOOP
}

void AsioServerSocketChannel::handleChildClosed(const ChannelFuture& future) {
    if (eventLoop()->inLoopThread()) {
        ChildChannels::iterator itr =
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
        const boost::optional<bool>& reuseChild = serverConfig_.isReuseChild();

        // only if reuseChild set to false, will not recycle the channel.
        if (!reuseChild || *reuseChild) {
            reusableChildChannels_.push_back(itr->second);
            LOG_INFO << "the channel: " << ch->id()
                     << " push to reusable pool.";
        }

        childChannels_.erase(itr);
    }
    else {
        eventLoop()->post(boost::bind(&AsioServerSocketChannel::handleChildClosed,
                                      this,
                                      boost::cref(future)));
    }
}

void AsioServerSocketChannel::doPreOpen() {
    if (!initialized_) {
        Channel::config().setOptionSetCallback(boost::bind(
                &AsioServerSocketChannelConfig::setOption,
                &serverConfig_,
                _1,
                _2));

        pipeline().setHead<AsioServerSocketChannel*>("head", this);

        initialized_ = true;
    }
}

AsioSocketChannelPtr AsioServerSocketChannel::createChild() {
    if (!reusableChildChannels_.empty()) {
        AsioSocketChannelPtr newChannel = reusableChildChannels_.front();
        reusableChildChannels_.pop_front();

        return newChannel;
    }
    else {
        const AsioServicePtr& ioService = childServicePool_->nextService();
        return AsioSocketChannelPtr(
                   new AsioSocketChannel(++lastChildId_,
                                         shared_from_this(),
                                         ioService));
    }
}

}
}
}
