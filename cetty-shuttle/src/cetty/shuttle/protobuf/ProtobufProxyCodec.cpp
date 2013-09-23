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

#include <cetty/shuttle/protobuf/ProtobufProxyCodec.h>

#include <boost/assert.hpp>
#include <google/protobuf/message.h>

#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBufferUtil.h>
#include <cetty/protobuf/service/service.pb.h>
#include <cetty/protobuf/service/handler/MessageCodec.h>
#include <cetty/shuttle/protobuf/ProtobufProxyMessage.h>

namespace cetty {
namespace shuttle {
namespace protobuf {

using namespace cetty::buffer;
using namespace cetty::channel;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::handler;

ProtobufProxyMessagePtr ProtobufProxyCodec::decode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& msg) {
    if (msg) {
        ProtobufProxyMessagePtr message(new ProtobufProxyMessage);

        int size = msg->readableBytes();
        ChannelBufferPtr buffer = Unpooled::buffer(size + 8, 4);
        msg->getBytes(msg->readerIndex(), buffer, size);

        if (!MessageCodec::decodeMessageHead(msg, &message->message())) {
            message->setBuffer(buffer);
            return message;
        }
        else {
            LOG_WARN << "decode Message failed.";
        }
    }
    else {
        LOG_WARN << "input channel buffer empty, decode failed.";
    }

    return ProtobufProxyMessagePtr();
}

ChannelBufferPtr ProtobufProxyCodec::encode(ChannelHandlerContext& ctx,
        const ProtobufProxyMessagePtr& msg) {
    if (msg) {
        return msg->buffer();
    }
    else {
        return ChannelBufferPtr();
    }
}

}
}
}
