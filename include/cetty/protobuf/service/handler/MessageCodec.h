#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_MESSAGECODEC_H)
#define CETTY_PROTOBUF_SERVICE_HANDLER_MESSAGECODEC_H

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

#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>

namespace google {
namespace protobuf {
class Message;
}
}

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::channel;

class MessageCodec {
public:
    static ProtobufServiceMessagePtr decode(ChannelHandlerContext& ctx,
                                            const ChannelBufferPtr& msg);

    static ChannelBufferPtr encode(ChannelHandlerContext& ctx,
                                   const ProtobufServiceMessagePtr& msg);

private:
    static int decodeMessage(const ChannelBufferPtr& buffer,
                             const ProtobufServiceMessagePtr& message);

    static int decodePayload(const ChannelBufferPtr& buffer,
                             const ProtobufServiceMessagePtr& message);

    static void encodeMessage(const ProtobufServiceMessagePtr& message,
                              const ChannelBufferPtr& buffer);

    static void encodeProtobuf(const google::protobuf::Message& message,
                               const ChannelBufferPtr& buffer);

private:
    MessageCodec();
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_MESSAGECODEC_H)

// Local Variables:
// mode: c++
// End:
