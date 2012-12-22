#if !defined(CETTY_HANDLER_CODEC_MESSAGETOMESSAGECODEC_H)
#define CETTY_HANDLER_CODEC_MESSAGETOMESSAGECODEC_H

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

#include <cetty/handler/codec/MessageToMessageDecoder.h>
#include <cetty/handler/codec/MessageToMessageEncoder.h>

namespace cetty {
namespace handler {
namespace codec {

template<typename H,
         typename InboundIn,
         typename InboundOut,
         typename OutboundIn,
         typename OutboundOut>
class MessageToMessageCodec : private boost::noncopyable {
public:
    typedef ChannelMessageHandlerContext<H,
            InboundIn,
            InboundOut,
            OutboundIn,
            OutboundOut,
            ChannelMessageContainer<InboundIn, MESSAGE_BLOCK>,
            ChannelMessageContainer<InboundOut, MESSAGE_BLOCK>,
            ChannelMessageContainer<OutboundIn, MESSAGE_BLOCK>,
            ChannelMessageContainer<OutboundOut, MESSAGE_BLOCK> > Context;

    typedef typename ChannelHandlerWrapper<H>::Handler Handler;
    typedef typename ChannelHandlerWrapper<H>::HandlerPtr HandlerPtr;

    typedef MessageToMessageDecoder<H, InboundIn, InboundOut, Context> MessageDecoder;
    typedef MessageToMessageEncoder<H, OutboundIn, OutboundOut, Context> MessageEncoder;

    typedef typename MessageDecoder::Decoder Decoder;
    typedef typename MessageEncoder::Encoder Encoder;

    typedef typename MessageDecoder::DecodableChecker DecodableChecker;
    typedef typename MessageEncoder::EncodableChecker EncodableChecker;

    typedef typename MessageDecoder::InboundContainer InboundContainer;
    typedef typename MessageEncoder::OutboundContainer OutboundContainer;

    typedef typename MessageDecoder::InboundQueue InboundQueue;
    typedef typename MessageEncoder::OutboundQueue OutboundQueue;

    typedef typename MessageDecoder::InboundTransfer InboundTransfer;
    typedef typename MessageEncoder::OutboundTransfer OutboundTransfer;

public:
    MessageToMessageCodec() {
    }

    MessageToMessageCodec(const Decoder& decoder, const Encoder& encoder)
        : decoder_(decoder),
          encoder_(encoder) {
    }

    void setDecoder(const Decoder& decoder) {
        decoder_.setDecoder(decoder);
    }

    void setEncoder(const Encoder& encoder) {
        encoder_.setEncoder(encoder);
    }

    void setChecker(const DecodableChecker& decodableChecker,
                    const EncodableChecker& encodableChecker) {
        decoder_.setDecodableChecker(decodableChecker);
        encoder_.setEncodableChecker(encodableChecker);
    }

    void registerTo(Context& ctx) {
        decoder_.registerTo(ctx);
        encoder_.registerTo(ctx);
    }

private:
    MessageDecoder decoder_;
    MessageEncoder encoder_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_MESSAGETOMESSAGECODEC_H)

// Local Variables:
// mode: c++
// End:
