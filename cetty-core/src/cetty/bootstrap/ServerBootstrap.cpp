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
#include <cetty/channel/NullChannel.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ServerChannelFactory.h>
#include <cetty/channel/ChannelPipelines.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelFutureListener.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelInboundMessageHandler.h>
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
    Acceptor(ServerBootstrap& bootstrap) : bootstrap(bootstrap) {
    }

    virtual ~Acceptor() {}

    virtual ChannelHandlerPtr clone() {
        return shared_from_this();
    }

    virtual std::string toString() const {
        return "Acceptor";
    }

    virtual void messageUpdated(ChannelHandlerContext& ctx) {
        while (!inboundQueue.empty()) {
            const ChannelPtr& child = inboundQueue.front();

            if (!child) {
                break;
            }

            const ChannelOption::Options& childOptions = bootstrap.getChildOptions();
            ChannelOption::Options::const_iterator itr = childOptions.begin();
            for (; itr != childOptions.end(); ++itr) {
                if (!child->getConfig().setOption(itr->first, itr->second)) {
                    //logger.warn("Unknown channel option: " + e);
                }
            }

            inboundQueue.pop_front();
        }
    }

private:
    ServerBootstrap& bootstrap;
};

const ChannelHandlerPtr& ServerBootstrap::getParentHandler() {
    return this->parentHandler;
}

ServerBootstrap& ServerBootstrap::setParentHandler(const ChannelHandlerPtr& parentHandler) {
    this->parentHandler = parentHandler;

    return *this;
}

ChannelFuturePtr ServerBootstrap::bind(int port) {
    return bind(SocketAddress(IpAddress::IPv4, port));
}

ChannelFuturePtr ServerBootstrap::bind(const std::string& ip, int port) {
    return bind(SocketAddress(ip, port));
}

ChannelFuturePtr ServerBootstrap::bind(const SocketAddress& localAddress) {
    // bossPipeline's life cycle will be managed by the server channel.
    ChannelPipelinePtr serverPipeline = ChannelPipelines::pipeline();

    ChannelHandlerPtr acceptor(new Acceptor(*this));
    ChannelHandlerPtr parentHandler = getParentHandler();

    serverPipeline->addLast("acceptor", acceptor);

    if (parentHandler) {
        serverPipeline->addLast("userHandler", parentHandler->clone());
    }

    const ChannelFactoryPtr& factory = getFactory();

    if (!factory) {
        LOG_ERROR << "has not set the factory.";
        return NullChannel::instance()->getCloseFuture();
    }

    ChannelPtr channel = factory->newChannel(serverPipeline);

    if (!channel) {
        LOG_ERROR << "Server channel factory failed to create a new channel.";
        return NullChannel::instance()->getCloseFuture();
    }

    ChannelFuturePtr future = channel->newFuture();
    channel->bind(localAddress, future)->addListener(ChannelFutureListener::CLOSE_ON_FAILURE);

    return future;
}

ServerBootstrap& ServerBootstrap::setChildOption(const ChannelOption& option,
                                     const ChannelOption::Variant& value) {
    if (value.empty()) {
        childOptions.erase(option);
        LOG_WARN << "setOption, the key ("
            << option.getName()
            << ") is empty value, remove from the options.";
    }
    else {
        LOG_INFO << "set Option, the key is " << option.getName();
        childOptions.insert(std::make_pair(option, value));
    }

    return *this;
}

const ChannelOption::Options& ServerBootstrap::getChildOptions() const {
    return childOptions;
}

ServerBootstrap& ServerBootstrap::setFactory(const ChannelFactoryPtr& factory) {
    ChannelPipelinePtr childPipeline = getPipeline();
    if (childPipeline) {
        boost::intrusive_ptr<ServerChannelFactory> serverFactory =
            boost::dynamic_pointer_cast<ServerChannelFactory>(factory);
        if (serverFactory) {
            serverFactory->setChildChannelPipeline(childPipeline);
        }
        else {
            //
        }
    }

    AbstractBootstrap::setFactory(factory);
    return *this;
}

ServerBootstrap& ServerBootstrap::setPipeline(const ChannelPipelinePtr& pipeline) {
    ChannelFactoryPtr factory = getFactory();
    if (factory) {
        boost::intrusive_ptr<ServerChannelFactory> serverFactory =
            boost::dynamic_pointer_cast<ServerChannelFactory>(factory);
        if (serverFactory) {
            serverFactory->setChildChannelPipeline(pipeline);
        }
        else {
            //
        }
    }

    AbstractBootstrap::setPipeline(pipeline);
    return *this;
}

}
}
