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

#include <cetty/handler/codec/LengthFieldPrepender.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/Unpooled.h>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;

enum {
    HEADER_FIELD_NONE  = 0,
    HEADER_FIELD_FIRST = 1,
    HEADER_FIELD_LAST  = 2
};

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength)
    : lengthFieldLength_(lengthFieldLength),
      lengthFieldOffset_(0),
      lengthAdjustment_(0),
      checksumFieldLength_(0),
      checksumCalcOffset_(0) {
    validateParameters();
}

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength,
        int checksumFieldLength,
        const ChecksumFunction& checksum)
    : lengthFieldLength_(lengthFieldLength),
      lengthFieldOffset_(0),
      lengthAdjustment_(0),
      checksumFieldLength_(checksumFieldLength),
      checksumCalcOffset_(lengthFieldLength),
      checksumFunction_(checksum) {
    validateParameters();
}

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength,
        int lengthFieldOffset,
        const std::string& header)
    : lengthFieldLength_(lengthFieldLength),
      lengthFieldOffset_(lengthFieldOffset),
      lengthAdjustment_(lengthFieldOffset ? 0 : header.size()),
      header_(header),
      checksumFieldLength_(0),
      checksumCalcOffset_(0) {
    validateParameters();
}

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength,
        const std::string& header)
    : lengthFieldLength_(lengthFieldLength),
      lengthFieldOffset_(header.size()),
      lengthAdjustment_(0),
      header_(header),
      checksumFieldLength_(0),
      checksumCalcOffset_(0) {
    validateParameters();
}

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength,
        int lengthFieldOffset,
        int lengthAdjustment,
        const std::string& header)
    : lengthFieldLength_(lengthFieldLength),
      lengthFieldOffset_(lengthFieldOffset),
      lengthAdjustment_(lengthAdjustment),
      header_(header),
      checksumFieldLength_(0),
      checksumCalcOffset_(0) {
    validateParameters();
}

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength,
        const std::string& header,
        int checksumFieldLength,
        const ChecksumFunction& checksum)
    : lengthFieldLength_(lengthFieldLength),
      lengthFieldOffset_(header.size()),
      lengthAdjustment_(0),
      header_(header),
      checksumFieldLength_(checksumFieldLength),
      checksumCalcOffset_(lengthFieldOffset_ + lengthFieldLength),
      checksumFunction_(checksumFunction_) {
    validateParameters();
}

LengthFieldPrepender::LengthFieldPrepender(int lengthFieldLength,
        int lengthFieldOffset,
        int lengthAdjustment,
        int checksumFieldLength,
        const std::string& header,
        ChecksumFunction checksumFunction)
    : lengthFieldLength_(lengthFieldLength),
      lengthFieldOffset_(lengthFieldOffset),
      lengthAdjustment_(lengthAdjustment),
      header_(header),
      checksumFieldLength_(checksumFieldLength),
      checksumCalcOffset_(lengthFieldOffset + lengthFieldLength),
      checksumFunction_(checksumFunction) {
    validateParameters();
}

ChannelBufferPtr LengthFieldPrepender::encode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& msg,
        const ChannelBufferPtr& out) {

    BOOST_ASSERT(msg);

    bool hasOutBuffer = !!out;
    int headerFieldLength = (int)header_.size();
    int headerLength = lengthFieldLength_ + headerFieldLength;

    int msgLength = headerLength + msg->readableBytes() + checksumFieldLength_;
    int contentLength = msgLength - (lengthFieldOffset_ + lengthFieldLength_ - lengthAdjustment_);

    uint32_t cs = 0;

    if (checksumFieldLength_ > 0) {
        StringPiece bytes;
        msg->readableBytes(&bytes);
        BOOST_ASSERT(!bytes.empty());

        if (checksumCalcOffset_ == headerLength) {
            cs = checksumFunction_((const uint8_t*)bytes.data(), bytes.length());
        }
        else {
            //TODO
        }
    }

    int headerPos = headerFieldLength ?
                    (lengthFieldOffset_ ? HEADER_FIELD_FIRST : HEADER_FIELD_LAST)
                        : HEADER_FIELD_NONE;

    if (hasOutBuffer) {
        return writeMessage(out, msg, contentLength, headerPos, cs);
    }
    else {
        if (msg->aheadWritableBytes() >= headerLength &&
                (checksumFieldLength_ ? msg->writableBytes() >= checksumFieldLength_ : true)) {
            preWriteHeader(msg, contentLength, headerPos);

            if (2 == checksumFieldLength_) {
                msg->writeShort(cs);
            }
            else if (4 == checksumFieldLength_) {
                msg->writeInt(cs);
            }

            return msg;
        }
        else {
            ChannelBufferPtr buffer = Unpooled::buffer(msgLength);
            return writeMessage(buffer, msg, contentLength, headerPos, cs);
        }
    }
}

void LengthFieldPrepender::validateParameters() {
    if (lengthFieldLength_ != 1 && lengthFieldLength_ != 2 &&
            lengthFieldLength_ != 4 && lengthFieldLength_ != 8) {
        std::string msg;
        StringUtil::printf(&msg,
                           "lengthFieldLength must be either 1, 2, 4, or 8: %d",
                           lengthFieldLength_);
        throw InvalidArgumentException(msg);
    }

    if (checksumFieldLength_ != 0 && checksumFieldLength_ != 2 && checksumFieldLength_ != 4) {
        std::string msg;
        StringUtil::printf(&msg,
                           "checksumFieldLength must be either 2 or 4: %d",
                           checksumFieldLength_);

        throw InvalidArgumentException(msg);
    }

    int preHeaderFieldLength = (int)header_.size();

    if (lengthFieldOffset_ != 0 && lengthFieldOffset_ != preHeaderFieldLength
            && lengthFieldOffset_ != (preHeaderFieldLength + lengthFieldLength_)) {
        std::string msg;
        StringUtil::printf(&msg,
                           "lengthFieldOffset must be either 0, preHeaderFieldLength, "
                           "lengthFieldLength : %d",
                           lengthFieldLength_);
        throw InvalidArgumentException(msg);
    }

    if (checksumFieldLength_ > 0) {//enable checksum
        if (!checksumFunction_) {
            throw InvalidArgumentException(
                std::string("you must set the checksum function if you enable checksum."));
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

void LengthFieldPrepender::writeHeader(const ChannelBufferPtr& msg, int contentLength, int headerPos) {
    if (HEADER_FIELD_FIRST == headerPos) {
        msg->writeBytes(header_);
    }

    switch (lengthFieldLength_) {
    case 1:
        if (contentLength >= 256) {
            throw InvalidArgumentException(
                StringUtil::printf("length does not fit into a byte: %d",
                                   contentLength));
        }

        msg->writeByte(contentLength);
        break;

    case 2:
        if (contentLength >= 65536) {
            throw InvalidArgumentException(
                StringUtil::printf("length does not fit into a short integer: %d",
                                   contentLength));
        }

        msg->writeShort(contentLength);
        break;

    case 4:
        msg->writeInt(contentLength);
        break;

    case 8:
        msg->writeLong(contentLength);
        break;

    default:
        throw RuntimeException("should not reach here");
    }

    if (HEADER_FIELD_LAST == headerPos) {
        msg->writeBytes(header_);
    }
}

void LengthFieldPrepender::preWriteHeader(const ChannelBufferPtr& msg, int contentLength, int headerPos) {
    if (HEADER_FIELD_LAST == headerPos) {
        msg->writeBytesAhead(header_);
    }

    switch (lengthFieldLength_) {
    case 1:
        if (contentLength >= 256) {
            throw InvalidArgumentException(
                StringUtil::printf("length does not fit into a byte: %d",
                                   contentLength));
        }

        msg->writeByteAhead(contentLength);
        break;

    case 2:
        if (contentLength >= 65536) {
            throw InvalidArgumentException(
                StringUtil::printf("length does not fit into a short integer: %d",
                                   contentLength));
        }

        msg->writeShortAhead(contentLength);
        break;

    case 4:
        msg->writeIntAhead(contentLength);
        break;

    case 8:
        msg->writeLongAhead(contentLength);
        break;

    default:
        throw RuntimeException("should not reach here");
    }

    if (HEADER_FIELD_FIRST == headerPos) {
        msg->writeBytesAhead(header_);
    }
}

const ChannelBufferPtr& LengthFieldPrepender::writeMessage(const ChannelBufferPtr& out,
        const ChannelBufferPtr& msg,
        int contentLength,
        int headerPos,
        uint32_t cs) {
    writeHeader(out, contentLength, headerPos);
    msg->readBytes(out, contentLength - checksumFieldLength_);

    if (2 == checksumFieldLength_) {
        out->writeShort(cs);
    }
    else if (4 == checksumFieldLength_) {
        out->writeInt(cs);
    }

    return out;
}

void LengthFieldPrepender::init() {
    validateParameters();

    encoder_.setEncoder(boost::bind(&LengthFieldPrepender::encode,
        this,
        _1,
        _2,
        _3));
}

}
}
}
