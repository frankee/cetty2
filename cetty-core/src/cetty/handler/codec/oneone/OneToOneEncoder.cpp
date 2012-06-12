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

#include <cetty/handler/codec/oneone/OneToOneEncoder.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/ChannelHandlerContext.h>

namespace cetty {
namespace handler {
namespace codec {
namespace oneone {

using namespace cetty::channel;

void OneToOneEncoder::handleDownstream(ChannelHandlerContext& ctx,
                                       const ChannelEvent& evt) {
    ctx.sendDownstream(evt);
}

void OneToOneEncoder::writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e) {
    const ChannelMessage& originalMessage = e.getMessage();
    ChannelMessage encodedMessage = encode(ctx, e.getChannel(), originalMessage);

    if (originalMessage == encodedMessage) {
        ctx.sendDownstream(e);
    }
    else if (!encodedMessage.empty()) {
        Channels::write(ctx,
                        e.getFuture(),
                        encodedMessage,
                        e.getRemoteAddress());
    }
    else {
        //log here
    }
}

void OneToOneEncoder::stateChangeRequested(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e) {
    ctx.sendDownstream(e);
}

}
}
}
}