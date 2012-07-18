/**
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/handler/codec/frame/LengthFieldPrepender.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/buffer/ChannelBufferFactory.h>

namespace cetty {
namespace handler {
namespace codec {
namespace frame {

using namespace cetty::channel;
using namespace cetty::buffer;

cetty::channel::ChannelMessage LengthFieldPrepender::encode(ChannelHandlerContext& ctx,
        const ChannelPtr& channel,
        const ChannelMessage& msg) {
    ChannelBufferPtr body = msg.smartPointer<ChannelBuffer>();

    if (!body) {
        return msg;
    }

    int preHeaderFieldLength = (int)header1.size();
    int headerLength = lengthFieldLength + preHeaderFieldLength;

    ChannelBufferPtr header = channel->getConfig().getBufferFactory()->getBuffer(body->order(), headerLength);

    int length = body->readableBytes() + checksumFieldLength;

    //if (lengthIncludesLengthFieldLength) {
    //    length += headerLength - lengthFieldOffset;
    //}

    if (preHeaderFieldLength > 0) {
        header->writeBytes(header1);
    }

    switch (lengthFieldLength) {
    case 1:
        if (length >= 256) {
            throw InvalidArgumentException(
                std::string("length does not fit into a byte: ") + Integer::toString(length));
        }

        header->writeByte(length);
        break;

    case 2:
        if (length >= 65536) {
            throw InvalidArgumentException(
                std::string("length does not fit into a short integer: ") + Integer::toString(length));
        }

        header->writeShort(length);
        break;

    case 4:
        header->writeInt(length);
        break;

    case 8:
        header->writeLong(length);
        break;

    default:
        throw RuntimeException("should not reach here");
    }

    ChannelBufferPtr checksum;

    if (checksumFieldLength > 0) {
        boost::uint32_t cs = 0;
        Array arry;
        body->readableBytes(&arry);
        BOOST_ASSERT(!arry.empty());

        if (checksumCalcOffset == headerLength) {
            cs = checksumFunction((const boost::uint8_t*)arry.data(), arry.length());
        }
        else {
            //TODO
        }

        if (body->writableBytes() >= checksumFieldLength) {
            if (2 == checksumFieldLength) {
                body->writeShort(cs);
            }
            else if (4 == checksumFieldLength) {
                body->writeInt(cs);
            }
            else {
                throw RuntimeException("should not reach here");
            }
        }
        else {
            checksum = channel->getConfig().getBufferFactory()->getBuffer(body->order(), checksumFieldLength);

            if (2 == checksumFieldLength) {
                checksum->writeShort(cs);
            }
            else if (4 == checksumFieldLength) {
                checksum->writeInt(cs);
            }
            else {
                throw RuntimeException("should not reach here");
            }
        }
    }

    return ChannelMessage(ChannelBuffers::wrappedBuffer(header, body, checksum));
}

void LengthFieldPrepender::validateParameters() {
    if (lengthFieldLength != 1 && lengthFieldLength != 2 &&
            lengthFieldLength != 4 && lengthFieldLength != 8) {
        throw InvalidArgumentException(
            std::string("lengthFieldLength must be either 1, 2, 4, or 8: ") +
            Integer(lengthFieldLength).toString());
    }

    if (checksumFieldLength != 0 && checksumFieldLength != 2 && checksumFieldLength != 4) {
        throw InvalidArgumentException(
            std::string("checksumFieldLength must be either 2 or 4: ") +
            Integer(checksumFieldLength).toString());
    }

    int preHeaderFieldLength = (int)header1.size();

    if (lengthFieldOffset != 0 && lengthFieldOffset != preHeaderFieldLength
            && lengthFieldOffset != (preHeaderFieldLength + lengthFieldLength)) {
        throw InvalidArgumentException(
            std::string("lengthFieldOffset must be either 0, preHeaderFieldLength, lengthFieldLength : ") +
            Integer(lengthFieldOffset).toString());
    }

    if (checksumFieldLength > 0) {//enable checksum
        if (!checksumFunction) {
            throw InvalidArgumentException(std::string("you must set the checksum function if you enable checksum."));
        }

        /*
        if (checksumFieldOffset != 0 && checksumFieldOffset != preHeaderFieldLength
            && checksumFieldOffset < (preHeaderFieldLength + lengthFieldLength)) {
                throw InvalidArgumentException(
                    std::string("lengthFieldOffset must be either 0, preHeaderFieldLength, lengthFieldLength : ") +
                    Integer(lengthFieldOffset).toString());
                    */
    }
}

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength) : lengthFieldLength(lengthFieldLength),
    lengthFieldOffset(0),
    lengthAdjustment(lengthFieldLength),
    checksumFieldLength(0),
    checksumCalcOffset(0) {
    validateParameters();
}

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength, int lengthFieldOffset, const std::string& header1) : lengthFieldLength(lengthFieldLength),
    lengthFieldOffset(lengthFieldOffset),
    lengthAdjustment(0),
    header1(header1),
    checksumFieldLength(0),
    checksumCalcOffset(0) {
    validateParameters();
}

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength, const std::string& header1) : lengthFieldLength(lengthFieldLength),
    lengthFieldOffset(lengthFieldLength + header1.size()),
    lengthAdjustment(0),
    header1(header1),
    checksumFieldLength(0),
    checksumCalcOffset(0) {
    validateParameters();
}

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength, int lengthFieldOffset, int lengthAdjustment, const std::string& header1) {

}

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength, int lengthFieldLengthOffset, int lengthAdjustment, const std::string& header1, const std::string& header2) :   lengthFieldLength(lengthFieldLength),
    lengthFieldOffset(lengthFieldOffset),
    lengthAdjustment(lengthAdjustment),
    header1(header1),
    header2(header2),
    checksumFieldLength(0),
    checksumCalcOffset(0) {
    validateParameters();
}

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength, int lengthFieldOffset, int lengthAdjustment, int checksumFieldLength, int checksumCalcOffset, const std::string& header1, const std::string& header2, ChecksumFunction checksumFunction) :   lengthFieldLength(lengthFieldLength),
    lengthFieldOffset(lengthFieldOffset),
    checksumFieldLength(checksumFieldLength),
    checksumCalcOffset(lengthFieldOffset + lengthFieldLength),
    checksumFunction(checksumFunction) {
    validateParameters();
}








}
}
}
}
