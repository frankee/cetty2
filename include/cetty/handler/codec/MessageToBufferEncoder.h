#if !defined(CETTY_HANDLER_CODEC_MESSAGETOBUFFERENCODER_H)
#define CETTY_HANDLER_CODEC_MESSAGETOBUFFERENCODER_H
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
#include <cetty/channel/ChannelOutboundMessageHandler.h>

namespace cetty {
namespace handler {
namespace codec {

template<typename OutboundInT>
class MessageToBufferEncoder : public ChannelOutboundMessageHandler<OutboundInT> {
public:
    void flush(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
        MessageBuf<I> in = ctx.outboundMessageBuffer();
        ByteBuf out = ctx.nextOutboundByteBuffer();

        for (;;) {
            Object msg = in.poll();

            if (msg == null) {
                break;
            }

            if (!isEncodable(msg)) {
                ctx.nextOutboundMessageBuffer().add(msg);
                continue;
            }

            I imsg = (I) msg;

            try {
                encode(ctx, imsg, out);
            }
            catch (Throwable t) {
                if (t instanceof CodecException) {
                    ctx.fireExceptionCaught(t);
                }
                else {
                    ctx.fireExceptionCaught(new EncoderException(t));
                }
            }
        }

        ctx.flush(future);
    }

    /**
     * Returns {@code true} if and only if the specified message can be encoded by this encoder.
     *
     * @param msg the message
     */
    virtual bool isEncodable(const OutboundInT& msg) {
        return true;
    }

    virtual void encode(ChannelHandlerContext& ctx,
                        const OutboundInT& msg,
                        ChannelBufferPtr& out) = 0;
};


}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_MESSAGETOBUFFERENCODER_H)

// Local Variables:
// mode: c++
// End:
