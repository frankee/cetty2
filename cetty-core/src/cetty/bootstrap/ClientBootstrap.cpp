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
#include <cetty/channel/InetAddress.h>
#include <cetty/channel/ChannelException.h>

#include <cetty/logging/LoggerHelper.h>

#include <cetty/channel/asio/AsioService.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/channel/asio/AsioSocketChannel.h>

namespace cetty {
namespace bootstrap {

using namespace cetty::channel;
using namespace cetty::channel::asio;

ClientBootstrap::ClientBootstrap() {
}

ClientBootstrap::ClientBootstrap(const EventLoopPoolPtr& pool)
    : Bootstrap<ClientBootstrap>(pool) {
}

ClientBootstrap::ClientBootstrap(const EventLoopPtr& eventLoop)
    : eventLoop_(eventLoop) {
}

ClientBootstrap::ClientBootstrap(int threadCnt) {
    if (threadCnt < 1) {
        LOG_WARN << "client bootstrap thread count should no less than 1";
        threadCnt = 1;
    }

    setEventLoopPool(new AsioServicePool(threadCnt));
}

ChannelFuturePtr ClientBootstrap::connect(const InetAddress& remoteAddress) {
    return connect(remoteAddress, InetAddress::NULL_ADDRESS);
}

ChannelFuturePtr ClientBootstrap::connect(const InetAddress& remote,
        const InetAddress& local) {
    if (!remote) {
        LOG_INFO << "the remote address is invalidated, then return a failed future.";
        return NullChannel::instance()->newFailedFuture(
                   ChannelException("Failed to initialize a pipeline."));
    }

    ChannelPtr ch = newChannel();

    if (!ch) {
        LOG_INFO << "failed to create a new channel, then return a failed future.";
        return NullChannel::instance()->newFailedFuture(
                   ChannelException("Failed to create a new channel."));
    }

    ch->setInitializer(initializer_);
    ch->open();

    // Set the options.
    ch->config().setOptions(channelOptions());

    insertChannel(ch->id(), ch);

    // Bind.
    if (localAddress()) {
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
        pool->waitingForStop();
    }
}

ChannelPtr ClientBootstrap::newChannel() {
    const EventLoopPoolPtr& pool = eventLoopPool();
    const EventLoopPtr& eventLoop =
        pool ? pool->nextLoop() : eventLoop_;
    
    if (boost::dynamic_pointer_cast<AsioService>(eventLoop)) {
        return ChannelPtr(new AsioSocketChannel(eventLoop));
    }
    else {
        // other implement
        BOOST_ASSERT(false);
        return ChannelPtr();
    }
}

void ClientBootstrap::waitingForExit() {
    Channels& clientChannels = channels();
    Channels::const_iterator itr = clientChannels.begin();
    for (; itr != clientChannels.end(); ++itr) {
        itr->second->closeFuture()->awaitUninterruptibly();
    }
}

}
}
