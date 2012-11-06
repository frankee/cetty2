
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

#include <cetty/gearman/protocol/GearmanMessageDecoder.h>

#include <cetty/channel/Channel.h>

#include <cetty/gearman/protocol/GearmanMessage.h>

namespace cetty {
namespace gearman {
namespace protocol {

using namespace cetty::channel;

//this array is  store the param-size of  gearman-type
//use a additional value of 0  at first, so use  MessageMetaInfo[type] to get the param-size
const int MessageMetaInfo[] = {
    0,1,1,0,0,
    0,0,2,1,0,
    0,2,3,1,1,
    1,0,0,2,2,
    5,2,1,2,0,
    1,1,1,1,1,
    0,0,2,2,2,
    2,2
};

GearmanMessageDecoder::GearmanMessageDecoder() {

}

GearmanMessageDecoder::~GearmanMessageDecoder() {

}

ChannelHandlerPtr GearmanMessageDecoder::clone() {
    return ChannelHandlerPtr(new GearmanMessageDecoder);
}

std::string GearmanMessageDecoder::toString() const {
    return "GearmanDecoder";
}

GearmanMessagePtr GearmanMessageDecoder::decode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& msg) {
    const ChannelBufferPtr& buffer = msg;

    if (buffer) {
        GearmanMessagePtr message(new GearmanMessage);
        int type = buffer->readInt();
        int length = buffer->readInt();

        int paramSize = 0;

        while (length > 0) {
            int bytes = buffer->bytesBefore(0);

            if (bytes > 0 && (paramSize < MessageMetaInfo[type])) {
                paramSize++;

                std::string* str = message->addParameter();
                str->reserve(bytes);

                buffer->readBytes(str, bytes);
                buffer->skipBytes(1);
            }
            else {
                //these types take the param as the last element
                if (type == GearmanMessage::JOB_CREATED || type == GearmanMessage::STATUS_RES || type == GearmanMessage::OPTION_RES ||
                        type == GearmanMessage::WORK_STATUS || type == GearmanMessage::WORK_FAIL || type == GearmanMessage::ERROR) {
                    std::string* str = message->addParameter();
                    int lastParamLen = buffer->readableBytes();
                    str->reserve(lastParamLen);
                    buffer->readBytes(str,lastParamLen);
                    break;
                }
                else if (buffer->readable()) {
                    message->setData(buffer);
                }

                break;
            }
        }

        message->setType(type);

        return message;
    }

    return GearmanMessagePtr();
}

}
}
}
