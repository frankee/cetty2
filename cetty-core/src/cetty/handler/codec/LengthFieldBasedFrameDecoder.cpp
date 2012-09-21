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
#include <cetty/util/Integer.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/Exception.h>
#include <cetty/logging/LoggerHelper.h>

#include <cetty/handler/codec/CorruptedFrameException.h>
#include <cetty/handler/codec/TooLongFrameException.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;

ChannelHandlerPtr LengthFieldBasedFrameDecoder::clone() {
    return ChannelHandlerPtr(new LengthFieldBasedFrameDecoder(*this));
}

ChannelBufferPtr LengthFieldBasedFrameDecoder::decode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& in) {
    if (discardingTooLongFrame) {
        int bytesToDiscard = this->bytesToDiscard;
        int localBytesToDiscard = std::min(bytesToDiscard, in->readableBytes());
        in->skipBytes(localBytesToDiscard);
        bytesToDiscard -= localBytesToDiscard;
        this->bytesToDiscard = bytesToDiscard;

        if (bytesToDiscard == 0) {
            // Reset to the initial state and tell the handlers that
            // the frame was too large.
            // TODO Let user choose when the exception should be raised - early or late?
            //      If early, fail() should be called when discardingTooLongFrame is set to true.
            int tooLongFrameLength = this->tooLongFrameLength;
            this->tooLongFrameLength = 0;
            fail(ctx, tooLongFrameLength);
        }
        else {
            // Keep discarding.
        }

        return ChannelBufferPtr();
    }

    if (in->readableBytes() < lengthFieldEndOffset) {
        return ChannelBufferPtr();
    }

    int actualLengthFieldOffset = in->readerIndex() + lengthFieldOffset;

    int frameLength;

    switch (lengthFieldLength) {
    case 1:
        frameLength = in->getUnsignedByte(actualLengthFieldOffset);
        break;

    case 2:
        frameLength = in->getUnsignedShort(actualLengthFieldOffset);
        break;

    case 4:
        frameLength = (int)in->getUnsignedInt(actualLengthFieldOffset);
        break;

    case 8:
        frameLength = (int)in->getLong(actualLengthFieldOffset);
        break;

    default:
        throw SystemException("should not reach here");
    }

    if (frameLength < 0) {
        in->skipBytes(lengthFieldEndOffset);
        throw CorruptedFrameException(
            StringUtil::strprintf("negative pre-adjustment length field: %d", frameLength));
    }

    frameLength += lengthAdjustment + lengthFieldEndOffset;

    if (frameLength < lengthFieldEndOffset) {
        in->skipBytes(lengthFieldEndOffset);
        throw CorruptedFrameException(
            StringUtil::strprintf(
                "Adjusted frame length (%d)  is less than lengthFieldEndOffset: %d",
                frameLength,
                lengthFieldEndOffset));
    }

    if (frameLength > maxFrameLength) {
        // Enter the discard mode and discard everything received so far.
        discardingTooLongFrame = true;
        tooLongFrameLength = frameLength;
        bytesToDiscard = frameLength - in->readableBytes();
        in->skipBytes(in->readableBytes());
        return ChannelBufferPtr();
    }

    // never overflows because it's less than maxFrameLength
    int frameLengthInt = (int) frameLength;

    if (in->readableBytes() < frameLengthInt) {
        return ChannelBufferPtr();
    }

    if (initialBytesToStrip > frameLengthInt) {
        in->skipBytes(frameLengthInt);
        std::string msg;
        StringUtil::strprintf(&msg,
                              "Adjusted frame length (%d) is less than initialBytesToStrip: %d",
                              frameLength, initialBytesToStrip);
        throw CorruptedFrameException(msg);
    }

    in->skipBytes(initialBytesToStrip);

    // extract frame
    int readerIndex = in->readerIndex();
    int actualFrameLength = frameLengthInt - initialBytesToStrip;
    ChannelBufferPtr frame = extractFrame(in, readerIndex, actualFrameLength);
    in->readerIndex(readerIndex + actualFrameLength);
    return frame;
}

ChannelBufferPtr
LengthFieldBasedFrameDecoder::extractFrame(const ChannelBufferPtr& buffer, int index, int length) {
    ChannelBufferPtr frame = Unpooled::buffer(length);
    frame->writeBytes(buffer, index, length);
    return frame;
}

void LengthFieldBasedFrameDecoder::fail(ChannelHandlerContext& ctx, int frameLength) {
    std::string msg;

    if (frameLength > 0) {
        StringUtil::strprintf(&msg,
                              "Adjusted frame length exceeds %d: % - discarded.",
                              maxFrameLength, frameLength);
        ctx.fireExceptionCaught(TooLongFrameException(msg));
    }
    else {
        StringUtil::strprintf("Adjusted frame length exceeds %d - discarded.", maxFrameLength);
        ctx.fireExceptionCaught(TooLongFrameException(msg));
    }
}

void LengthFieldBasedFrameDecoder::validateParameters() {
    if (maxFrameLength <= 0) {
        throw InvalidArgumentException(
            std::string("maxFrameLength must be a positive integer: ") +
            Integer::toString(maxFrameLength));
    }

    if (lengthFieldOffset < 0) {
        throw InvalidArgumentException(
            std::string("lengthFieldOffset must be a non-negative integer: ") +
            Integer::toString(lengthFieldOffset));
    }

    if (initialBytesToStrip < 0) {
        throw InvalidArgumentException(
            std::string("initialBytesToStrip must be a non-negative integer: ") +
            Integer::toString(initialBytesToStrip));
    }

    if (checksumFieldLength < 0) {
        throw InvalidArgumentException(
            std::string("checksumFieldLength must be a non-negative integer: ") +
            Integer::toString(checksumFieldLength));
    }

    if (checksumCalcOffset < 0) {
        throw InvalidArgumentException(
            std::string("checksumFieldOffset must be a non-negative integer: ") +
            Integer::toString(checksumCalcOffset));
    }

    if (checksumFieldLength > 0 && !checksumFunction) {
        throw InvalidArgumentException(
            std::string("you set the checksumField but no checksum function set.\n"));
    }

    if (lengthFieldLength != 1 && lengthFieldLength != 2 &&
            lengthFieldLength != 4 && lengthFieldLength != 8) {
        throw InvalidArgumentException(
            std::string("lengthFieldLength must be either 1, 2, 4, or 8: ") +
            Integer::toString(lengthFieldLength));
    }

    if (lengthFieldOffset > maxFrameLength - lengthFieldLength) {
        std::string msg;
        StringUtil::strprintf(&msg,
                              "maxFrameLength (%d) must be equal to or greater \
            than lengthFieldOffset (%d) + lengthFieldLength (%d).",
                              maxFrameLength, lengthFieldOffset, lengthFieldLength);

        throw InvalidArgumentException(msg);
    }
}

}
}
}
