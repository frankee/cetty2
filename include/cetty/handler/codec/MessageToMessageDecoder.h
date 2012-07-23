#if !defined(CETTY_HANDLER_CODEC_MESSAGETOMESSAGEDECODER_H)
#define CETTY_HANDLER_CODEC_MESSAGETOMESSAGEDECODER_H

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
namespace cetty {
namespace handler {
namespace codec {

template<InboundInT, InboundOutT>
class MessageToMessageDecoder : ChannelInboundMessageHandler<InboundInT> {
public:
    MessageToMessageDecoder() {}
    virtual ~MessageToMessageDecoder() {}

    virtual void messageUpdated(ChannelHandlerContext& ctx) {
        MessageBuf<I> in = ctx.inboundMessageBuffer();
        boolean notify = false;

        for (;;) {
            try {
                Object msg = in.poll();

                if (msg == null) {
                    break;
                }

                if (!isDecodable(msg)) {
                    ctx.nextInboundMessageBuffer().add(msg);
                    notify = true;
                    continue;
                }

                I imsg = (I) msg;
                O omsg = decode(ctx, imsg);

                if (omsg == null) {
                    // Decoder consumed a message but returned null.
                    // Probably it needs more messages because it's an aggregator.
                    continue;
                }

                if (CodecUtil.unfoldAndAdd(ctx, omsg, true)) {
                    notify = true;
                }
            }
            catch (const CodecException& e) {
                ctx.fireExceptionCaught(e);
            }
            catch (Throwable t) {
                ctx.fireExceptionCaught(new DecoderException(t));
            }
        }

        if (notify) {
            ctx.fireMessageUpdated();
        }
    }

    /**
     * Returns {@code true} if and only if the specified message can be decoded by this decoder.
     *
     * @param msg the message
     */
    bool isDecodable(const InboundInT& msg) {
        return true;
    }

    virtual InboundOutT decode(ChannelHandlerContext& ctx,
                               const InboundInT& msg) = 0;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_MESSAGETOMESSAGEDECODER_H)

// Local Variables:
// mode: c++
// End:
