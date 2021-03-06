#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGECODEC_H)
#define CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGECODEC_H

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

#include <cetty/handler/codec/MessageToMessageCodec.h>
#include <cetty/protobuf/service/handler/MessageCodec.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::channel;
using namespace cetty::handler::codec;

class ProtobufServiceMessageCodec : private boost::noncopyable {
public:
    typedef MessageToMessageCodec<ProtobufServiceMessageCodec,
            ChannelBufferPtr,
            ProtobufServiceMessagePtr,
            ProtobufServiceMessagePtr,
            ChannelBufferPtr> Codec;

    typedef Codec::Context Context;
    typedef Codec::Handler Handler;
    typedef Codec::HandlerPtr HandlerPtr;

public:
    ProtobufServiceMessageCodec()
        : codec_(boost::bind(MessageCodec::decode, _1, _2),
        boost::bind(MessageCodec::encode, _1, _2)) {
    }

    void registerTo(Context& ctx) {
        codec_.registerTo(ctx);
    }

private:
    Codec codec_;
};

}
}
}
}


#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGECODEC_H)

// Local Variables:
// mode: c++
// End:
