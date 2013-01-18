#if !defined(CETTY_GEARMAN_PROTOCOL_GEARMANCODEC_H)
#define CETTY_GEARMAN_PROTOCOL_GEARMANCODEC_H

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

#include <cetty/handler/codec/MessageToBufferEncoder.h>
#include <cetty/handler/codec/MessageToMessageDecoder.h>

#include <cetty/gearman/protocol/GearmanMessage.h>

namespace cetty {
namespace gearman {
namespace protocol {

using namespace cetty::channel;
using namespace cetty::handler::codec;

class GearmanMessageCodec : private boost::noncopyable {
public:
    typedef ChannelMessageContainer<ChannelBufferPtr, MESSAGE_BLOCK> InboundContainer;
    typedef ChannelMessageContainer<GearmanMessagePtr, MESSAGE_BLOCK> NextInboundContainer;

    typedef ChannelMessageContainer<GearmanMessagePtr, MESSAGE_BLOCK> OutboundContainer;
    typedef ChannelMessageContainer<ChannelBufferPtr, MESSAGE_STREAM> NextOutboundContainer;

    typedef ChannelMessageHandlerContext<GearmanMessageCodec,
            ChannelBufferPtr,
            GearmanMessagePtr,
            GearmanMessagePtr,
            ChannelBufferPtr,
            InboundContainer,
            NextInboundContainer,
            OutboundContainer,
            NextOutboundContainer> Context;

    typedef ChannelHandlerWrapper<GearmanMessageCodec>::Handler Handler;
    typedef ChannelHandlerWrapper<GearmanMessageCodec>::HandlerPtr HandlerPtr;

    typedef MessageToMessageDecoder<GearmanMessageCodec,
            ChannelBufferPtr,
            GearmanMessagePtr,
            Context> MessageDecoder;

    typedef MessageToBufferEncoder<GearmanMessageCodec,
            GearmanMessagePtr,
            Context> MessageEncoder;

    typedef MessageDecoder::Decoder Decoder;
    typedef MessageEncoder::Encoder Encoder;

    typedef InboundContainer::MessageQueue InboundQueue;
    typedef OutboundContainer::MessageQueue OutboundQueue;

    typedef MessageDecoder::InboundTransfer InboundTransfer;
    typedef MessageEncoder::OutboundTransfer OutboundTransfer;

public:
    GearmanMessageCodec();
    ~GearmanMessageCodec();

    void registerTo(Context& ctx) {
        decoder_.registerTo(ctx);
        encoder_.registerTo(ctx);
    }

private:
    GearmanMessagePtr decode(ChannelHandlerContext& ctx,
                             const ChannelBufferPtr& msg);

    ChannelBufferPtr encode(ChannelHandlerContext& ctx,
                            const GearmanMessagePtr& msg);

    int caculateParametersLength(const GearmanMessagePtr& msg);

    void writeHeader(const ChannelBufferPtr& buffer, int type, int length);
    void writeHeaderAhead(const ChannelBufferPtr& buffer, int type, int length);
    void writeParameters(const ChannelBufferPtr& buffer,
                         const std::vector<std::string>& parameters,
                         bool withZeroPad);
    void writeParametersAhead(const ChannelBufferPtr& buffer,
                              const std::vector<std::string>& parameters,
                              bool withZeroPad);

private:
    MessageDecoder decoder_;
    MessageEncoder encoder_;
};

}
}
}

#endif //#if !defined(CETTY_GEARMAN_PROTOCOL_GEARMANCODEC_H)

// Local Variables:
// mode: c++
// End:
