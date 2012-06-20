#if !defined(CETTY_PROTOBUF_HANDLER_PROTOBUFRPCMESSAGE_H)
#define CETTY_PROTOBUF_HANDLER_PROTOBUFRPCMESSAGE_H

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

#include <cetty/util/ReferenceCounter.h>
#include <boost/intrusive_ptr.hpp>

namespace google {
namespace protobuf {
class Message;
}
}

namespace cetty {
namespace protobuf {
namespace proto {
class RpcMessage;
}
}
}

namespace cetty {
namespace protobuf {
namespace handler {

    using google::protobuf::Message;
    using namespace cetty::protobuf::proto;

class ProtobufRpcMessage : public cetty::util::ReferenceCounter<ProtobufRpcMessage> {
public:
    ProtobufRpcMessage();
    ProtobufRpcMessage(int type);
    ProtobufRpcMessage(int type, const std::string& service, const std::string& method);
    ProtobufRpcMessage(int type, const std::string& service, const std::string& method, const Message* payload);

    RpcMessage* mutableRpcMessage();
    const RpcMessage& getRpcMessage() const;

    const Message* getPayload() const;

    void setPayload(const Message* message);

private:
    cetty::protobuf::proto::RpcMessage rpc;
    const google::protobuf::Message* payload;
};

typedef boost::intrusive_ptr<ProtobufRpcMessage> ProtobufRpcMessagePtr;

}
}
}

#endif //#if !defined(CETTY_PROTOBUF_HANDLER_PROTOBUFRPCMESSAGE_H)
