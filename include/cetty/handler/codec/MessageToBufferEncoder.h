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
#include <cetty/buffer/Unpooled.h>
#include <cetty/channel/ChannelMessageHandlerContext.h>
#include <cetty/handler/codec/EncoderException.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::buffer;
using namespace cetty::channel;

template<typename H,
         typename OutboundIn,
         typename Context = ChannelMessageHandlerContext<H,
         VoidMessage,
         VoidMessage,
         OutboundIn,
         ChannelBufferPtr,
         VoidMessageContainer,
         VoidMessageContainer,
         ChannelMessageContainer<OutboundIn, MESSAGE_BLOCK>,
         ChannelBufferContainer> >
class MessageToBufferEncoder : public boost::noncopyable {
public:
    typedef MessageToBufferEncoder<H, OutboundIn, Context> Self;

    typedef typename ChannelHandlerWrapper<H>::Handler Handler;
    typedef typename ChannelHandlerWrapper<H>::HandlerPtr HandlerPtr;

    typedef boost::function<ChannelBufferPtr(ChannelHandlerContext&,
            OutboundIn const&,
            ChannelBufferPtr const&)> Encoder;

    typedef ChannelMessageContainer<OutboundIn, MESSAGE_BLOCK> OutboundContainer;
    typedef typename OutboundContainer::MessageQueue MessageQueue;

    typedef ChannelMessageTransfer<OutboundIn,
            ChannelBufferContainer,
            TRANSFER_OUTBOUND> OutboundTransfer;

public:
    MessageToBufferEncoder()
        : nextBufferContainerAccumulated_(false) {
    }

    MessageToBufferEncoder(const Encoder& encoder)
        : nextBufferContainerAccumulated_(false),
          encoder_(encoder) {
    }

    ~MessageToBufferEncoder() {}

    void registerTo(Context& ctx) {
        transfer_ = ctx.outboundTransfer();
        container_ = ctx.outboundTransfer();

        ctx.setFlushFunctor(boost::bind(
                                &Self::flush,
                                this,
                                _1,
                                _2));
    }

    void setEncoder(const Encoder& encoder) {
        encoder_ = encoder;
    }

    void flush(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
        ChannelBufferPtr out;

        if (nextBufferContainerAccumulated_) {
            out = Unpooled::buffer(initBufferSize_);
        }

        OutboundContainer* container = context_->outboundContainer();
        MessageQueue& outboundQueue = container->getMessages();

        while (!outboundQueue.empty()) {
            OutboundIn& msg = outboundQueue.front();

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
                if (nextBufferContainerAccumulated_) {
                    encode(ctx, msg, out);
                }
                else {
                    ChannelBufferPtr encodedBuf = encode(ctx, msg, out);

                    if (encodedBuf) {
                        context_->outboundTransfer()->write(encodedBuf, future);
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

        if (nextBufferContainerAccumulated_) {
            context_->outboundTransfer()->write(out, future);
        }
    }

private:
    bool nextBufferContainerAccumulated_;

    Encoder encoder_;
    OutboundTransfer* transfer_;
    OutboundContainer* container_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_MESSAGETOBUFFERENCODER_H)

// Local Variables:
// mode: c++
// End:
