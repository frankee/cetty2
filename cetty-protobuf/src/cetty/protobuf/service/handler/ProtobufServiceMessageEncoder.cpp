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

#include <cetty/channel/ChannelMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageCodec.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::channel;
using namespace cetty::protobuf::service::proto;

ChannelMessage ProtobufServiceMessageEncoder::encode(ChannelHandlerContext& ctx,
        const ChannelPtr& channel,
        const ChannelMessage& msg) {

    ProtobufServiceMessagePtr message = msg.smartPointer<ProtobufServiceMessage>();

    if (message) {
        int msgSize = message->getMessageSize();
        ChannelBufferPtr buffer = ChannelBuffers::buffer(msgSize + 8);
        encodeMessage(buffer, message);

        return ChannelMessage(buffer);
    }
    else {
        return msg;
    }
}

void ProtobufServiceMessageEncoder::encodeProtobufMessage(const ChannelBufferPtr& buffer,
        const google::protobuf::Message& message) {
    Array arry;
    buffer->writableBytes(&arry);
    int messageSize = message.GetCachedSize();
    message.SerializeWithCachedSizesToArray((google::protobuf::uint8*)arry.data());
    buffer->offsetWriterIndex(messageSize);
}

void ProtobufServiceMessageEncoder::encodeMessage(ChannelBufferPtr& buffer,
        const ProtobufServiceMessagePtr& message) {

    const ServiceMessage& serviceMessage = message->getServiceMessage();
    BOOST_ASSERT(!serviceMessage.has_request()
                 && !serviceMessage.has_response()
                 && "Can not contain the response or request payload in service message");

    encodeProtobufMessage(buffer, serviceMessage);

    if (serviceMessage.type() == REQUEST) {
        int payloadSize = message->getPayload()->GetCachedSize();
        ProtobufServiceMessageCodec::encodeTag(buffer, 6, ProtobufServiceMessageCodec::WIRETYPE_LENGTH_DELIMITED);
        ProtobufServiceMessageCodec::encodeVarint(buffer, payloadSize);

        encodeProtobufMessage(buffer, *message->getPayload());
    }
    else if (serviceMessage.type() == RESPONSE) {
        int payloadSize = message->getPayload()->GetCachedSize();
        ProtobufServiceMessageCodec::encodeTag(buffer, 7, ProtobufServiceMessageCodec::WIRETYPE_LENGTH_DELIMITED);
        ProtobufServiceMessageCodec::encodeVarint(buffer, payloadSize);

        encodeProtobufMessage(buffer, *message->getPayload());
    }
}

}
}
}
}
