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

#include <cetty/channel/DownstreamChannelStateEvent.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelState.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/util/internal/ConversionUtil.h>

namespace cetty {
namespace channel {

using namespace cetty::util::internal;

static const boost::any EMPTY_ANY;

DownstreamChannelStateEvent::DownstreamChannelStateEvent(
    const ChannelPtr& channel,
    const ChannelFuturePtr& future,
    const ChannelState& state,
    const boost::any& value)
    : channel(channel), future(future), state(state), value(value) {
}

DownstreamChannelStateEvent::DownstreamChannelStateEvent(
    const ChannelPtr& channel,
    const ChannelFuturePtr& future,
    const ChannelState& state)
    : channel(channel), future(future), state(state), value(EMPTY_ANY) {
}

DownstreamChannelStateEvent::~DownstreamChannelStateEvent() {
}

const ChannelPtr& DownstreamChannelStateEvent::getChannel() const {
    return this->channel;
}

const ChannelFuturePtr& DownstreamChannelStateEvent::getFuture() const {
    return this->future;
}

const ChannelState& DownstreamChannelStateEvent::getState() const {
    return this->state;
}

const boost::any& DownstreamChannelStateEvent::getValue() const {
    return this->value;
}

std::string DownstreamChannelStateEvent::toString() const {
    std::string channelString = this->channel->toString();

    if (ChannelState::OPEN == this->state) {
        if (this->value.empty()) {
            channelString.append(" CLOSE");
        }
        else {
            channelString.append(" OPEN");
        }
    }
    else if (ChannelState::BOUND == this->state) {
        if (this->value.empty()) {
            channelString.append(" UNBIND");
        }
        else {
            channelString.append(" BIND: ");
            const SocketAddress* address = boost::any_cast<SocketAddress>(&value);

            if (address) {
                channelString.append(address->toString());
            }
            else {
                channelString.append("cann't parse the address.");
            }
        }
    }
    else if (ChannelState::CONNECTED == this->state) {
        if (this->value.empty()) {
            channelString.append(" DISCONNECT");
        }
        else {
            channelString.append(" CONNECT: ");
            const SocketAddress* address = boost::any_cast<SocketAddress>(&value);

            if (address) {
                channelString.append(address->toString());
            }
            else {
                channelString.append("can't parse the address.");
            }
        }
    }
    else if (ChannelState::INTEREST_OPS == this->state) {
        channelString.append(" CHANGE_INTEREST: ");
        int ops = ConversionUtil::toInt(value);

        if (ops & Channel::OP_READ) {
            channelString.append("set channel readable");
        }
        else {
            channelString.append("set channel unreadable");
        }
    }
    else {
        channelString.append(" UNKOWN STATE.");
    }

    return channelString;
}

}
}
