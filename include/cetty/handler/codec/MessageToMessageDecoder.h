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

#include <cetty/channel/AbstractChannelInboundMessageHandler.h>
#include <cetty/handler/codec/DecoderException.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::channel;

template<typename InboundInT, typename InboundOutT>
class MessageToMessageDecoder
    : public AbstractChannelInboundMessageHandler<InboundInT, InboundOutT> {
public:
    typedef typename ChannelInboundMessageHandler<InboundInT>::MessageQueue MessageQueue;

public:
    MessageToMessageDecoder() {}
    virtual ~MessageToMessageDecoder() {}

    virtual void messageUpdated(ChannelHandlerContext& ctx) {
        bool notify = false;
        MessageQueue& queue = ChannelInboundMessageHandler<InboundInT>::queue;

        while (!queue.empty()) {
            try {
                InboundInT& msg = queue.front();

                if (!msg) {
                    break;
                }

                if (!isDecodable(msg)) {
                    //ctx.nextInboundMessageBuffer().add(msg);
                    notify = true;
                    queue.pop_front();
                    continue;
                }

                InboundOutT omsg = decode(ctx, msg);

                if (!omsg) {
                    // Decoder consumed a message but returned null.
                    // Probably it needs more messages because it's an aggregator.
                    queue.pop_front();
                    continue;
                }

                if (AbstractChannelInboundMessageHandler<InboundInT, InboundOutT>::inboundTransfer.unfoldAndAdd(ctx, omsg)) {
                    notify = true;
                }

                queue.pop_front();
            }
            catch (const CodecException& e) {
                ctx.fireExceptionCaught(e);
            }
            catch (const std::exception& e) {
                ctx.fireExceptionCaught(DecoderException(e.what()));
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
