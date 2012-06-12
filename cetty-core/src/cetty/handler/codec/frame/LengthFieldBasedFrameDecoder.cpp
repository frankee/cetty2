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

#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/buffer/ChannelBufferFactory.h>
#include <cetty/util/Integer.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/Exception.h>
#include <cetty/logging/LoggerHelper.h>

#include <cetty/handler/codec/frame/CorruptedFrameException.h>
#include <cetty/handler/codec/frame/TooLongFrameException.h>
#include <cetty/handler/codec/frame/LengthFieldBasedFrameDecoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace frame {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;

ChannelHandlerPtr LengthFieldBasedFrameDecoder::clone() {
    return ChannelHandlerPtr(new LengthFieldBasedFrameDecoder(*this));
}

cetty::channel::ChannelMessage
LengthFieldBasedFrameDecoder::decode(ChannelHandlerContext& ctx,
                                     const ChannelPtr& channel,
                                     const ChannelBufferPtr& buffer) {
    if (discardingTooLongFrame) {
        int bytesToDiscard = this->bytesToDiscard;
        int localBytesToDiscard = std::min(bytesToDiscard, buffer->readableBytes());
        buffer->skipBytes(localBytesToDiscard);
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

        return ChannelMessage::EMPTY_MESSAGE;
    }

    if (buffer->readableBytes() < lengthFieldEndOffset) {
        return ChannelMessage::EMPTY_MESSAGE;
    }

    int actualLengthFieldOffset = buffer->readerIndex() + lengthFieldOffset;

    int frameLength;

    switch (lengthFieldLength) {
    case 1:
        frameLength = buffer->getUnsignedByte(actualLengthFieldOffset);
        break;

    case 2:
        frameLength = buffer->getUnsignedShort(actualLengthFieldOffset);
        break;

    case 4:
        frameLength = buffer->getUnsignedInt(actualLengthFieldOffset);
        break;

    case 8:
        frameLength = (int)buffer->getLong(actualLengthFieldOffset);
        break;

    default:
        throw SystemException("should not reach here");
    }

    if (frameLength < 0) {
        buffer->skipBytes(lengthFieldEndOffset);
        throw CorruptedFrameException(
            std::string("negative pre-adjustment length field: ") +
            Integer::toString(frameLength));
    }

    frameLength += lengthAdjustment + lengthFieldEndOffset;

    if (frameLength < lengthFieldEndOffset) {
        buffer->skipBytes(lengthFieldEndOffset);
        throw CorruptedFrameException(
            std::string("Adjusted frame length (") +
            Integer::toString(frameLength) +
            std::string(") is less than lengthFieldEndOffset: ") +
            Integer::toString(lengthFieldEndOffset));
    }

    if (frameLength > maxFrameLength) {
        // Enter the discard mode and discard everything received so far.
        discardingTooLongFrame = true;
        tooLongFrameLength = frameLength;
        bytesToDiscard = frameLength - buffer->readableBytes();
        buffer->skipBytes(buffer->readableBytes());
        return ChannelMessage::EMPTY_MESSAGE;
    }

    // never overflows because it's less than maxFrameLength
    int frameLengthInt = (int) frameLength;

    if (buffer->readableBytes() < frameLengthInt) {
        return ChannelMessage::EMPTY_MESSAGE;
    }

    if (initialBytesToStrip > frameLengthInt) {
        buffer->skipBytes(frameLengthInt);
        std::string msg;
        StringUtil::strprintf(&msg,
                              "Adjusted frame length (%d) is less than initialBytesToStrip: %d",
                              frameLength, initialBytesToStrip);
        throw CorruptedFrameException(msg);
    }

    buffer->skipBytes(initialBytesToStrip);

    // extract frame
    int readerIndex = buffer->readerIndex();
    int actualFrameLength = frameLengthInt - initialBytesToStrip;
    ChannelBufferPtr frame = extractFrame(buffer, readerIndex, actualFrameLength);
    buffer->readerIndex(readerIndex + actualFrameLength);
    return ChannelMessage(frame);
}

ChannelBufferPtr
LengthFieldBasedFrameDecoder::extractFrame(const ChannelBufferPtr& buffer, int index, int length) {
    ChannelBufferPtr frame = buffer->factory().getBuffer(length);
    frame->writeBytes(buffer, index, length);
    return frame;
}

void LengthFieldBasedFrameDecoder::fail(ChannelHandlerContext& ctx, int frameLength) {
    std::string msg;

    if (frameLength > 0) {
        StringUtil::strprintf(&msg,
                              "Adjusted frame length exceeds %d: % - discarded.",
                              maxFrameLength, frameLength);
        Channels::fireExceptionCaught(ctx.getChannel(), TooLongFrameException(msg));
    }
    else {
        StringUtil::strprintf("Adjusted frame length exceeds %d - discarded.", maxFrameLength);
        Channels::fireExceptionCaught(ctx.getChannel(), TooLongFrameException(msg));
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
}
