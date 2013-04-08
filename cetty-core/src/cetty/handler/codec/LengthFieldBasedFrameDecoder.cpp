/*
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

#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>

#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/buffer/Unpooled.h>

#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>
#include <cetty/logging/LoggerHelper.h>

#include <cetty/handler/codec/CorruptedFrameException.h>
#include <cetty/handler/codec/TooLongFrameException.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::buffer;
using namespace cetty::util;

LengthFieldBasedFrameDecoder::LengthFieldBasedFrameDecoder(int maxFrameLength,
        int lengthFieldOffset,
        int lengthFieldLength)
    : discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0),
      bytesToDiscard_(0),
      lengthFieldOffset_(lengthFieldOffset),
      lengthFieldLength_(lengthFieldLength),
      lengthFieldEndOffset_(lengthFieldOffset + lengthFieldLength),
      lengthAdjustment_(0),
      initialBytesToStrip_(0),
      checksumFieldLength_(0),
      checksumCalcOffset_(0) {
    init();
}

LengthFieldBasedFrameDecoder::LengthFieldBasedFrameDecoder(int maxFrameLength,
        int lengthFieldOffset,
        int lengthFieldLength,
        const std::string& header1)
    : discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0),
      bytesToDiscard_(0),
      lengthFieldOffset_(lengthFieldOffset),
      lengthFieldLength_(lengthFieldLength),
      lengthFieldEndOffset_(lengthFieldOffset + lengthFieldLength),
      lengthAdjustment_(0),
      initialBytesToStrip_(0),
      checksumFieldLength_(0),
      checksumCalcOffset_(0),
      header1_(header1) {
    init();
}

LengthFieldBasedFrameDecoder::LengthFieldBasedFrameDecoder(int maxFrameLength,
        int lengthFieldOffset,
        int lengthFieldLength,
        int lengthAdjustment,
        int initialBytesToStrip)
    : discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0),
      bytesToDiscard_(0),
      lengthFieldOffset_(lengthFieldOffset),
      lengthFieldLength_(lengthFieldLength),
      lengthFieldEndOffset_(lengthFieldOffset + lengthFieldLength),
      lengthAdjustment_(lengthAdjustment),
      initialBytesToStrip_(initialBytesToStrip),
      checksumFieldLength_(0),
      checksumCalcOffset_(0) {
    init();
}

LengthFieldBasedFrameDecoder::LengthFieldBasedFrameDecoder(int maxFrameLength,
        int lengthFieldOffset,
        int lengthFieldLength,
        int lengthAdjustment,
        int initialBytesToStrip,
        const std::string& header1,
        const std::string& header2)
    :discardingTooLongFrame_(false),
     maxFrameLength_(maxFrameLength),
     tooLongFrameLength_(0),
     bytesToDiscard_(0),
     lengthFieldOffset_(lengthFieldOffset),
     lengthFieldLength_(lengthFieldLength),
     lengthFieldEndOffset_(lengthFieldOffset + lengthFieldLength),
     lengthAdjustment_(lengthAdjustment),
     initialBytesToStrip_(initialBytesToStrip),
     checksumFieldLength_(0),
     checksumCalcOffset_(0),
     header1_(header1),
     header2_(header2) {
    init();
}

LengthFieldBasedFrameDecoder::LengthFieldBasedFrameDecoder(int maxFrameLength,
        int lengthFieldOffset,
        int lengthFieldLength,
        int lengthAdjustment,
        int initialBytesToStrip,
        int checksumFieldLength,
        const ChecksumFunction& checksumFunction)
    : discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0),
      bytesToDiscard_(0),
      lengthFieldOffset_(lengthFieldOffset),
      lengthFieldLength_(lengthFieldLength),
      lengthFieldEndOffset_(lengthFieldOffset + lengthFieldLength),
      lengthAdjustment_(lengthAdjustment),
      initialBytesToStrip_(initialBytesToStrip),
      checksumFieldLength_(checksumFieldLength),
      checksumCalcOffset_(lengthFieldOffset + lengthFieldLength),
      checksumFunction_(checksumFunction) {
    init();
}

LengthFieldBasedFrameDecoder::LengthFieldBasedFrameDecoder(int maxFrameLength,
        int lengthFieldOffset,
        int lengthFieldLength,
        int lengthAdjustment,
        int initialBytesToStrip,
        int checksumFieldLength,
        const ChecksumFunction& checksumFunction,
        const std::string& header1)
    : discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0),
      bytesToDiscard_(0),
      lengthFieldOffset_(lengthFieldOffset),
      lengthFieldLength_(lengthFieldLength),
      lengthFieldEndOffset_(lengthFieldOffset + lengthFieldLength),
      lengthAdjustment_(lengthAdjustment),
      initialBytesToStrip_(initialBytesToStrip),
      checksumFieldLength_(checksumFieldLength),
      checksumCalcOffset_(lengthFieldOffset + lengthFieldLength),
      checksumFunction_(checksumFunction),
      header1_(header1) {
    init();
}

LengthFieldBasedFrameDecoder::LengthFieldBasedFrameDecoder(int maxFrameLength,
        int lengthFieldOffset,
        int lengthFieldLength,
        int lengthAdjustment,
        int initialBytesToStrip,
        int checksumFieldLength,
        const ChecksumFunction& checksumFunction,
        const std::string& header1,
        const std::string& header2)
    : discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0),
      bytesToDiscard_(0),
      lengthFieldOffset_(lengthFieldOffset),
      lengthFieldLength_(lengthFieldLength),
      lengthFieldEndOffset_(lengthFieldOffset + lengthFieldLength),
      lengthAdjustment_(lengthAdjustment),
      initialBytesToStrip_(initialBytesToStrip),
      checksumFieldLength_(checksumFieldLength),
      checksumCalcOffset_(lengthFieldOffset + lengthFieldLength),
      checksumFunction_(checksumFunction),
      header1_(header1),
      header2_(header2) {
    init();
}

LengthFieldBasedFrameDecoder::LengthFieldBasedFrameDecoder(int maxFrameLength,
        int lengthFieldOffset,
        int lengthFieldLength,
        int lengthAdjustment,
        int initialBytesToStrip,
        int checksumFieldLength,
        int checksumCalcOffset,
        const ChecksumFunction& checksumFunction)
    : discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0),
      bytesToDiscard_(0),
      lengthFieldOffset_(lengthFieldOffset),
      lengthFieldLength_(lengthFieldLength),
      lengthFieldEndOffset_(lengthFieldOffset + lengthFieldLength),
      lengthAdjustment_(lengthAdjustment),
      initialBytesToStrip_(initialBytesToStrip),
      checksumFieldLength_(checksumFieldLength),
      checksumCalcOffset_(checksumCalcOffset),
      checksumFunction_(checksumFunction) {
    init();
}

LengthFieldBasedFrameDecoder::LengthFieldBasedFrameDecoder(int maxFrameLength,
        int lengthFieldOffset,
        int lengthFieldLength,
        int lengthAdjustment,
        int initialBytesToStrip,
        int checksumFieldLength,
        int checksumCalcOffset,
        const ChecksumFunction& checksumFunction,
        const std::string& header1)
    : discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0),
      bytesToDiscard_(0),
      lengthFieldOffset_(lengthFieldOffset),
      lengthFieldLength_(lengthFieldLength),
      lengthFieldEndOffset_(lengthFieldOffset + lengthFieldLength),
      lengthAdjustment_(lengthAdjustment),
      initialBytesToStrip_(initialBytesToStrip),
      checksumFieldLength_(checksumFieldLength),
      checksumCalcOffset_(checksumCalcOffset),
      checksumFunction_(checksumFunction),
      header1_(header1) {
    init();
}

LengthFieldBasedFrameDecoder::LengthFieldBasedFrameDecoder(int maxFrameLength,
        int lengthFieldOffset,
        int lengthFieldLength,
        int lengthAdjustment,
        int initialBytesToStrip,
        int checksumFieldLength,
        int checksumCalcOffset,
        const ChecksumFunction& checksumFunction,
        const std::string& header1,
        const std::string& header2)
    : discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0),
      bytesToDiscard_(0),
      lengthFieldOffset_(lengthFieldOffset),
      lengthFieldLength_(lengthFieldLength),
      lengthFieldEndOffset_(lengthFieldOffset + lengthFieldLength),
      lengthAdjustment_(lengthAdjustment),
      initialBytesToStrip_(initialBytesToStrip),
      checksumFieldLength_(checksumFieldLength),
      checksumCalcOffset_(checksumCalcOffset),
      checksumFunction_(checksumFunction),
      header1_(header1),
      header2_(header2) {
    init();
}

ChannelBufferPtr LengthFieldBasedFrameDecoder::decode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& in) {
    if (discardingTooLongFrame_) {
        int bytesToDiscard = this->bytesToDiscard_;
        int localBytesToDiscard = std::min(bytesToDiscard, in->readableBytes());
        in->skipBytes(localBytesToDiscard);
        bytesToDiscard -= localBytesToDiscard;
        this->bytesToDiscard_ = bytesToDiscard;

        if (bytesToDiscard == 0) {
            // Reset to the initial state and tell the handlers that
            // the frame was too large.
            // TODO Let user choose when the exception should be raised - early or late?
            //      If early, fail() should be called when discardingTooLongFrame is set to true.
            int tooLongFrameLength = this->tooLongFrameLength_;
            this->tooLongFrameLength_ = 0;
            fail(ctx, tooLongFrameLength);
        }
        else {
            // Keep discarding.
        }

        return ChannelBufferPtr();
    }

    if (in->readableBytes() < lengthFieldEndOffset_) {
        return ChannelBufferPtr();
    }

    int actualLengthFieldOffset = in->readerIndex() + lengthFieldOffset_;

    int frameLength;

    switch (lengthFieldLength_) {
    case 1:
        frameLength = in->getUnsignedByte(actualLengthFieldOffset);
        break;

    case 2:
        frameLength = in->getUnsignedShort(actualLengthFieldOffset);
        break;

    case 4:
        frameLength = static_cast<int>(in->getUnsignedInt(actualLengthFieldOffset));
        break;

    case 8:
        frameLength = static_cast<int>(in->getLong(actualLengthFieldOffset));
        break;

    default:
        throw SystemException("should not reach here");
    }

    if (frameLength < 0) {
        in->skipBytes(lengthFieldEndOffset_);
        throw CorruptedFrameException(
            StringUtil::printf("negative pre-adjustment length field: %d", frameLength));
    }

    frameLength += lengthAdjustment_ + lengthFieldEndOffset_;

    if (frameLength < lengthFieldEndOffset_) {
        in->skipBytes(lengthFieldEndOffset_);
        throw CorruptedFrameException(
            StringUtil::printf(
                "Adjusted frame length (%d)  is less than lengthFieldEndOffset: %d",
                frameLength,
                lengthFieldEndOffset_));
    }

    if (frameLength > maxFrameLength_) {
        // Enter the discard mode and discard everything received so far.
        discardingTooLongFrame_ = true;
        tooLongFrameLength_ = frameLength;
        bytesToDiscard_ = frameLength - in->readableBytes();
        in->skipBytes(in->readableBytes());
        return ChannelBufferPtr();
    }

    // never overflows because it's less than maxFrameLength
    if (in->readableBytes() < frameLength) {
        return ChannelBufferPtr();
    }

    if (initialBytesToStrip_ > frameLength) {
        in->skipBytes(frameLength);
        std::string msg;
        StringUtil::printf(&msg,
                           "Adjusted frame length (%d) is less than initialBytesToStrip: %d",
                           frameLength, initialBytesToStrip_);
        throw CorruptedFrameException(msg);
    }

    in->skipBytes(initialBytesToStrip_);

    // extract frame
    int readerIndex = in->readerIndex();
    int actualFrameLength = frameLength - initialBytesToStrip_ - checksumFieldLength_;
    ChannelBufferPtr frame = extractFrame(in, readerIndex, actualFrameLength);
    in->readerIndex(readerIndex + actualFrameLength + checksumFieldLength_);
    return frame;
}

ChannelBufferPtr
LengthFieldBasedFrameDecoder::extractFrame(const ChannelBufferPtr& buffer,
        int index,
        int length) {
    ChannelBufferPtr frame = Unpooled::buffer(length);
    frame->writeBytes(buffer, index, length);
    return frame;
}

void LengthFieldBasedFrameDecoder::fail(ChannelHandlerContext& ctx,
                                        int frameLength) {
    std::string msg;

    if (frameLength > 0) {
        StringUtil::printf(&msg,
                           "Adjusted frame length exceeds %d: % - discarded.",
                           maxFrameLength_, frameLength);
        ctx.fireExceptionCaught(TooLongFrameException(msg));
    }
    else {
        StringUtil::printf("Adjusted frame length exceeds %d - discarded.",
                           maxFrameLength_);
        ctx.fireExceptionCaught(TooLongFrameException(msg));
    }
}

void LengthFieldBasedFrameDecoder::validateParameters() {
    std::string msg;

    if (maxFrameLength_ <= 0) {
        StringUtil::printf(&msg,
                           "maxFrameLength must be a positive integer: %d",
                           maxFrameLength_);

        LOG_ERROR << msg;
        throw InvalidArgumentException(msg);
    }

    if (lengthFieldOffset_ < 0) {
        StringUtil::printf(&msg,
                           "lengthFieldOffset must be a non-negative integer: %d",
                           lengthFieldOffset_);

        LOG_ERROR << msg;
        throw InvalidArgumentException(msg);
    }

    if (initialBytesToStrip_ < 0) {
        StringUtil::printf(&msg,
                           "initialBytesToStrip must be a non-negative integer: %d",
                           initialBytesToStrip_);

        LOG_ERROR << msg;
        throw InvalidArgumentException(msg);
    }

    if (checksumFieldLength_ < 0) {
        StringUtil::printf(&msg,
                           "checksumFieldLength must be a non-negative integer: %d",
                           checksumFieldLength_);

        LOG_ERROR << msg;
        throw InvalidArgumentException(msg);
    }

    if (checksumCalcOffset_ < 0) {
        StringUtil::printf(&msg,
                           "checksumFieldOffset must be a non-negative integer:  %d",
                           checksumCalcOffset_);

        LOG_ERROR << msg;
        throw InvalidArgumentException(msg);
    }

    if (checksumFieldLength_ > 0 && !checksumFunction_) {
        msg = "you set the checksumField but no checksum function set.";

        LOG_ERROR << msg;
        throw InvalidArgumentException(msg);
    }

    if (lengthFieldLength_ != 1 && lengthFieldLength_ != 2 &&
            lengthFieldLength_ != 4 && lengthFieldLength_ != 8) {
        StringUtil::printf(&msg,
                           "lengthFieldLength must be either 1, 2, 4, or 8:  %d",
                           lengthFieldLength_);

        LOG_ERROR << msg;
        throw InvalidArgumentException(msg);
    }

    if (lengthFieldOffset_ > maxFrameLength_ - lengthFieldLength_) {
        StringUtil::printf(&msg,
                           "maxFrameLength (%d) must be equal to or greater "
                           "than lengthFieldOffset (%d) + lengthFieldLength (%d).",
                           maxFrameLength_, lengthFieldOffset_, lengthFieldLength_);

        LOG_ERROR << msg;
        throw InvalidArgumentException(msg);
    }
}

void LengthFieldBasedFrameDecoder::init() {
    validateParameters();

    decoder_.setDecoder(boost::bind(&LengthFieldBasedFrameDecoder::decode,
                                    this,
                                    _1,
                                    _2));
}

}
}
}
