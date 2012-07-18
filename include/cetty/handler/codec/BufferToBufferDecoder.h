#if !defined(CETTY_HANDLER_CODEC_BUFFERTOBUFFERDECODER_H)
#define CETTY_HANDLER_CODEC_BUFFERTOBUFFERDECODER_H
/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
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

#include <cetty/channel/ChannelInboundBufferHandler.h>

namespace cetty {
namespace handler {
namespace codec {

class BufferToBufferDecoder : public cetty::channel::ChannelInboundBufferHandler {
public:
    virtual void messageUpdated(ChannelHandlerContext& ctx, const ChannelBufferPtr& in) {
        callDecode(ctx, in, ctx.nextOutboundByteBuffer());
    }

    virtual void channelInactive(ChannelHandlerContext& ctx) {
        ByteBuf in = ctx.inboundByteBuffer();
        ByteBuf out = ctx.nextInboundByteBuffer();

        if (!in.readable()) {
            callDecode(ctx, in, out);
        }

        int oldOutSize = out.readableBytes();

        try {
            decodeLast(ctx, in, out);
        }
        catch (Throwable t) {
            if (t instanceof CodecException) {
                ctx.fireExceptionCaught(t);
            }
            else {
                ctx.fireExceptionCaught(new DecoderException(t));
            }
        }

        if (out.readableBytes() > oldOutSize) {
            in.discardReadBytes();
            ctx.fireInboundBufferUpdated();
        }

        ctx.fireChannelInactive();
    }

    void decodeLast(ChannelHandlerContext& ctx, ByteBuf in, ByteBuf out) {
        decode(ctx, in, out);
    }

    virtual void decode(ChannelHandlerContext& ctx, ByteBuf in, ByteBuf out) = 0;

private:
    void callDecode(ChannelHandlerContext& ctx, ByteBuf in, ByteBuf out) {
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
            ctx.fireInboundBufferUpdated();
        }
    }
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_BUFFERTOBUFFERDECODER_H)

// Local Variables:
// mode: c++
// End:
