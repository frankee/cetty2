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

#include <cetty/bootstrap/ClientBootstrap.h>

#include <boost/assert.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/NullChannel.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/channel/ChannelPipelineException.h>
#include <cetty/channel/socket/ClientSocketChannelFactory.h>
#include <cetty/util/Exception.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace bootstrap {

using namespace cetty::channel;
using namespace cetty::channel::socket;
using namespace cetty::util;
using namespace cetty::logging;

ClientBootstrap::ClientBootstrap() {
}

ClientBootstrap::ClientBootstrap(const ChannelFactoryPtr& channelFactory)
    : Bootstrap(channelFactory) {
}

ChannelFuturePtr ClientBootstrap::connect() {
    const SocketAddress* remoteAddress = getTypedOption<SocketAddress>("remoteAddress");

    if (remoteAddress == NULL) {
        LOG_ERROR(logger, "has not set the remoteAddress, connect failed.");
        return Channels::failedFuture(
                   NullChannel::getInstance(),
                   IllegalStateException("remoteAddress option is not set."));
    }

    return connect(*remoteAddress);
}

cetty::channel::ChannelFuturePtr ClientBootstrap::connect(const std::string& host, int port) {
    SocketAddress remote(host, port);
    return connect(remote);
}

ChannelFuturePtr ClientBootstrap::connect(const SocketAddress& remoteAddress) {
    const SocketAddress* localAddress =
        getTypedOption<SocketAddress>("localAddress");

    if (localAddress) {
        return connect(remoteAddress, *localAddress);
    }

    return connect(remoteAddress, SocketAddress::NULL_ADDRESS);
}

ChannelFuturePtr ClientBootstrap::connect(const SocketAddress& remoteAddress, const SocketAddress& localAddress) {
    ChannelPipelinePtr pipeline;
    ChannelPtr ch;

    // FIXME:
    // for some reason, the pointer address of the impl int the remoteAddress
    // will be changed after getFactory()->newChannel(pipeline) under msvc8.
    SocketAddress remote = remoteAddress;
    SocketAddress local  = localAddress;

    const ChannelPipelineFactoryPtr& factory = getPipelineFactory();

    if (!factory) {
        LOG_ERROR(logger, "has not set the pipeline factory, then return a failed future.");
        return Channels::failedFuture(
                   NullChannel::getInstance(),
                   ChannelPipelineException("has not set the pipeline factory."));
    }

    try {
        pipeline = getPipelineFactory()->getPipeline();
    }
    catch (...) {
        LOG_ERROR(logger, "has an exception when get pipeline from factory, then return a failed future.");
        return Channels::failedFuture(
                   NullChannel::getInstance(),
                   ChannelPipelineException("Failed to initialize a pipeline."));
    }

    ch = getFactory()->newChannel(pipeline);

    if (!ch) {
        LOG_ERROR(logger, "failed to create a new channel from the factory, then return a failed future.");
        return Channels::failedFuture(
                   NullChannel::getInstance(),
                   ChannelPipelineException("Failed to create a new channel."));
    }

    // Set the options.
    ch->getConfig().setOptions(getOptions());

    // Bind.
    if (local.validated()) {
        LOG_INFO(logger, "bind the channel to local address %s.", local.toString().c_str());
        ChannelFuturePtr future = ch->bind(local);
        future->awaitUninterruptibly();
    }

    // Connect.
    return ch->connect(remote);
}

}
}
