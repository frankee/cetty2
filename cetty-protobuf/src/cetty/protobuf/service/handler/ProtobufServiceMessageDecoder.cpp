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

#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/protobuf/service/proto/service.pb.h>
#include <cetty/protobuf/service/ProtobufServiceRegister.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufMessageCodec.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::buffer;
using namespace cetty::channel;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::proto;


ChannelHandlerPtr ProtobufServiceMessageDecoder::clone() {
    return ChannelHandlerPtr(this);
}

std::string ProtobufServiceMessageDecoder::toString() const {
    return "ProtobufServiceMessageDecoder";
}

ChannelMessage ProtobufServiceMessageDecoder::decode(ChannelHandlerContext& ctx,
        const ChannelPtr& channel,
        const ChannelMessage& msg) {

    ChannelBufferPtr buffer = msg.smartPointer<ChannelBuffer>();

    if (buffer) {
        ProtobufServiceMessagePtr message(new ProtobufServiceMessage);

        if (!decode(buffer, message)) {
            return ChannelMessage(message);
        }
        else {
            // error here
            printf("ProtobufServiceMessageDecoder::decode failed beacuse of type wrong");
            return msg;
        }
    }
    else {
        return msg;
    }
}

int ProtobufServiceMessageDecoder::decodePayload(const ChannelBufferPtr& buffer,
        const ProtobufServiceMessagePtr& message) {
    const google::protobuf::Message* prototype = NULL;

    const ServiceMessage& serviceMessage = message->getServiceMessage();

    if (serviceMessage.type() == REQUEST) {
        prototype = ProtobufServiceRegister::instance().getRequestPrototype(
                        serviceMessage.service(), serviceMessage.method());
    }
    else if (serviceMessage.type() == RESPONSE) {
        prototype = ProtobufServiceRegister::instance().getResponsePrototype(
                        serviceMessage.service(), serviceMessage.method());
    }

    google::protobuf::Message* payload = prototype->New();
    Array arry;
    buffer->readableBytes(&arry);
    payload->ParseFromArray(arry.data(), arry.length());
    message->setPayload(payload);

    return true;
}

//if successful then return 0,if type is wrong return -1;
int ProtobufServiceMessageDecoder::decode(const ChannelBufferPtr& buffer,
        const ProtobufServiceMessagePtr& message) {
    ServiceMessage* serviceMessage = message->mutableServiceMessage();

    while (buffer->readable()) {
        int wireType=0;
        int fieldNum=0;
        int fieldLength=0;
        int64_t type=0;
        int64_t id=0;
        int64_t error=0;

        if (ProtobufMessageCodec::decodeField(buffer, &wireType, &fieldNum, &fieldLength)) {
            switch (fieldNum) {
                //involved varint
            case 1:
                type = ProtobufMessageCodec::decodeVarint(buffer);

                if (type != REQUEST && type != RESPONSE && type != ERROR) {
                    return -1;
                }

                serviceMessage->set_type(static_cast<MessageType>(type));
                break;

            case 2:
                id = ProtobufMessageCodec::decodeFixed64(buffer);
				id = ChannelBuffers::swapLong(id);
                serviceMessage->set_id(id);
                break;

            case 3:
                buffer->readBytes(serviceMessage->mutable_service(), fieldLength);
                break;

            case 4:
                buffer->readBytes(serviceMessage->mutable_method(), fieldLength);
                break;

            case 5:
                error = ProtobufMessageCodec::decodeVarint(buffer);
                serviceMessage->set_error((ErrorCode)error);
                break;

            case 6:
                decodePayload(buffer, message);
                break;

            case 7:
                decodePayload(buffer, message);
                break;

            default:
                break;
            }
        }
    }

    return 0;
}

}
}
}
}