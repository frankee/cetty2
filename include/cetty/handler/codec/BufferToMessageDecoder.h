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

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/channel/ChannelHandlerWrapper.h>
#include <cetty/channel/ChannelMessageHandlerContext.h>
#include <cetty/handler/codec/DecoderException.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::buffer;
using namespace cetty::channel;

template<typename H,
         typename InboundOut,
         typename Ctx = ChannelMessageHandlerContext<H,
         ChannelBufferPtr,
         InboundOut,
         VoidMessage,
         VoidMessage,
         ChannelBufferContainer,
         ChannelMessageContainer<ChannelBufferPtr, MESSAGE_BLOCK>,
         VoidMessageContainer,
         VoidMessageContainer> >
class BufferToMessageDecoder : private boost::noncopyable {
public:
    typedef Ctx Context;
    typedef BufferToMessageDecoder<H, InboundOut, Context> Self;

    typedef boost::function<InboundOut(ChannelHandlerContext&, const ChannelBufferPtr&)> Decoder;

    typedef typename ChannelHandlerWrapper<H>::Handler Handler;
    typedef typename ChannelHandlerWrapper<H>::HandlerPtr HandlerPtr;

    typedef ChannelMessageContainer<ChannelBufferPtr, MESSAGE_STREAM> InboundContainer;
    typedef ChannelMessageTransfer<InboundOut,
            ChannelMessageContainer<InboundOut, MESSAGE_BLOCK>,
            TRANSFER_INBOUND> InboundTransfer;

public:
    BufferToMessageDecoder() {
    }

    BufferToMessageDecoder(const Decoder& decoder)
        : decoder_(decoder) {
    }

    ~BufferToMessageDecoder() {}

    void registerTo(Context& ctx) {
        transfer_ = ctx.inboundTransfer();
        container_ = ctx.inboundContainer();

        ctx.setChannelMessageUpdatedCallback(boost::bind(
                &Self::messageUpdated,
                this,
                _1));

        ctx.setChannelInactiveCallback(boost::bind(
                                           &Self::channelInactive,
                                           this,
                                           _1));
    }

    void setDecoder(const Decoder& decoder) {
        decoder_ = decoder;
    }

    void messageUpdated(ChannelHandlerContext& ctx) {
        callDecode(ctx);
    }

    void channelInactive(ChannelHandlerContext& ctx) {
#if 0
        const ChannelBufferPtr& in = getInboundChannelBuffer();

        if (in && in->readable()) {
            callDecode(ctx);

            try {
                if (inboundTransfer.unfoldAndAdd(decodeLast(ctx, in))) {
                    in->discardReadBytes();
                    ctx.fireMessageUpdated();
                }
            }
            catch (const CodecException& e) {
                ctx.fireExceptionCaught(e);
            }
            catch (const std::exception& e) {
                ctx.fireExceptionCaught(DecoderException(e.what()));
            }
        }

        ctx.fireChannelInactive();
#endif
    }

    InboundOut decodeLast(ChannelHandlerContext& ctx,
                          const ChannelBufferPtr& in) {
        return decoder_(ctx, in);
    }

#if 0
    /**
     * Replace this decoder in the {@link ChannelPipeline} with the given handler.
     * All remaining bytes in the inbound buffer will be forwarded to the new handler's
     * inbound buffer.
     */
    void replace(const std::string& newHandlerName, ChannelInboundByteHandler newHandler) {
        if (!ctx.executor().inEventLoop()) {
            throw new IllegalStateException("not in event loop");
        }

        // We do not use ChannelPipeline.replace() here so that the current context points
        // the new handler.
        ctx.pipeline().addAfter(ctx.name(), newHandlerName, newHandler);

        ByteBuf in = ctx.inboundByteBuffer();

        try {
            if (in->readable()) {
                ctx.nextInboundByteBuffer().writeBytes(ctx.inboundByteBuffer());
                ctx.fireMessageUpdated();
            }
        } finally {

            ctx.pipeline().remove(this);
        }
    }
#endif

protected:
    void callDecode(ChannelHandlerContext& ctx) {
        const ChannelBufferPtr& in = container_->getMessages();
        bool decoded = false;

        for (;;) {
            try {
                int oldInputLength = in->readableBytes();
                InboundOut o = decoder_(ctx, in);

                if (!o) {
                    if (oldInputLength == in->readableBytes()) {
                        break;
                    }
                    else {
                        continue;
                    }
                }
                else {
                    if (oldInputLength == in->readableBytes()) {
                        throw cetty::util::IllegalStateException(
                            "decode() did not read anything but decoded a message.");
                    }
                }

                if (transfer_->unfoldAndAdd(o)) {
                    decoded = true;
                }
                else {
                    break;
                }
            }
            catch (const CodecException& e) {
                if (decoded) {
                    decoded = false;
                    ctx.fireMessageUpdated();
                    in->discardReadBytes();
                }

                ctx.fireExceptionCaught(e);
            }
            catch (const std::exception& e) {
                if (decoded) {
                    decoded = false;
                    ctx.fireMessageUpdated();
                    in->discardReadBytes();
                }

                ctx.fireExceptionCaught(DecoderException(e.what()));
            }
        }

        if (decoded) {
            ctx.fireMessageUpdated();
            in->discardReadBytes();
        }
    }

protected:
    Decoder decoder_;

    InboundTransfer* transfer_;
    InboundContainer* container_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_BUFFERTOMESSAGEDECODER_H)

// Local Variables:
// mode: c++
// End:
