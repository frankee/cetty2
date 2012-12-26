#if !defined(CETTY_HANDLER_CODEC_BUFFERTOBUFFERCODEC_H)
#define CETTY_HANDLER_CODEC_BUFFERTOBUFFERCODEC_H

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

#include <cetty/handler/codec/BufferToBufferDecoder.h>
#include <cetty/handler/codec/BufferToBufferEncoder.h>

namespace cetty {
namespace handler {
namespace codec {

template<typename H>
class BufferToBufferCodec : private boost::noncopyable {
public:
    typedef BufferToBufferDecoder::Decoder Decoder;
    typedef BufferToBufferEncoder::Encoder Encoder;

    typedef ChannelMessageHandlerContext<H,
            ChannelBufferPtr,
            ChannelBufferPtr,
            ChannelBufferPtr,
            ChannelBufferPtr,
            ChannelBufferContainer,
            ChannelBufferContainer,
            ChannelBufferContainer,
            ChannelBufferContainer> Context;

public:
    BufferToBufferCodec(const Decoder& decoder, const Encoder& encoder)
        : decoder_(decoder),
          encoder_(encoder) {
    }

    ~ BufferToBufferCodec() {}

    void registerTo(Context& ctx) {
        decoder_.registerTo(ctx);
        encoder_.registerTo(ctx);
    }

private:
    BufferToBufferDecoder<H, Context> decoder_;
    BufferToBufferEncoder<H, Context> encoder_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_BUFFERTOBUFFERCODEC_H)

// Local Variables:
// mode: c++
// End:
