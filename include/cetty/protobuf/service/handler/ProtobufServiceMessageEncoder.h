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

#include <cetty/buffer/ChannelBufferPtr.h>
#include <cetty/handler/codec/MessageToMessageEncoder.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>
#include <cetty/protobuf/service/handler/MessageCodec.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::handler::codec;

class ProtobufServiceMessageEncoder {
public:
    typedef MessageToMessageEncoder<ProtobufServiceMessageEncoder,
        ProtobufServiceMessagePtr,
        ChannelBufferPtr> Encoder;

    typedef Encoder::Context Context;
    typedef Encoder::Handler Handler;
    typedef Encoder::HandlerPtr HandlerPtr;

public:
    ProtobufServiceMessageEncoder()
        : encoder_(boost::bind(&MessageCodec::encode, _1, _2)) {
    }

    ~ProtobufServiceMessageEncoder() {}

    void registerTo(Context& ctx) {
        encoder_.registerTo(ctx);
    }

private:
    Encoder encoder_;
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEENCODER_H)

// Local Variables:
// mode: c++
// End:
