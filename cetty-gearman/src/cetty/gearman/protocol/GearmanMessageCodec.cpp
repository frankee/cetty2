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

#include <cetty/gearman/protocol/GearmanMessageCodec.h>
#include <cetty/gearman/protocol/GearmanMessage.h>

#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/ChannelBufferUtil.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace gearman {
namespace protocol {

using namespace cetty::channel;
using namespace cetty::buffer;

//this array is store the param-size of gearman-type
//use a additional value of 0 at first, so use MessageMetaInfo[type] to get the param-size
static const int MESSAGE_PARAM_SIZE[] = {
    0,1,1,0,0,
    0,0,2,1,0,
    0,2,3,1,1,
    1,0,0,2,2,
    5,2,1,2,0,
    1,1,1,1,1,
    0,0,2,2,2,
    2,2
};

GearmanMessageCodec::GearmanMessageCodec() {
    decoder_.setDecoder(boost::bind(&GearmanMessageCodec::decode,
                                    this,
                                    _1,
                                    _2));

    encoder_.setEncoder(boost::bind(&GearmanMessageCodec::encode,
                                    this,
                                    _1,
                                    _2));
}

GearmanMessageCodec::~GearmanMessageCodec() {
}

GearmanMessagePtr GearmanMessageCodec::decode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& msg) {
    const ChannelBufferPtr& buffer = msg;

    if (buffer) {
        GearmanMessagePtr message(new GearmanMessage);
        int type = buffer->readInt();
        int length = buffer->readInt();

        int paramSize = 0;

        while (length > 0) {
            int bytes = buffer->bytesBefore(0);

            if (bytes > 0 && (paramSize < MESSAGE_PARAM_SIZE[type])) {
                paramSize++;

                std::string* str = message->addParameter();
                str->reserve(bytes);

                buffer->readBytes(str, bytes);
                buffer->skipBytes(1);
            }
            else {
                //these types take the param as the last element
                if (type == GearmanMessage::JOB_CREATED ||
                        type == GearmanMessage::STATUS_RES ||
                        type == GearmanMessage::OPTION_RES ||
                        type == GearmanMessage::WORK_STATUS ||
                        type == GearmanMessage::WORK_FAIL ||
                        type == GearmanMessage::ERROR) {
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

ChannelBufferPtr GearmanMessageCodec::encode(ChannelHandlerContext& ctx,
        const GearmanMessagePtr& msg) {

    if (!msg) {
        return ChannelBufferPtr();
    }

    int headerLength = 12;
    int bodyLenth = 0;
    int messageLength = 0;
    int parametersLength = caculateParametersLength(msg);

    if (msg->hasData()) {
        const ChannelBufferPtr& data = msg->data();
        bodyLenth = data->readableBytes();
        messageLength = bodyLenth + parametersLength;

        if (data->aheadWritableBytes() >= parametersLength + headerLength) {
            if (parametersLength > 0) {
                writeParametersAhead(data, msg->parameters(), true);
            }

            writeHeaderAhead(data, msg->type(), messageLength);
            DLOG_DEBUG << "command dump: " << ChannelBufferUtil::hexDump(data);
            return data;
        }
        else {
            ChannelBufferPtr buffer = Unpooled::buffer(headerLength + messageLength);
            writeHeader(buffer, msg->type(), messageLength);

            if (parametersLength > 0) {
                writeParameters(buffer, msg->parameters(), true);
            }

            buffer->writeBytes(data);
            DLOG_DEBUG << "command dump: " << ChannelBufferUtil::hexDump(buffer);
            return buffer;
        }
    }
    else {
        if (parametersLength > 0) {
            messageLength = parametersLength - 1; // remove the last zero pad
        }

        ChannelBufferPtr buffer = Unpooled::buffer(headerLength + messageLength);
        writeHeader(buffer, msg->type(), messageLength);

        if (parametersLength > 1) {
            writeParameters(buffer, msg->parameters(), false);
        }

        DLOG_DEBUG << "command dump: " << ChannelBufferUtil::hexDump(buffer);
        return buffer;
    }

    return ChannelBufferPtr();
}

int GearmanMessageCodec::caculateParametersLength(const GearmanMessagePtr& msg) {
    std::size_t length = 0;
    const std::vector<std::string>& parameters = msg->parameters();

    for (std::size_t i = 0, j = parameters.size(); i < j; ++i) {
        length += parameters[i].size();
        length += 1;
    }

    return static_cast<int>(length);
}

void GearmanMessageCodec::writeHeader(const ChannelBufferPtr& buffer,
                                      int type,
                                      int length) {
    buffer->writeBytes(GearmanMessage::REQUEST_MAGIC);
    buffer->writeInt(type);
    buffer->writeInt(length);
}

void GearmanMessageCodec::writeHeaderAhead(const ChannelBufferPtr& buffer,
        int type,
        int length) {
    buffer->writeIntAhead(length);
    buffer->writeIntAhead(type);
    buffer->writeBytesAhead(GearmanMessage::REQUEST_MAGIC);
}

void GearmanMessageCodec::writeParameters(const ChannelBufferPtr& buffer,
        const std::vector<std::string>& parameters,
        bool withZeroPad) {
    std::size_t j = parameters.size();

    for (std::size_t i = 0; i < j; ++i) {
        buffer->writeBytes(parameters[i]);

        if (!withZeroPad && ((j - i) == 1)) {
            // last one no zero pad
            break;
        }

        buffer->writeByte(0);
    }
}

void GearmanMessageCodec::writeParametersAhead(const ChannelBufferPtr& buffer,
        const std::vector<std::string>& parameters,
        bool withZeroPad) {
    int j = static_cast<int>(parameters.size());

    if (j == 0) {
        return;
    }

    if (withZeroPad) {
        buffer->writeByteAhead(0);
    }

    buffer->writeBytesAhead(parameters[j - 1]);

    for (int i = j - 2; i >= 0; --i) {
        buffer->writeByteAhead(0);
        buffer->writeBytesAhead(parameters[i]);
    }
}

}
}
}
