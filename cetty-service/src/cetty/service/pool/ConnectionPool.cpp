/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/service/pool/ConnectionPool.h>

#include <boost/bind.hpp>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelFuture.h>

namespace cetty {
namespace service {
namespace pool {

using namespace cetty::channel;
using namespace cetty::service;
using namespace cetty::bootstrap::asio;

ConnectionPool::ConnectionPool(const Connections& connections)
    : connections(connections), bootstrap(0), connecting(false) {

}

ConnectionPool::~ConnectionPool() {

}

ChannelPtr ConnectionPool::getChannel(const ConnectedCallback& callback) {
    if (channels.empty()) {
        if (!connecting) {
            ChannelFuturePtr future =
                bootstrap.connect(connections[0].host, connections[0].port);
            callbacks.push_back(callback);

            connecting = true;

            future->addListener(boost::bind(
                &ConnectionPool::connectedCallback, this, _1));
        }

        return ChannelPtr();
    }
    else {
        return channels.begin()->second->channel;
    }
}

cetty::channel::ChannelPtr ConnectionPool::getChannel() {
    if (!channels.empty()) {
        return channels.begin()->second->channel;
    }
    return ChannelPtr();
}

void ConnectionPool::connectedCallback(const ChannelFuture& future) {
    connecting = false;

    ChannelConnection* conn = new ChannelConnection;
    ChannelPtr channel = future.channel();
    
    conn->channel = channel;
    int id = channel->id();
    channels.insert(id, conn);

    while (!callbacks.empty()) {
        const ConnectedCallback& call = callbacks.front();
        if (call) {
            call(channel);
        }
        callbacks.pop_front();
    }
}

}
}
}
