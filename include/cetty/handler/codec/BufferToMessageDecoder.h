#if !defined(CETTY_HANDLER_CODEC_BUFFERTOMESSAGEDECODER_H)
#define CETTY_HANDLER_CODEC_BUFFERTOMESSAGEDECODER_H
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

namespace cetty { namespace handler { namespace codec {

template<InboundOutT>
class BufferToMessageDecoder : public ChannelInboundBufferHandler {
public:
    virtual void beforeAdd(ChannelHandlerContext ctx) {
        this.ctx = ctx;
        ChannelInboundBufferHandler::beforeAdd(ctx);
    }

    virtual void inboundBufferUpdated(ChannelHandlerContext ctx) {
        callDecode(ctx);
    }

    virtual void channelInactive(ChannelHandlerContext ctx) {
        ByteBuf in = ctx.inboundByteBuffer();
        if (in.readable()) {
            callDecode(ctx);
        }

        try {
            if (CodecUtil.unfoldAndAdd(ctx, decodeLast(ctx, in), true)) {
                in.discardReadBytes();
                ctx.fireInboundBufferUpdated();
            }
        } catch (Throwable t) {
            if (t instanceof CodecException) {
                ctx.fireExceptionCaught(t);
            } else {
                ctx.fireExceptionCaught(new DecoderException(t));
            }
        }

        ctx.fireChannelInactive();
    }

    virtual MessageT decode(ChannelHandlerContext ctx, ByteBuf in) = 0;

    MessageT decodeLast(ChannelHandlerContext ctx, ByteBuf in) {
        return decode(ctx, in);
    }

protected:
    void callDecode(ChannelHandlerContext ctx) {
        ByteBuf in = ctx.inboundByteBuffer();

        boolean decoded = false;
        for (;;) {
            try {
                int oldInputLength = in.readableBytes();
                MessageT o = decode(ctx, in);
                if (o == null) {
                    if (oldInputLength == in.readableBytes()) {
                        break;
                    } else {
                        continue;
                    }
                } else {
                    if (oldInputLength == in.readableBytes()) {
                        throw new IllegalStateException(
                                "decode() did not read anything but decoded a message.");
                    }
                }

                if (CodecUtil.unfoldAndAdd(ctx, o, true)) {
                    decoded = true;
                } else {
                    break;
                }
            } catch (Throwable t) {
                if (decoded) {
                    decoded = false;
                    in.discardReadBytes();
                    ctx.fireInboundBufferUpdated();
                }

                if (t instanceof CodecException) {
                    ctx.fireExceptionCaught(t);
                } else {
                    ctx.fireExceptionCaught(new DecoderException(t));
                }
            }
        }

        if (decoded) {
            in.discardReadBytes();
            ctx.fireInboundBufferUpdated();
        }
    }

    /**
     * Replace this decoder in the {@link ChannelPipeline} with the given handler.
     * All remaining bytes in the inbound buffer will be forwarded to the new handler's
     * inbound buffer.
     */
    void replace(String newHandlerName, ChannelInboundByteHandler newHandler) {
        if (!ctx.executor().inEventLoop()) {
            throw new IllegalStateException("not in event loop");
        }

        // We do not use ChannelPipeline.replace() here so that the current context points
        // the new handler.
        ctx.pipeline().addAfter(ctx.name(), newHandlerName, newHandler);

        ByteBuf in = ctx.inboundByteBuffer();
        try {
            if (in.readable()) {
                ctx.nextInboundByteBuffer().writeBytes(ctx.inboundByteBuffer());
                ctx.fireInboundBufferUpdated();
            }
        } finally {
            ctx.pipeline().remove(this);
        }
    }

private:
    ChannelHandlerContext* ctx;
};

}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_BUFFERTOMESSAGEDECODER_H)

// Local Variables:
// mode: c++
// End:
