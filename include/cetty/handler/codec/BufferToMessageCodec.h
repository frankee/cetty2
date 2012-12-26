#if !defined(CETTY_HANDLER_CODEC_BUFFERTOMESSAGECODEC_H)
#define CETTY_HANDLER_CODEC_BUFFERTOMESSAGECODEC_H

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

#include <cetty/handler/codec/BufferToMessageDecoder.h>
#include <cetty/handler/codec/MessageToBufferEncoder.h>

namespace cetty {
namespace handler {
namespace codec {

template<typename H,
         typename InboundOut,
         typename OutboundIn>
class BufferToMessageCodec : private boost::noncopyable {
public:
    typename BufferToMessageDecoder::Decoder Decoder;
    typename MessageToBufferEncoder::Encoder Encoder;

    typename ChannelMessageHandlerContext<H,
             ChannelBufferPtr,
             InboundOut,
             OutboundIn,
             ChannelBufferPtr,
             ChannelBufferContainer,
             ChannelMessageContainer<InboundOut, MESSAGE_BLOCK>,
             ChannelMessageContainer<OutboundIn, MESSAGE_BLOCK>,
             ChannelBufferContainer> Context;

public:
    BufferToMessageCodec(const Decoder& decoder, const Encoder& encoder)
        : decoder_(decoder), encoder_(encoder) {
    }

    ~BufferToMessageCodec() {}

    void registerTo(Context& ctx) {
        decoder_.registerTo(ctx);
        encoder_.registerTo(ctx);
    }

private:
    BufferToMessageDecoder<H, InboundOut, Context> decoder_;
    MessageToBufferEncoder<H, OutboundIn, Context> encoder_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_BUFFERTOMESSAGECODEC_H)

// Local Variables:
// mode: c++
// End:
