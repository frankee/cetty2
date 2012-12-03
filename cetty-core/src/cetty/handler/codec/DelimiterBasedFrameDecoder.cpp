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

#include <cetty/handler/codec/DelimiterBasedFrameDecoder.h>
#include <cetty/handler/codec/TooLongFrameException.h>
#include <cetty/buffer/ChannelBuffer.h>

#include <cetty/channel/ChannelHandlerContext.h>

#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::buffer;

DelimiterBasedFrameDecoder::DelimiterBasedFrameDecoder(
    int maxFrameLength,
    const ChannelBufferPtr& delimiter)
    : stripDelimiter_(true),
      discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0) {
    init();
    validateDelimiter(delimiter);
    delimiters_.push_back(delimiter->slice(delimiter->readerIndex(),
                                           delimiter->readableBytes()));
}

DelimiterBasedFrameDecoder::DelimiterBasedFrameDecoder(
    int maxFrameLength,
    bool stripDelimiter,
    const ChannelBufferPtr& delimiter)
    : stripDelimiter_(stripDelimiter),
      discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0) {
    init();
    validateDelimiter(delimiter);
    delimiters_.push_back(delimiter->slice(delimiter->readerIndex(),
                                           delimiter->readableBytes()));
}

DelimiterBasedFrameDecoder::DelimiterBasedFrameDecoder(
    int maxFrameLength,
    const std::vector<ChannelBufferPtr>& delimiters)
    : stripDelimiter_(true),
      discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0) {
    init();
    initDelimiters(delimiters);
}

DelimiterBasedFrameDecoder::DelimiterBasedFrameDecoder(
    int maxFrameLength,
    bool stripDelimiter,
    const std::vector<ChannelBufferPtr>& delimiters)
    : stripDelimiter_(stripDelimiter),
      discardingTooLongFrame_(false),
      maxFrameLength_(maxFrameLength),
      tooLongFrameLength_(0) {
    init();
    initDelimiters(delimiters);
}

ChannelBufferPtr DelimiterBasedFrameDecoder::decode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& in) {
    // Try all delimiters and choose the delimiter which yields the shortest frame.
    int minFrameLength = MAX_INT32;
    ChannelBufferPtr minDelim;

    for (size_t i = 0; i < delimiters_.size(); ++i) {
        int frameLength = indexOf(in, delimiters_[i]);

        if (frameLength >= 0 && frameLength < minFrameLength) {
            minFrameLength = frameLength;
            minDelim = delimiters_[i];
        }
    }

    if (minDelim) {
        int minDelimLength = minDelim->capacity();
        ChannelBufferPtr frame;

        if (discardingTooLongFrame_) {
            // We've just finished discarding a very large frame.
            // Go back to the initial state.
            discardingTooLongFrame_ = false;
            in->skipBytes(minFrameLength + minDelimLength);

            // TODO Let user choose when the exception should be raised - early or late?
            //      If early, fail() should be called when discardingTooLongFrame is set to true.
            int tooLongFrameLength = this->tooLongFrameLength_;
            this->tooLongFrameLength_ = 0;
            fail(ctx, tooLongFrameLength);
            return ChannelBufferPtr();
        }

        if (minFrameLength > maxFrameLength_) {
            // Discard read frame.
            in->skipBytes(minFrameLength + minDelimLength);
            fail(ctx, minFrameLength);
            return ChannelBufferPtr();
        }

        if (stripDelimiter_) {
            frame = in->readBytes(minFrameLength);
            in->skipBytes(minDelimLength);
        }
        else {
            frame = in->readBytes(minFrameLength + minDelimLength);
        }

        return frame;
    }
    else {
        if (!discardingTooLongFrame_) {
            if (in->readableBytes() > maxFrameLength_) {
                // Discard the content of the buffer until a delimiter is found.
                tooLongFrameLength_ = in->readableBytes();
                in->skipBytes(in->readableBytes());
                discardingTooLongFrame_ = true;
            }
        }
        else {
            // Still discarding the buffer since a delimiter is not found.
            tooLongFrameLength_ += in->readableBytes();
            in->skipBytes(in->readableBytes());
        }

        return ChannelBufferPtr();
    }
}

void DelimiterBasedFrameDecoder::fail(ChannelHandlerContext& ctx, long frameLength) {
    std::string msg;
    msg.reserve(64);

    if (frameLength > 0) {
        StringUtil::printf(&msg,
                           "frame length exceeds %d: %d - discarded.",
                           maxFrameLength_,
                           frameLength);

        ctx.fireExceptionCaught(TooLongFrameException(msg));
    }
    else {
        StringUtil::printf(&msg,
                           "frame length exceeds %d - discarded",
                           maxFrameLength_);

        ctx.fireExceptionCaught(TooLongFrameException(msg));
    }
}

int DelimiterBasedFrameDecoder::indexOf(const ChannelBufferPtr& haystack,
                                        const ChannelBufferPtr& needle) {
    for (int i = haystack->readerIndex(); i < haystack->writerIndex(); ++i) {
        int haystackIndex = i;
        int needleIndex;

        for (needleIndex = 0; needleIndex < needle->capacity(); ++needleIndex) {
            if (haystack->getByte(haystackIndex) != needle->getByte(needleIndex)) {
                break;
            }
            else {
                haystackIndex ++;

                if (haystackIndex == haystack->writerIndex() &&
                        needleIndex != needle->capacity() - 1) {
                    return -1;
                }
            }
        }

        if (needleIndex == needle->capacity()) {
            // Found the needle from the haystack!
            return i - haystack->readerIndex();
        }
    }

    return -1;
}

void DelimiterBasedFrameDecoder::validateDelimiter(const ChannelBufferPtr& delimiter) {
    if (!delimiter) {
        throw NullPointerException("delimiter");
    }

    if (!delimiter->readable()) {
        throw InvalidArgumentException("empty delimiter");
    }
}

void DelimiterBasedFrameDecoder::validateMaxFrameLength() {
    if (maxFrameLength_ <= 0) {
        std::string msg;
        StringUtil::printf(&msg,
                           "maxFrameLength must be a positive integer: %d",
                           maxFrameLength_);

        LOG_ERROR << msg;
        throw InvalidArgumentException(msg);
    }
}

void DelimiterBasedFrameDecoder::initDelimiters(
    const std::vector<ChannelBufferPtr>& delimiters) {
    if (delimiters.size() == 0) {
        throw InvalidArgumentException("empty delimiters");
    }

    for (size_t i = 0; i < delimiters.size(); ++i) {
        const ChannelBufferPtr& d = delimiters[i];
        validateDelimiter(d);
        this->delimiters_.push_back(d->slice(d->readerIndex(), d->readableBytes()));
    }
}

void DelimiterBasedFrameDecoder::init() {
    validateMaxFrameLength();

    decoder_.setDecoder(boost::bind(&DelimiterBasedFrameDecoder::decode,
                                    this,
                                    _1,
                                    _2));
}

}
}
}
