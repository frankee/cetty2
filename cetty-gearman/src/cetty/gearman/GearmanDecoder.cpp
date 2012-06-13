
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

#include <cetty/gearman/GearmanDecoder.h>

#include <cetty/channel/Channel.h>

#include <cetty/gearman/GearmanMessage.h>

namespace cetty {
namespace gearman {

using namespace cetty::channel;

GearmanDecoder::GearmanDecoder() {

}

GearmanDecoder::~GearmanDecoder() {

}

ChannelHandlerPtr GearmanDecoder::clone() {
    return ChannelHandlerPtr(new GearmanDecoder);
}

std::string GearmanDecoder::toString() const {
    return "GearmanDecoder";
}

ChannelMessage GearmanDecoder::decode(ChannelHandlerContext& ctx,
    const ChannelPtr& channel,
    const ChannelMessage& msg) {
    ChannelBufferPtr buffer = msg.smartPointer<ChannelBuffer>();

    if (buffer) {
        GearmanMessagePtr message(new GearmanMessage);
        int type = buffer->readInt();
        int length = buffer->readInt();
        while (length > 0) {
            int bytes = buffer->bytesBefore(0);
            if (bytes > 0) {
                std::string* str = message->addParameter();
                str->reserve(bytes+1);
                buffer->readBytes(str, bytes+1);
            }
            else {
                if (buffer->readable()) {
                    message->setData(buffer);
                }
                break;
            }
        }

        message->setType(type);

        return ChannelMessage(message);
    }

    return ChannelMessage::EMPTY_MESSAGE;
}

}
}
