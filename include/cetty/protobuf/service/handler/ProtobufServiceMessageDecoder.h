#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEDECODER_H)
#define CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEDECODER_H

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

#include <cetty/handler/codec/oneone/OneToOneDecoder.h>
#include <cetty/buffer/ChannelBufferFwd.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>


namespace cetty {
namespace protobuf {
namespace service {
namespace proto {
class ServiceMessage;
}
}
}
}

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::channel;
using namespace cetty::protobuf::service::proto;
using namespace cetty::buffer;
using namespace cetty::protobuf::service::handler;

class ProtobufServiceMessageDecoder : public cetty::handler::codec::oneone::OneToOneDecoder {
public:
    ProtobufServiceMessageDecoder() {}
    virtual ~ProtobufServiceMessageDecoder() {}

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const;

    //protected:
    virtual ChannelMessage decode(ChannelHandlerContext& ctx,
                                  const ChannelPtr& channel,
                                  const ChannelMessage& msg);


    static int decode(const ChannelBufferPtr& buffer,
                      const ProtobufServiceMessagePtr& message);

private:
    static int decodePayload(const ChannelBufferPtr& buffer,
                             const ProtobufServiceMessagePtr& message);

};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEDECODER_H)

// Local Variables:
// mode: c++
// End:
