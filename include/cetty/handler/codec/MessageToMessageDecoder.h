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

#include <cetty/channel/ChannelMessageHandlerContext.h>
#include <cetty/handler/codec/DecoderException.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::channel;

template<typename InboundIn, typename InboundOut>
class MessageToMessageDecoder : private boost::noncopyable {
public:
    typedef MessageToMessageDecoder<InboundIn, InboundOut> Self;
    typedef boost::shared_ptr<Self> Ptr;

    typedef ChannelMessageContainer<InboundIn, MESSAGE_BLOCK> InboundContainer;

    typedef ChannelMessageHandlerContext<
        Self,
        InboundIn,
        InboundOut,
        VoidMessage,
        VoidMessage,
        InboundContainer,
        ChannelMessageContainer<InboundOut, MESSAGE_BLOCK>,
        VoidMessageContainer,
        VoidMessageContainer> Context;

public:
    MessageToMessageDecoder() {}
    virtual ~MessageToMessageDecoder() {}

    virtual void registerTo(Context& ctx) {
        context_ = &ctx;

        ctx.setChannelMessageUpdatedCallback(boost::bind(
            &Self::messageUpdated,
            this,
            _1));
    }

    void messageUpdated(ChannelHandlerContext& ctx) {
        bool notify = false;

        typename InboundContainer::MessageQueue& inboundQueue =
            context_->inboundContainer()->getMessages();

        while (!inboundQueue.empty()) {
            try {
                InboundIn& msg = inboundQueue.front();

                if (!msg) {
                    break;
                }

//                 if (!isDecodable(msg)) {
//                     if (skipInboundTransfer.unfoldAndAdd(msg)) {
//                         notify = true;
//                     }
// 
//                     inboundQueue.pop_front();
//                     continue;
//                 }

                InboundOut omsg = decode(ctx, msg);

                if (!omsg) {
                    // Decoder consumed a message but returned null.
                    // Probably it needs more messages because it's an aggregator.
                    inboundQueue.pop_front();
                    continue;
                }

                if (context_->inboundTransfer()->unfoldAndAdd(omsg)) {
                    notify = true;
                }

                inboundQueue.pop_front();
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
    bool isDecodable(const InboundIn& msg) {
        return true;
    }

    virtual InboundOut decode(ChannelHandlerContext& ctx,
                               const InboundIn& msg) = 0;

protected:
    Context* context_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_MESSAGETOMESSAGEDECODER_H)

// Local Variables:
// mode: c++
// End:
