#if !defined(CETTY_HANDLER_CODEC_PROTOBUF_PROTOBUFENCODER_H)
#define CETTY_HANDLER_CODEC_PROTOBUF_PROTOBUFENCODER_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/handler/codec/oneone/OneToOneEncoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace protobuf {

using namespace cetty::handler::codec::oneone;

/**
 * Encodes the requested <a href="http://code.google.com/p/protobuf/">Google
 * Protocol Buffers</a> {@link Message} and {@link MessageLite} into a
 * {@link ChannelBuffer}. A typical setup for TCP/IP would be:
 * <pre>
 * {@link ChannelPipeline} pipeline = ...;
 *
 * // Decoders
 * pipeline.addLast("frameDecoder",
 *                  new {@link LengthFieldBasedFrameDecoder}(1048576, 0, 4, 0, 4));
 * pipeline.addLast("protobufDecoder",
 *                  new {@link ProtobufDecoder}(MyMessage.getDefaultInstance()));
 *
 * // Encoder
 * pipeline.addLast("frameEncoder", new {@link LengthFieldPrepender}(4));
 * pipeline.addLast("protobufEncoder", new {@link ProtobufEncoder}());
 * </pre>
 * and then you can use a <tt>MyMessage</tt> instead of a {@link ChannelBuffer}
 * as a message:
 * <pre>
 * void messageReceived({@link ChannelHandlerContext} ctx, {@link MessageEvent} e) {
 *     MyMessage req = (MyMessage) e.getMessage();
 *     MyMessage res = MyMessage.newBuilder().setText(
 *                               "Did you say '" + req.getText() + "'?").build();
 *     ch.write(res);
 * }
 * </pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 */
class ProtobufEncoder : public cetty::handler::codec::oneone::OneToOneEncoder {
public:
    /**
     * Creates a new instance.
     */
    ProtobufEncoder() {}
    virtual ~ProtobufEncoder() {}

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const;

protected:
    virtual ChannelMessage encode(ChannelHandlerContext& ctx,
                                  const ChannelPtr& channel,
                                  const ChannelMessage& msg);
};


}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_PROTOBUF_PROTOBUFENCODER_H)

// Local Variables:
// mode: c++
// End:

