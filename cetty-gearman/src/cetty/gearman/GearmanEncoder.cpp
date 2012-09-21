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

#include <cetty/gearman/GearmanEncoder.h>
#include <cetty/gearman/GearmanMessage.h>

#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/ChannelBufferUtil.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace gearman {

using namespace cetty::channel;
using namespace cetty::buffer;

GearmanEncoder::GearmanEncoder() {

}

GearmanEncoder::~GearmanEncoder() {

}

ChannelHandlerPtr GearmanEncoder::clone() {
    return ChannelHandlerPtr(new GearmanEncoder);
}

std::string GearmanEncoder::toString() const {
    return "GearmanEncoder";
}

ChannelBufferPtr GearmanEncoder::encode(ChannelHandlerContext& ctx,
                                        const GearmanMessagePtr& msg) {

    if (!msg) {
        return ChannelBufferPtr();
    }

    int parametersLength = caculateParametersLength(msg);
    int headerLength = 12;
    int bodyLenth = 0;
    int messageLength = 0;

    if (msg->hasData()) {
        const ChannelBufferPtr& data = msg->getData();
        bodyLenth += data->readableBytes();
        messageLength = bodyLenth + parametersLength;

        if (data->aheadWritableBytes() >= parametersLength + headerLength) {
            if (parametersLength) {
                writeParametersAhead(data, msg->getParameters(), true);
            }

            writeHeaderAhead(data, msg->getType(), messageLength);
            DLOG_DEBUG << "the send data is " << ChannelBufferUtil::hexDump(data);
            return data;
        }
        else {
            ChannelBufferPtr buffer = Unpooled::buffer(messageLength + headerLength);

            writeHeader(buffer, msg->getType(), messageLength);

            if (parametersLength) {
                writeParameters(buffer, msg->getParameters(), true);
            }

            buffer->writeBytes(data);
            DLOG_DEBUG << "the send data is " << ChannelBufferUtil::hexDump(data);
            return buffer;
        }
    }
    else {
        if (msg->getParameters().size() != 0) {
            messageLength =  parametersLength - 1;
        }
        else {
            messageLength =  parametersLength;
        }

        ChannelBufferPtr buffer = Unpooled::buffer(messageLength+headerLength);
        writeHeader(buffer, msg->getType(), messageLength);

        if ((parametersLength - 1) > 0) {
            writeParameters(buffer, msg->getParameters(), false);
        }
        DLOG_DEBUG << "the send data is " << ChannelBufferUtil::hexDump(buffer);
        
        return buffer;
    }

    return ChannelBufferPtr();
}

int GearmanEncoder::caculateParametersLength(const GearmanMessagePtr& msg) {
    int length = 0;

    const std::vector<std::string>& parameters = msg->getParameters();

    for (int i = 0, j = parameters.size(); i < j; ++i) {
        length += parameters[i].size();
        length += 1;
    }

    return length;
}

void GearmanEncoder::writeHeader(const ChannelBufferPtr& buffer,
    int type,
    int length) {
    buffer->writeBytes(GearmanMessage::REQUEST_MAGIC);
    buffer->writeInt(type);
    buffer->writeInt(length);
}

void GearmanEncoder::writeHeaderAhead(const ChannelBufferPtr& buffer,
    int type,
    int length) {
    buffer->writeIntAhead(length);
    buffer->writeIntAhead(type);
    buffer->writeBytesAhead(GearmanMessage::REQUEST_MAGIC);
}

void GearmanEncoder::writeParameters(const ChannelBufferPtr& buffer,
                                     const std::vector<std::string>& parameters,
                                     bool withZeroPad) {
    int j = parameters.size();

    for (int i = 0; i < j; ++i) {
        buffer->writeBytes(parameters[i]);

        if (!withZeroPad && ((j - i) == 1)) {
            // last one no zero pad
            break;
        }

        buffer->writeByte(0);
    }
}

void GearmanEncoder::writeParametersAhead(const ChannelBufferPtr& buffer,
        const std::vector<std::string>& parameters,
        bool withZeroPad) {
    int j = parameters.size();

    if (j == 0) { return; }

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
