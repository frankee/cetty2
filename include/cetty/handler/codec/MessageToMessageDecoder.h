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

template<typename H,
         typename InboundIn,
         typename InboundOut,
         typename C = ChannelMessageHandlerContext<H,
         InboundIn,
         InboundOut,
         VoidMessage,
         VoidMessage,
         ChannelMessageContainer<InboundIn, MESSAGE_BLOCK>,
         ChannelMessageContainer<InboundOut, MESSAGE_BLOCK>,
         VoidMessageContainer,
         VoidMessageContainer> >
class MessageToMessageDecoder : private boost::noncopyable {
public:
    typedef MessageToMessageDecoder<H, InboundIn, InboundOut, C> Self;
    typedef C Context;

    typedef typename ChannelHandlerWrapper<H>::Handler Handler;
    typedef typename ChannelHandlerWrapper<H>::HandlerPtr HandlerPtr;

    typedef ChannelMessageContainer<InboundIn, MESSAGE_BLOCK> InboundContainer;
    typedef typename InboundContainer::MessageQueue InboundQueue;

    typedef ChannelMessageTransfer<InboundOut,
            ChannelMessageContainer<InboundOut, MESSAGE_BLOCK>,
            TRANSFER_INBOUND> InboundTransfer;

    typedef boost::function<bool (InboundIn const&)> DecodableChecker;
    typedef boost::function<InboundOut(ChannelHandlerContext&, InboundIn const&)> Decoder;

public:
    MessageToMessageDecoder()
        : transfer_(),
          constainer_() {
    }

    MessageToMessageDecoder(const Decoder& decoder)
        : decoder_(decoder),
          transfer_(),
          constainer_() {
    }

    MessageToMessageDecoder(const Decoder& decoder,
                            const DecodableChecker& checker)
        : decoder_(decoder),
          checker_(checker),
          transfer_(),
          constainer_() {
    }

    ~MessageToMessageDecoder() {}

    void setDecoder(const Decoder& decoder) {
        decoder_ = decoder;
    }

    void setDecodableChecker(const DecodableChecker& checker) {
        checker_ = checker;
    }

    void registerTo(Context& ctx) {
        BOOST_ASSERT(decoder_);

        transfer_ = ctx.inboundTransfer();
        constainer_ = ctx.inboundContainer();

        ctx.setChannelMessageUpdatedCallback(boost::bind(
                &Self::messageUpdated,
                this,
                _1));
    }

    void messageUpdated(ChannelHandlerContext& ctx) {
        bool notify = false;

        InboundQueue& inboundQueue = constainer_->getMessages();

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

                InboundOut omsg = decoder_(ctx, msg);

                if (!omsg) {
                    // Decoder consumed a message but returned null.
                    // Probably it needs more messages because it's an aggregator.
                    inboundQueue.pop_front();
                    continue;
                }

                if (transfer_->unfoldAndAdd(omsg)) {
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

protected:
    Decoder decoder_;
    DecodableChecker checker_;

    InboundTransfer* transfer_;
    InboundContainer* constainer_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_MESSAGETOMESSAGEDECODER_H)

// Local Variables:
// mode: c++
// End:
