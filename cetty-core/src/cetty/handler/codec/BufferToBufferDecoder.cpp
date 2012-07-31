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

namespace cetty {
namespace handler {
namespace codec {

    using namespace cetty::buffer;
    using namespace cetty::channel;

BufferToBufferDecoder::BufferToBufferDecoder() {

}

BufferToBufferDecoder::~BufferToBufferDecoder() {

}

#if 0
void BufferToBufferDecoder::messageUpdated(ChannelHandlerContext& ctx,
    const ChannelBufferPtr& in) {
    NextContext* nextCtx = ctx.nextInboundBufferHandlerContext();
    if (nextCtx) {
        ChannelBufferPtr out = callDecode(ctx, in);
        nextCtx->setInboundChannelBuffer(out);
    }
}

void BufferToBufferDecoder::channelInactive(ChannelHandlerContext& ctx) {
    Context* context = ctx.downcast()<Context>();
    NextContext* nextContxt = ctx.nextInboundBufferHandlerContext();

    const ChannelBuffer& in = context->getInboundChannelBuffer();

    if (!in.readable()) {
        callDecode(ctx, in, out);
    }

    int oldOutSize = out.readableBytes();

    try {
        decodeLast(ctx, in, out);
    }
    catch (const CodecException& e) {
        ctx.fireExceptionCaught(e);
    }
    catch (const std::exception& e) {
        ctx.fireExceptionCaught(DecoderException(e));
    }

    if (out.readableBytes() > oldOutSize) {
        in->discardReadBytes();
        ctx.fireMessageUpdated();
    }

    ctx.fireChannelInactive();
}

ChannelBufferPtr BufferToBufferDecoder::callDecode(ChannelHandlerContext& ctx,
    const ChannelBufferPtr& in) {
    int oldOutSize = out.readableBytes();

    while (in.readable()) {
        int oldInSize = in.readableBytes();

        try {
            decode(ctx, in, out);
        }
        catch (Throwable t) {
            if (t instanceof CodecException) {
                ctx.fireExceptionCaught(t);
            }
            else {
                ctx.fireExceptionCaught(new DecoderException(t));
            }
        }

        if (oldInSize == in.readableBytes()) {
            break;
        }
    }

    if (out.readableBytes() > oldOutSize) {
        in.discardReadBytes();
        ctx.fireMessageUpdated();
    }
}

#endif
}
}
}