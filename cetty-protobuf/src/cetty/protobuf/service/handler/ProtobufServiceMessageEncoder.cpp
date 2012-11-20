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

#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>

#include <boost/assert.hpp>
#include <cetty/buffer/Unpooled.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufMessageCodec.h>
#include <cetty/protobuf/service/service.pb.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::protobuf::service;

ChannelHandlerPtr ProtobufServiceMessageEncoder::clone() {
    return ChannelHandlerPtr(new ProtobufServiceMessageEncoder);
}

std::string ProtobufServiceMessageEncoder::toString() const {
    return "ProtobufServiceMessageEncoder";
}

ChannelBufferPtr ProtobufServiceMessageEncoder::encode(ChannelHandlerContext& ctx,
        const ProtobufServiceMessagePtr& msg) {
    if (msg) {
        int msgSize = msg->getMessageSize();
        ChannelBufferPtr buffer = Unpooled::buffer(msgSize + 32, 4);
        buffer->writeBytes("RPC0");
        encodeMessage(buffer, msg);

        return buffer;
    }
    else {
        return ChannelBufferPtr();
    }
}

void ProtobufServiceMessageEncoder::encodeProtobufMessage(const ChannelBufferPtr& buffer,
        const google::protobuf::Message& message) {
    int length;
    char* bytes = buffer->writableBytes(&length);
    int messageSize = message.GetCachedSize();
    message.SerializeWithCachedSizesToArray((google::protobuf::uint8*)bytes);
    buffer->offsetWriterIndex(messageSize);
}

void ProtobufServiceMessageEncoder::encodeMessage(const ChannelBufferPtr& buffer,
        const ProtobufServiceMessagePtr& message) {

    const ServiceMessage& serviceMessage = message->getServiceMessage();
    BOOST_ASSERT(!serviceMessage.has_request()
                 && !serviceMessage.has_response()
                 && "Can not contain the response or request payload in service message");

    encodeProtobufMessage(buffer, serviceMessage);

    if (serviceMessage.type() == REQUEST) {
        int payloadSize = message->getPayload()->GetCachedSize();
        ProtobufMessageCodec::encodeTag(buffer, 8, ProtobufMessageCodec::WIRETYPE_LENGTH_DELIMITED);
        ProtobufMessageCodec::encodeVarint(buffer, payloadSize);

        encodeProtobufMessage(buffer, *message->getPayload());
    }
    else if (serviceMessage.type() == RESPONSE) {
        int payloadSize = message->getPayload()->GetCachedSize();
        ProtobufMessageCodec::encodeTag(buffer, 9, ProtobufMessageCodec::WIRETYPE_LENGTH_DELIMITED);
        ProtobufMessageCodec::encodeVarint(buffer, payloadSize);

        encodeProtobufMessage(buffer, *message->getPayload());
    }
}

}
}
}
}
