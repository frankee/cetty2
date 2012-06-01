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

#include <cetty/channel/UpstreamChannelStateEvent.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelState.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/util/Integer.h>
#include <cetty/util/internal/ConversionUtil.h>

namespace cetty {
namespace channel {

using namespace cetty::util;
using namespace cetty::util::internal;

UpstreamChannelStateEvent::UpstreamChannelStateEvent(const ChannelPtr& channel,
    const ChannelState& state,
    const boost::any& value)
    : channel(channel), state(state), value(value) {
}

UpstreamChannelStateEvent::~UpstreamChannelStateEvent() {
}

const ChannelPtr& UpstreamChannelStateEvent::getChannel() const {
    return this->channel;
}

const ChannelState& UpstreamChannelStateEvent::getState() const {
    return this->state;
}

const boost::any& UpstreamChannelStateEvent::getValue() const {
    return this->value;
}

const ChannelFuturePtr& UpstreamChannelStateEvent::getFuture() const {
    return this->channel->getSucceededFuture();
}

std::string UpstreamChannelStateEvent::toString() const {
    std::string buf;
    buf.reserve(256);
    buf = this->channel->toString();

    if (state == ChannelState::OPEN) {
        if (!value.empty()) {
            buf.append(" OPEN");
        }
        else {
            buf.append(" CLOSED");
        }
    }
    else if (state == ChannelState::BOUND) {
        if (!value.empty()) {
            const SocketAddress* address = boost::any_cast<SocketAddress>(&value);
            buf.append(" BOUND: ");

            if (address) {
                buf.append(address->toString());
            }
        }
        else {
            buf.append(" UNBOUND");
        }
    }
    else if (state == ChannelState::CONNECTED) {
        if (!value.empty()) {
            buf.append(" CONNECTED: ");
            const SocketAddress* address = boost::any_cast<SocketAddress>(&value);

            if (address) {
                buf.append(address->toString());
            }
        }
        else {
            buf.append(" DISCONNECTED");
        }
    }
    else if (state == ChannelState::INTEREST_OPS) {
        buf.append(" INTEREST_CHANGED: ");
        buf.append(Integer::toString(ConversionUtil::toInt(value)));
    }
    else {
        buf.append(state.toString());
        //buf.append(": ");
        //buf.append(getValue());
    }

    return buf;
}

}
}
