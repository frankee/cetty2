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

#include <cetty/bootstrap/ServerBootstrap.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/EventLoop.h>
#include <cetty/channel/EventLoopPool.h>
#include <cetty/channel/IpAddress.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/NullChannel.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelInitializer.h>
#include <cetty/channel/ChannelFutureListener.h>
#include <cetty/channel/ChannelMessageHandlerContext.h>


#include <cetty/util/Exception.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace bootstrap {

using namespace cetty::channel;
using namespace cetty::util;

class Acceptor : private boost::noncopyable {
public:
    typedef boost::shared_ptr<Acceptor> Ptr;

    typedef ChannelMessageContainer<ChannelPtr, MESSAGE_BLOCK> InboundContainer;
    typedef InboundContainer::MessageQueue InboundQueue;

    typedef ChannelMessageHandlerContext<Acceptor,
            ChannelPtr,
            VoidMessage,
            VoidMessage,
            VoidMessage,
            InboundContainer,
            VoidMessageContainer,
            VoidMessageContainer,
            VoidMessageContainer> Context;

public:
    Acceptor(ServerBootstrap& bootstrap)
        : bootstrap_(bootstrap),
          context_() {
    }

    ~Acceptor() {}

    void registerTo(Context& ctx) {
        context_ = &ctx;

        ctx.setChannelMessageUpdatedCallback(boost::bind(
                &Acceptor::messageUpdated,
                this,
                _1));
    }

    void messageUpdated(ChannelHandlerContext& ctx) {
        InboundQueue& inboundQueue = context_->inboundContainer()->getMessages();

        while (!inboundQueue.empty()) {
            const ChannelPtr& child = inboundQueue.front();

            if (!child) {
                break;
            }

            child->setInitializer(bootstrap_.childInitializer());

            const ChannelOptions& childOptions =
                bootstrap_.childOptions();

            ChannelOptions::ConstIterator itr = childOptions.begin();

            for (; itr != childOptions.end(); ++itr) {
                if (!child->config().setOption(itr->first, itr->second)) {
                    LOG_WARN << "set " << itr->first.name() << " failed.";
                }
            }

            inboundQueue.pop_front();
        }
    }

private:
    ServerBootstrap& bootstrap_;
    Context* context_;
};

ChannelFuturePtr ServerBootstrap::bind(int port) {
    return bind(SocketAddress(IpAddress::IPv4, port));
}

bool ServerBootstrap::initServerChannel(const ChannelPtr& channel) {
    ChannelPipeline& pipeline = channel->pipeline();

    pipeline.addLast<Acceptor>("acceptor",
                               Acceptor::Ptr(new Acceptor(*this)));

    if (parentHandler_) {
        pipeline.addLast(parentHandler_);
    }

    return true;
}

ChannelFuturePtr ServerBootstrap::bind(const SocketAddress& localAddress) {
    // bossPipeline's life cycle will be managed by the server channel.
    ChannelPtr channel = newChannel();

    if (!channel) {
        LOG_ERROR << "Server channel factory failed to create a new channel.";
        return NullChannel::instance()->newFailedFuture(
                   ChannelException("Failed to create a new channel."));
    }

    channel->setInitializer(boost::bind(
                                &ServerBootstrap::initServerChannel,
                                this,
                                _1));

    channel->initialize();

    if (channel->isOpen()) {
        LOG_INFO << "Opened the AsioServerSocketChannel.";
        serverChannels_.push_back(channel);
    }

    ChannelFuturePtr future = channel->newFuture();
    channel->bind(localAddress, future)->addListener(ChannelFutureListener::CLOSE_ON_FAILURE);

    return future;
}

cetty::channel::ChannelFuturePtr ServerBootstrap::bind() {
    return bind(localAddress());
}

cetty::channel::ChannelFuturePtr ServerBootstrap::bind(const std::string& ip, int port) {
    return bind(SocketAddress(ip, port));
}

ServerBootstrap& ServerBootstrap::setChildOptions(const ChannelOptions& options) {
    childOptions_ = options;
    return *this;
}

ServerBootstrap& ServerBootstrap::setChildOption(const ChannelOption& option,
        const ChannelOption::Variant& value) {
    childOptions_.setOption(option, value);
    return *this;
}

void ServerBootstrap::shutdown() {
    EventLoopPoolPtr serverPool = eventLoopPool();

    if (serverPool) {
        serverPool->stop();
        serverPool->waitForStop();
    }

    childPool_->stop();
    childPool_->waitForStop();

    serverChannels_.clear();

    if (parentHandler_) {
        delete parentHandler_;
    }
}

ServerBootstrap::ServerBootstrap() : parentHandler_() {

}

ServerBootstrap::ServerBootstrap(const EventLoopPoolPtr& pool) : AbstractBootstrap<ServerBootstrap>(pool),
    parentHandler_() {
        setChildEventLoopPool(pool);
}

ServerBootstrap::ServerBootstrap(const EventLoopPoolPtr& parent, const EventLoopPoolPtr& child) : AbstractBootstrap<ServerBootstrap>(parent),
    parentHandler_() {
        if (child) {
            setChildEventLoopPool(child);
            LOG_WARN << "set null EventLoopPool to child, using parent.";
        }
        else {
            setChildEventLoopPool(parent);
        }
}

ServerBootstrap& ServerBootstrap::setEventLoopPools(const EventLoopPoolPtr& parent, const EventLoopPoolPtr& child) {
    if (child) {
        childPool_ = child;
    }
    else {
        childPool_ = parent;
    }

    AbstractBootstrap<ServerBootstrap>::setEventLoopPool(parent);

    return *this;
}

}
}
