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
#include <cetty/channel/NullChannel.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelines.h>
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

cetty::channel::ChannelFuturePtr ClientBootstrap::connect(const std::string& host, int port) {
    SocketAddress remote(host, port);
    return connect(remote);
}

ChannelFuturePtr ClientBootstrap::connect(const SocketAddress& remoteAddress) {
    return connect(remoteAddress, SocketAddress::NULL_ADDRESS);
}

ChannelFuturePtr ClientBootstrap::connect(const SocketAddress& remoteAddress, const SocketAddress& localAddress) {
    ChannelPipelinePtr pipeline;
    ChannelPtr ch;

    SocketAddress remote = remoteAddress;
    SocketAddress local  = localAddress;

    try {
        pipeline = ChannelPipelines::pipeline(getPipeline());
    }
    catch (...) {
        LOG_ERROR() << "has an exception when get pipeline from factory, then return a failed future.";
        return NullChannel::getInstance()->newFailedFuture(
                   ChannelPipelineException("Failed to initialize a pipeline."));
    }

    ch = getFactory()->newChannel(pipeline);

    if (!ch) {
        LOG_ERROR() << "failed to create a new channel from the factory, then return a failed future.";
        return NullChannel::getInstance()->newFailedFuture(
                   ChannelPipelineException("Failed to create a new channel."));
    }

    // Set the options.
    ch->getConfig().setOptions(getOptions());

    // Bind.
    if (local.validated()) {
        LOG_INFO() << "bind the channel to local address" << local.toString();
        ChannelFuturePtr future = ch->bind(local);
        future->awaitUninterruptibly();
    }

    // Connect.
    return ch->connect(remote);
}

}
}
