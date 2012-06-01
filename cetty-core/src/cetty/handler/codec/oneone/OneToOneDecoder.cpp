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

#include <cetty/channel/Channel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/handler/codec/oneone/OneToOneDecoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace oneone {

using namespace cetty::channel;

void OneToOneDecoder::handleUpstream(ChannelHandlerContext& ctx, const ChannelEvent& evt) {
    ctx.sendUpstream(evt);
}

void OneToOneDecoder::messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
    const ChannelMessage& originalMessage = e.getMessage();
    ChannelMessage decodedMessage = decode(ctx, e.getChannel(), originalMessage);

    if (originalMessage == decodedMessage) {
        ctx.sendUpstream(e);
    }
    else if (!decodedMessage.empty()) {
        Channels::fireMessageReceived(ctx, decodedMessage, e.getRemoteAddress());
    }
    else {
        //log here, decoded message is empty.
    }
}

void OneToOneDecoder::exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
    ctx.sendUpstream(e);
}

void OneToOneDecoder::writeCompleted(ChannelHandlerContext& ctx, const WriteCompletionEvent& e) {
    ctx.sendUpstream(e);
}

void OneToOneDecoder::channelStateChanged(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    ctx.sendUpstream(e);
}

void OneToOneDecoder::childChannelStateChanged(ChannelHandlerContext& ctx, const ChildChannelStateEvent& e) {
    ctx.sendUpstream(e);
}

}
}
}
}