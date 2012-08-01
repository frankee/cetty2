#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEENCODER_H)
#define CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEENCODER_H

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

#include <cetty/buffer/ChannelBufferFwd.h>
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

using namespace cetty::buffer;
using namespace cetty::channel;
using namespace cetty::handler::codec;
using namespace cetty::protobuf::service;

class ProtobufServiceMessageEncoder
    : public MessageToMessageEncoder<ProtobufServiceMessagePtr, ChannelBufferPtr> {
public:
    ProtobufServiceMessageEncoder() {}
    virtual ~ProtobufServiceMessageEncoder() {}

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const;

    static void encodeMessage(const ChannelBufferPtr& buffer,
                              const ProtobufServiceMessagePtr& message);

protected:
    virtual ChannelBufferPtr encode(ChannelHandlerContext& ctx,
        const ProtobufServiceMessagePtr& msg);

private:
    static void encodeProtobufMessage(const ChannelBufferPtr& buffer,
                                      const google::protobuf::Message& message);
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEENCODER_H)

// Local Variables:
// mode: c++
// End:
