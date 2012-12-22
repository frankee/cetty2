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
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelException.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace bootstrap {

using namespace cetty::channel;

ClientBootstrap::ClientBootstrap() {

}

ClientBootstrap::ClientBootstrap(const EventLoopPoolPtr& pool)
    : AbstractBootstrap<ClientBootstrap>(pool) {
}

ChannelFuturePtr ClientBootstrap::connect(const SocketAddress& remoteAddress) {
    return connect(remoteAddress, SocketAddress::NULL_ADDRESS);
}

ChannelFuturePtr ClientBootstrap::connect(const SocketAddress& remote,
        const SocketAddress& local) {
    if (!remote.validated()) {
        LOG_ERROR << "the remote address is invalidated, then return a failed future.";
        return NullChannel::instance()->newFailedFuture(
                   ChannelException("Failed to initialize a pipeline."));
    }

    ChannelPtr ch = newChannel();

    if (!ch) {
        LOG_ERROR << "failed to create a new channel, then return a failed future.";
        return NullChannel::instance()->newFailedFuture(
                   ChannelException("Failed to create a new channel."));
    }

    ch->setInitializer(initializer_);

    // Set the options.
    ch->config().setOptions(options());

    ch->initialize();

    clientChannels_[ch->id()] = ch;

    // Bind.
    if (localAddress().validated()) {
        LOG_INFO << "bind the channel to local address" << local.toString();
        ChannelFuturePtr future = ch->bind(local);
        future->awaitUninterruptibly();
    }

    // Connect.
    return ch->connect(remote);
}

void ClientBootstrap::shutdown() {
    EventLoopPoolPtr pool = eventLoopPool();
    if (pool) {
        pool->stop();
        pool->waitForStop();
    }
}

}
}
