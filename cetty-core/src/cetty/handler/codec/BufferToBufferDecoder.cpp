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

#include <cetty/handler/codec/BufferToBufferDecoder.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelInboundBufferHandlerContext.h>

#include <cetty/handler/codec/CodecException.h>
#include <cetty/handler/codec/DecoderException.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::buffer;
using namespace cetty::channel;

BufferToBufferDecoder::BufferToBufferDecoder() {

}

BufferToBufferDecoder::~BufferToBufferDecoder() {

}

void BufferToBufferDecoder::messageReceived(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& in) {
    ChannelBufferPtr out = callDecode(ctx, in);
    inboundTransfer.unfoldAndAdd(out);
}

void BufferToBufferDecoder::channelInactive(ChannelHandlerContext& ctx) {

    const ChannelBufferPtr& in = getInboundChannelBuffer();
    ChannelBufferPtr out;

    if (!in->readable()) {
        out = callDecode(ctx, in);
    }

#if 0 //FIXME
    int oldOutSize = out.readableBytes();

    try {
        decodeLast(ctx, in, out);
    }
    catch (const CodecException& e) {
        ctx.fireExceptionCaught(e);
    }
    catch (const std::exception& e) {
        ctx.fireExceptionCaught(DecoderException(e.what()));
    }

    if (out.readableBytes() > oldOutSize) {
        in->discardReadBytes();
        ctx.fireMessageUpdated();
    }
#endif

    ctx.fireChannelInactive();
}

ChannelBufferPtr BufferToBufferDecoder::callDecode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& in) {
    //int oldOutSize = out.readableBytes();
    ChannelBufferPtr out;

    while (in->readable()) {
        int oldInSize = in->readableBytes();

        try {
            out = decode(ctx, in);
        }
        catch (const CodecException& e) {
            ctx.fireExceptionCaught(e);
        }
        catch (const std::exception& e) {
            ctx.fireExceptionCaught(DecoderException(e.what()));
        }

        if (oldInSize == in->readableBytes()) {
            break;
        }
    }

    //if (out.readableBytes() > oldOutSize) {
    //    in->discardReadBytes();
    //    ctx.fireMessageUpdated();
    //}

    return out;
}

}
}
}
