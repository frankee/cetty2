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
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ServerChannelFactory.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ExceptionEvent.h>
#include <cetty/channel/ChannelStateEvent.h>
#include <cetty/channel/ChildChannelStateEvent.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelInboundMessageHandler.h>
#include <cetty/channel/SimpleChannelUpstreamHandler.h>
#include <cetty/channel/socket/ServerSocketChannelFactory.h>
#include <cetty/channel/IpAddress.h>

#include <cetty/util/Exception.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace bootstrap {

using namespace cetty::channel;
using namespace cetty::channel::socket;
using namespace cetty::util;

class Acceptor : public cetty::channel::ChannelInboundMessageHandler<ChannelPtr> {
public:
    virtual void messageUpdated(ChannelHandlerContext& ctx) {
        InboundMessageHandlerContext* context =
            static_cast<InboundMessageHandlerContext*>(&ctx);

        std::deque<ChannelPtr>& in = context->getInboundMessageQueue();
        for (;;) {
            Channel child = in.poll();
            if (child == null) {
                break;
            }

            for (Entry<ChannelOption<?>, Object> e: childOptions.entrySet()) {
                try {
                    if (!child.config().setOption((ChannelOption<Object>) e.getKey(), e.getValue())) {
                        logger.warn("Unknown channel option: " + e);
                    }
                } catch (Throwable t) {
                    logger.warn("Failed to set a channel option: " + child, t);
                }
            }

            try {
                childEventLoop.register(child);
            } catch (Throwable t) {
                logger.warn("Failed to register an accepted channel: " + child, t);
            }
        }
    }
};

class Binder : public cetty::channel::SimpleChannelUpstreamHandler {
public:
    typedef std::deque<ChannelFuturePtr> FutureQueue;
    typedef std::map<std::string, boost::any> OptionsMap;

public:
    Binder(const SocketAddress& localAddress, ServerBootstrap& bootstrap)
        : localAddress(localAddress), bootstrap(bootstrap) {
    }
    virtual ~Binder() {}

    virtual ChannelHandlerPtr clone() { return shared_from_this(); }
    virtual std::string toString() const { return "Binder"; }

    FutureQueue& getFutureQueue() { return futureQueue; }

    void channelOpen(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        if (bootstrap.getPipelineFactory()) {
            e.getChannel()->getConfig().setPipelineFactory(
                bootstrap.getPipelineFactory());
        }

        // Split options into two categories: parent and child.
        OptionsMap& allOptions = bootstrap.getOptions();

        for (OptionsMap::iterator itr = allOptions.begin(); itr != allOptions.end(); ++itr) {
            if (itr->first.find("child.") == 0) {
                childOptions.insert(std::make_pair(itr->first.substr(6), itr->second));
            }
            else if (itr->first.compare("pipelineFactory") != 0) {
                parentOptions.insert(std::make_pair(itr->first, itr->second));
            }
        }

        // Apply parent options.
        e.getChannel()->getConfig().setOptions(parentOptions);
        ctx.sendUpstream(e);

        futureQueue.push_front(e.getChannel()->bind(localAddress));
    }

    void childChannelOpen(ChannelHandlerContext& ctx, const ChildChannelStateEvent& e) {
        // Apply child options.
        e.getChildChannel()->getConfig().setOptions(childOptions);
        ctx.sendUpstream(e);
    }

    void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
        futureQueue.push_front(Channels::failedFuture(e.getChannel(), e.getCause()));
        ctx.sendUpstream(e);
    }

private:
    SocketAddress localAddress;
    ServerBootstrap& bootstrap;

    FutureQueue futureQueue;
    OptionsMap parentOptions;
    OptionsMap  childOptions;
};

typedef boost::intrusive_ptr<Binder> BinderPtr;

void ServerBootstrap::setFactory(const ChannelFactoryPtr& factory) {
    if (dynamic_cast<ServerChannelFactory*>(factory.get())) {
        Bootstrap::setFactory(factory);
    }
    else {
        throw InvalidArgumentException(
            "factory must be a ServerChannelFactory");
    }
}

const ChannelHandlerPtr& ServerBootstrap::getParentHandler() {
    return this->parentHandler;
}

void ServerBootstrap::setParentHandler(const ChannelHandlerPtr& parentHandler) {
    this->parentHandler = parentHandler;
}

ChannelPtr ServerBootstrap::bind() {
    const SocketAddress* localAddress = getTypedOption<SocketAddress>("localAddress");

    if (NULL == localAddress) {
        return ChannelPtr();
    }

    return bind(*localAddress);
}

ChannelPtr ServerBootstrap::bind(int port) {
    return bind(SocketAddress(IpAddress::IPv4, port));
}

ChannelPtr ServerBootstrap::bind(const std::string& ip, int port) {
    return bind(SocketAddress(ip, port));
}

ChannelFuturePtr ServerBootstrap::bind(const SocketAddress& localAddress) {
    // bossPipeline's life cycle will be managed by the server channel.
    ChannelPipelinePtr bossPipeline = Channels::pipeline();

    BinderPtr binder = BinderPtr(new Binder(localAddress, *this));
    ChannelHandlerPtr binderHandler = boost::dynamic_pointer_cast<ChannelHandler>(binder);
    ChannelHandlerPtr parentHandler = getParentHandler();

    bossPipeline->addLast("binder", binderHandler);

    if (parentHandler) {
        bossPipeline->addLast("userHandler", parentHandler->clone());
    }

    const ChannelFactoryPtr& factory = getFactory();

    if (!factory) {
        LOG_ERROR(logger, "has not set the factory.");
        return ChannelPtr();
    }

    ChannelPtr channel = getFactory()->newChannel(bossPipeline);

    if (!channel) {
        LOG_ERROR(logger, "Server channel factory failed to create a new channel.");
        return ChannelPtr();
    }

    ChannelFuturePtr future = channel->newFuture();
    channel->bind(localAddress, future)->addListener(ChannelFutureListener::CLOSE_ON_FAILURE);

    return future;
}





}
}
