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
#include <cetty/channel/InetAddress.h>
#include <cetty/channel/NullChannel.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelInitializer.h>
#include <cetty/channel/ChannelFutureListener.h>
#include <cetty/channel/ChannelMessageHandlerContext.h>

#include <cetty/util/Exception.h>
#include <cetty/logging/LoggerHelper.h>

#include <cetty/channel/asio/AsioService.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/channel/asio/AsioServerSocketChannel.h>

#include <cetty/bootstrap/ServerUtil.h>

namespace cetty {
namespace bootstrap {

using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::channel::asio;

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
          inboundContainer_() {
    }

    ~Acceptor() {}

    void registerTo(Context& ctx) {
        inboundContainer_ = ctx.inboundContainer();

        ctx.setChannelMessageUpdatedCallback(boost::bind(
                &Acceptor::messageUpdated,
                this,
                _1));
    }

    void messageUpdated(ChannelHandlerContext& ctx) {
        InboundQueue& inboundQueue = inboundContainer_->getMessages();

        while (!inboundQueue.empty()) {
            const ChannelPtr& child = inboundQueue.front();

            if (!child) {
                break;
            }

            child->setInitializer(bootstrap_.childInitializer());
            child->open();

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
    InboundContainer* inboundContainer_;
};

ServerBootstrap::ServerBootstrap()
    : daemonized_(false),
      parentHandler_() {
}

ServerBootstrap::ServerBootstrap(const EventLoopPoolPtr& pool)
    : Bootstrap<ServerBootstrap>(pool),
      daemonized_(false),
      parentHandler_() {
    setChildEventLoopPool(pool);
}

ServerBootstrap::ServerBootstrap(const EventLoopPoolPtr& parent,
                                 const EventLoopPoolPtr& child)
    : Bootstrap<ServerBootstrap>(parent),
      daemonized_(false),
      parentHandler_() {
    if (child) {
        setChildEventLoopPool(child);
        LOG_WARN << "set null EventLoopPool to child, using parent.";
    }
    else {
        setChildEventLoopPool(parent);
    }
}

ServerBootstrap::ServerBootstrap(int parentThreadCnt, int childThreadCnt)
    : daemonized_(false),
      parentHandler_() {
    setParentEventLoopPool(new AsioServicePool(parentThreadCnt));

    if (childThreadCnt > 0) {
        setChildEventLoopPool(new AsioServicePool(childThreadCnt));
    }
}

ServerBootstrap& ServerBootstrap::setParentEventLoopPool(
    const EventLoopPoolPtr& pool) {
    if (pool) {
        if (!childPool_) {
            childPool_ = pool;
            LOG_INFO << "child pool has not been set,"
                     " using the same pool with parent";
        }

        Bootstrap<ServerBootstrap>::setEventLoopPool(pool);
    }
    else {
        LOG_WARN << "setting the NULL EventLoopPool, skip it";
    }

    return *this;
}

ChannelFuturePtr ServerBootstrap::bind() {
    return bind(localAddress());
}

ChannelFuturePtr ServerBootstrap::bind(int port) {
    return bind(InetAddress(InetAddress::FAMILY_IPv4, port));
}

ChannelFuturePtr ServerBootstrap::bind(const std::string& ip, int port) {
    return bind(InetAddress(ip, port));
}

ChannelFuturePtr ServerBootstrap::bind(const InetAddress& localAddress) {
    // bossPipeline's life cycle will be managed by the server channel.
    ChannelPtr channel = newChannel();

    if (!channel) {
        LOG_WARN << "Server channel factory failed to create a new channel.";
        return NullChannel::instance()->newFailedFuture(
                   ChannelException("Failed to create a new channel."));
    }

    channel->setInitializer(boost::bind(
                                &ServerBootstrap::initServerChannel,
                                this,
                                _1));

    channel->open();

    if (channel->isOpen()) {
        // Set the options.
        channel->config().setOptions(channelOptions());

        insertChannel(channel->id(), channel);

        ChannelFuturePtr future = channel->newFuture();
        channel->bind(localAddress, future)->addListener(
            ChannelFutureListener::CLOSE_ON_FAILURE);

        return future;
    }
    else {
        LOG_WARN << "failed to open a server channel.";
        return NullChannel::instance()->newFailedFuture(
            ChannelException("failed to open a server channel"));
    }
}

void ServerBootstrap::shutdown() {
    EventLoopPoolPtr serverPool = eventLoopPool();

    if (serverPool) {
        serverPool->stop();
        serverPool->waitingForStop();
    }

    childPool_->stop();
    childPool_->waitingForStop();

    clearChannels();

    if (parentHandler_) {
        delete parentHandler_;
    }
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

ChannelPtr ServerBootstrap::newChannel() {
    const EventLoopPoolPtr& parent = eventLoopPool();

    if (parent) {
        if (boost::dynamic_pointer_cast<AsioServicePool>(parent)) {
            return ChannelPtr(
                       new AsioServerSocketChannel(parent->nextLoop(),
                                                   childLoopPool()));
        }
        else {
            BOOST_ASSERT(false && "has not implement yet.");
            return ChannelPtr();
        }
    }
    else {
        LOG_WARN << "has not set the parent EventLoopPool.";
        return ChannelPtr();
    }
}

void ServerBootstrap::waitingForExit() {
    const Channels& serverChannels = channels();

    if (daemonized_) {
        ServerUtil::createPidFile(pidFile_);

        Channels::const_iterator itr = serverChannels.begin();

        for (; itr != serverChannels.end(); ++itr) {
            itr->second->closeFuture()->awaitUninterruptibly();
        }
    }
    else {
        printf("Servers: \n");
        Channels::const_iterator itr = serverChannels.begin();

        for (; itr != serverChannels.end(); ++itr) {
            printf("    Channel ID: %d has bind to %s\n",
                   itr->second->id(),
                   itr->second->localAddress().toString().c_str());
        }

        printf("To quit server, press 'q'.\n");

        char input;

        do {
            input = getchar();

            if (input == 'q') {
                break;
            }
        }
        while (true);
    }

    shutdown();
}

}
}
