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
using namespace cetty::bootstrap;

ConnectionPool::ConnectionPool(const Connections& connections)
    : connections_(connections),
      bootstrap_(1),
      connecting_(false) {
}

ConnectionPool::ConnectionPool(const Connections& connections,
                               const EventLoopPtr& eventLoop)
    : connections_(connections),
      bootstrap_(eventLoop),
      connecting_(false) {
}

ConnectionPool::~ConnectionPool() {
}

ChannelPtr ConnectionPool::getChannel(const ConnectedCallback& callback) {
    if (channels_.empty()) {
        if (!connecting_) {
            ChannelFuturePtr future =
                bootstrap_.connect(connections_[0].host, connections_[0].port);
            callbacks_.push_back(callback);

            connecting_ = true;

            future->addListener(boost::bind(
                                    &ConnectionPool::connectedCallback, this, _1));
        }

        return ChannelPtr();
    }
    else {
        return channels_.begin()->second->channel;
    }
}

cetty::channel::ChannelPtr ConnectionPool::getChannel() {
    if (!channels_.empty()) {
        return channels_.begin()->second->channel;
    }

    return ChannelPtr();
}

void ConnectionPool::connectedCallback(const ChannelFuture& future) {
    connecting_ = false;

    if (future.isSuccess()) {
        ChannelConnection* conn = new ChannelConnection;
        ChannelPtr channel = future.channel();

        conn->channel = channel;
        int id = channel->id();
        channels_.insert(id, conn);

        while (!callbacks_.empty()) {
            const ConnectedCallback& call = callbacks_.front();

            if (call) {
                call(channel);
            }

            callbacks_.pop_front();
        }
    }
    else {
        //TODO
    }
}

void ConnectionPool::setChannelInitializer(const Channel::Initializer& initializer) {
    bootstrap_.setChannelInitializer(initializer);
}

}
}
}
