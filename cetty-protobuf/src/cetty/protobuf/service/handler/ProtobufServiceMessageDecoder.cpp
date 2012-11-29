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

#include <cstdio>
#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBufferUtil.h>
#include <cetty/protobuf/service/service.pb.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/ProtobufServiceRegister.h>
#include <cetty/protobuf/service/handler/ProtobufMessageCodec.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::buffer;
using namespace cetty::channel;
using namespace cetty::protobuf::service;

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
    StringPiece arry;
    buffer->readableBytes(&arry);
    payload->ParseFromArray(arry.data(), arry.length());
    buffer->offsetReaderIndex(arry.length());

    message->setPayload(payload);

    return true;
}

//if successful then return 0,if type is wrong return -1;
int ProtobufServiceMessageDecoder::decode(const ChannelBufferPtr& buffer,
        const ProtobufServiceMessagePtr& message) {
    ServiceMessage* serviceMessage = message->mutableServiceMessage();
    bool flag = false;

    // for "RPC0"
    buffer->offsetReaderIndex(4);

    while (buffer->readable() && !flag) {
        int wireType = 0;
        int fieldNum = 0;
        int fieldLength = 0;
        int64_t type = 0;
        int64_t id = 0;
        int64_t error = 0;

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
                id = ChannelBufferUtil::swapLong(id);
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

            case 8:
                decodePayload(buffer, message);
                flag = true;
                break;

            case 9:
                decodePayload(buffer, message);
                flag = true;
                break;

            default:
                break;
            }
        }
    }

    return 0;
}

ProtobufServiceMessagePtr ProtobufServiceMessageDecoder::decode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& msg) {

    if (msg) {
        ProtobufServiceMessagePtr message(new ProtobufServiceMessage);

        if (!decode(msg, message)) {
            return message;
        }
        else {
            // error here
            printf("ProtobufServiceMessageDecoder::decode failed because of type wrong");
        }
    }

    return ProtobufServiceMessagePtr();
}

}
}
}
}
