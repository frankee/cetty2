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

#include <cetty/buffer/ChannelBufferPtr.h>

namespace cetty {
namespace channel {
class ChannelHandlerContext;
class ChannelInboundBufferHandlerContext;
}
}

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::channel;
using namespace cetty::buffer;


template<typename H,
    typename Context = ChannelMessageHandlerContext<H,
    ChannelBufferPtr,
    ChannelBufferPtr,
    VoidMessage,
    VoidMessage,
    ChannelBufferContainer,
    ChannelBufferContainer,
    VoidMessageContainer,
    VoidMessageContainer> >
class BufferToBufferDecoder : boost::noncopyable {
public:
    typedef boost::function<ChannelBufferPtr (ChannelHandlerContext&, const ChannelBufferPtr&)> Decoder;


public:
    BufferToBufferDecoder(const Decoder& decoder);
    ~BufferToBufferDecoder();

    void registerTo(Context& ctx) {

    }

    void channelInactive(ChannelHandlerContext& ctx) {
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

    ChannelBufferPtr decodeLast(ChannelHandlerContext& ctx,
                                const ChannelBufferPtr& in) {
        return decode(ctx, in);
    }

protected:
    virtual void messageReceived(ChannelHandlerContext& ctx,
                                 const ChannelBufferPtr& in) {
                                     ChannelBufferPtr out = callDecode(ctx, in);
                                     inboundTransfer.unfoldAndAdd(out);
    }

private:
    ChannelBufferPtr callDecode(ChannelHandlerContext& ctx,
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

private:
    Decoder decoder_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_BUFFERTOBUFFERDECODER_H)

// Local Variables:
// mode: c++
// End:
