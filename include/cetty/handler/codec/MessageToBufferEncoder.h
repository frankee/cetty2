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
#include <cetty/channel/ChannelPipelineMessageTransfer.h>
#include <cetty/buffer/Unpooled.h>
#include <cetty/handler/codec/EncoderException.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::buffer;
using namespace cetty::channel;

template<typename OutboundInT>
class MessageToBufferEncoder
    : public ChannelOutboundMessageHandler<OutboundInT> {
protected:
    using ChannelOutboundMessageHandler<OutboundInT>::outboundQueue;

public:
    MessageToBufferEncoder() : hasOutBuffer(false), initBufferSize(0) {}
    MessageToBufferEncoder(int initBufferSize)
        : hasOutBuffer(true),
          initBufferSize(initBufferSize <= 0 ? 8*1024 : initBufferSize) {
    }

    virtual ~MessageToBufferEncoder() {}

    virtual void afterAdd(ChannelHandlerContext& ctx) {
        outboundTransfer.setContext(ctx);
    }

    void flush(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
        ChannelBufferPtr out;

        if (hasOutBuffer) {
            out = Unpooled::buffer(initBufferSize);
        }

        while (!outboundQueue.empty()) {
            OutboundInT& msg = outboundQueue.front();

            if (!msg) {
                break;
            }

            //             if (!isEncodable(msg)) {
            //                 MessageContext* nextMsgCtx = ctx.nextOutboundMessageHandlerContext();
            //                 if (nextMsgCtx) {
            //                     nextMsgCtx->addOutboundMessage(msg);
            //                 }
            //                 continue;
            //             }

            try {
                if (hasOutBuffer) {
                    encode(ctx, msg, out);
                }
                else {
                    ChannelBufferPtr decodedBuf = encode(ctx, msg, out);

                    if (decodedBuf) {
                        outboundTransfer.write(decodedBuf, future);
                    }
                }
            }
            catch (const CodecException& e) {
                ctx.fireExceptionCaught(e);
            }
            catch (const std::exception& e) {
                ctx.fireExceptionCaught(EncoderException(e.what()));
            }

            outboundQueue.pop_front();
        }

        if (hasOutBuffer) {
            outboundTransfer.write(out, future);
        }
    }

    /**
     * Returns {@code true} if and only if the specified message can be encoded by this encoder.
     *
     * @param msg the message
     */
    virtual bool isEncodable(const OutboundInT& msg) {
        return true;
    }

    virtual ChannelBufferPtr encode(ChannelHandlerContext& ctx,
                                    const OutboundInT& msg,
                                    const ChannelBufferPtr& out) = 0;

protected:
    ChannelPipelineMessageTransfer<ChannelBufferPtr, ChannelOutboundBufferHandlerContext> outboundTransfer;

private:
    bool hasOutBuffer;
    int  initBufferSize;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_MESSAGETOBUFFERENCODER_H)

// Local Variables:
// mode: c++
// End:
