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

#include <boost/noncopyable.hpp>
#include <cetty/channel/ChannelMessageHandlerContext.h>
#include <cetty/handler/codec/EncoderException.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::channel;

template<typename OutboundIn, typename OutboundOut>
class MessageToMessageEncoder :private boost::noncopyable {
public:
    typedef MessageToMessageEncoder<OutboundIn, OutboundOut> Self;
    typedef boost::shared_ptr<Self> Ptr;

    typedef ChannelMessageContainer<OutboundIn, MESSAGE_BLOCK> OutboundContainer;
    typedef typename OutboundContainer::MessageQueue OutboundMessageQueue;

    typedef ChannelMessageHandlerContext<
    Self,
    VoidMessage,
    VoidMessage,
    OutboundIn,
    OutboundOut,
    VoidMessageContainer,
    VoidMessageContainer,
    OutboundContainer,
    ChannelMessageContainer<OutboundOut, MESSAGE_BLOCK> > Context;

public:
    virtual ~MessageToMessageEncoder() {}

    virtual void registerTo(Context& ctx) {
        context_ = &ctx;

        ctx.setFlushFunctor(boost::bind(
                                &Self::flush,
                                this,
                                _1,
                                _2));
    }

    void flush(ChannelHandlerContext& ctx,
               const ChannelFuturePtr& future) {

        OutboundMessageQueue& outboundQueue =
            context_->outboundContainer()->getMessages();

        while (!outboundQueue.empty()) {
            try {
                OutboundIn& msg = outboundQueue.front();

                if (!msg) {
                    break;
                }

                if (!isEncodable(msg)) {
                    //ctx.nextOutboundMessageBuffer().add(msg);
                    outboundQueue.pop_front();
                    continue;
                }

                OutboundOut omsg = encode(ctx, msg);

                if (!omsg) {
                    // encode() might be waiting for more inbound messages to generate
                    // an aggregated message - keep polling.
                    outboundQueue.pop_front();
                    continue;
                }

                context_->outboundTransfer()->unfoldAndAdd(omsg);

                outboundQueue.pop_front();
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
    virtual bool isEncodable(const OutboundIn& msg) {
        return true;
    }

    virtual OutboundOut encode(ChannelHandlerContext& ctx,
                               const OutboundIn& msg) = 0;

protected:
    Context* context_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_MESSAGETOMESSAGEENCODER_H)

// Local Variables:
// mode: c++
// End:
