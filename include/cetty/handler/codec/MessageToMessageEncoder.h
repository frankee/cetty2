#if !defined(CETTY_HANDLER_CODEC_MESSAGETOMESSAGEENCODER_H)
#define CETTY_HANDLER_CODEC_MESSAGETOMESSAGEENCODER_H

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
#include <cetty/handler/codec/CodecUtil.h>
#include <cetty/handler/codec/EncoderException.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::channel;

template<typename OutboundInT, typename OutboundOutT>
class MessageToMessageEncoder
        : public cetty::channel::ChannelOutboundMessageHandler<OutboundInT> {
public:
    virtual ~MessageToMessageEncoder() {}

protected:
    virtual void flush(OutboundMessageContext& ctx,
                       const ChannelFuturePtr& future) {
        OutboundMessageContext::MessageQueue& in =
            ctx.getOutboundMessageQueue();

        while (!in.empty()) {
            try {
                OutboundInT& msg = in.front();

                if (!msg) {
                    break;
                }

                if (!isEncodable(msg)) {
                    //ctx.nextOutboundMessageBuffer().add(msg);
                    continue;
                }

                OutboundOutT omsg = encode(ctx, msg);

                if (!omsg) {
                    // encode() might be waiting for more inbound messages to generate
                    // an aggregated message - keep polling.
                    continue;
                }

                CodecUtil<OutboundOutT>::unfoldAndAdd(ctx, omsg, false);
            }
            catch (const CodecException& e) {
                ctx.fireExceptionCaught(e);
            }
            catch (const std::exception& e) {
                ctx.fireExceptionCaught(EncoderException(e.what()));
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

    virtual OutboundOutT encode(ChannelHandlerContext& ctx,
                                const OutboundInT& msg) = 0;
};


}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_MESSAGETOMESSAGEENCODER_H)

// Local Variables:
// mode: c++
// End: