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

#include <cetty/redis/protocol/RedisReplyDecoder.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>
#include <cetty/logging/LoggerHelper.h>

#include <cetty/handler/codec/TooLongFrameException.h>

#include <cetty/redis/protocol/RedisReply.h>

namespace cetty {
namespace redis {
namespace protocol {

using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::handler::codec;

//static const char* REDIS_LBR                       = "\r\n";
//static const char* REDIS_STATUS_REPLY_OK           = "OK";
//static const char* REDIS_PREFIX_STATUS_REPLY_ERROR = "-ERR ";

static const char REDIS_PREFIX_ERROR_REPLY       = '-';
static const char REDIS_PREFIX_STATUS_REPLY      = '+';
static const char REDIS_PREFIX_INTEGER_REPLY     = ':';
static const char REDIS_PREFIX_SINGLE_BULK_REPLY = '$';
static const char REDIS_PREFIX_MULTI_BULK_REPLY  = '*';

//static const char REDIS_WHITESPACE                " \f\n\r\t\v"

static inline
int getFrameInt(const StringPiece& frame, int frameLength) {
    StringPiece value(frame.data() + 1, frameLength - 1);
    return StringUtil::strto32(value);
}

static inline
StringPiece getFrameBytes(const StringPiece& frame, int frameLength) {
    return StringPiece(frame.data() + 1, frameLength - 1);
}

RedisReplyPtr RedisReplyMessageDecoder::decode(ChannelHandlerContext& ctx,
        const ReplayingDecoderBufferPtr& buffer,
        int state) {
    // Try all delimiters and choose the delimiter which yields the shortest frame.
    //int minFrameLength = Integer::MAX_VALUE;

    if (!reply) {
        reply = RedisReplyPtr(new RedisReply);
    }

    StringPiece bytes;
    buffer->readableBytes(&bytes);

    int frameLength = indexOf(bytes, 0);

    if (frameLength < 0) {
        LOG_DEBUG << "has not find the \"\\r\\n\", continue to reading";

        if (bytes.length() > maxBulkSize) {
            LOG_ERROR << "reading the data has bigger then the maxBulkSize";
            //buffer->skipBytes(buffer->readableBytes());
        }

        return RedisReplyPtr();
    }
    else if (frameLength == 0) {
        LOG_ERROR << "parsing reply from server, but is an empty line";
        //TODO fire an error?
        buffer->skipBytes(frameLength + 2);
        reply->setType(RedisReplyType::NIL);
        return reset();
    }

    if (READ_INITIAL == state) {
        if (bytes[0] == REDIS_PREFIX_STATUS_REPLY) {
            reply->setType(RedisReplyType::STATUS);
            buffer->skipBytes(frameLength + 2);
            reply->setValue(getFrameBytes(bytes, frameLength));
        }
        else if (bytes[0] == REDIS_PREFIX_INTEGER_REPLY) {
            reply->setType(RedisReplyType::INTEGER);
            buffer->skipBytes(frameLength + 2);
            reply->setValue(getFrameInt(bytes, frameLength));
        }
        else if (bytes[0] == REDIS_PREFIX_ERROR_REPLY) {
            reply->setType(RedisReplyType::ERROR);
            buffer->skipBytes(frameLength + 2);
            reply->setValue(getFrameBytes(bytes, frameLength));
        }
        else if (bytes[0] == REDIS_PREFIX_SINGLE_BULK_REPLY) {
            reply->setType(RedisReplyType::STRING);
            bulkSize = getFrameInt(bytes, frameLength);
            buffer->skipBytes(frameLength + 2);

            if (bulkSize < 0) {
                reply->setType(RedisReplyType::NIL);
                return reset();
            }

            if (bulkSize == 0) {
                reply->setValue(std::string());
                return reset();
            }

            if (bytes.length() >= frameLength + 4 + bulkSize) {
                StringPiece data;
                buffer->readableBytes(&data);
                buffer->skipBytes(bulkSize + 2);
                reply->setValue(StringPiece(data.data(), bulkSize));
            }
            else {
                checkpoint(READ_BULK);
                return RedisReplyPtr();
            }
        }
        else if (bytes[0] == REDIS_PREFIX_MULTI_BULK_REPLY) {
            reply->setType(RedisReplyType::ARRAY);
            bulkSize = 0;
            multiBulkSize = getFrameInt(bytes, frameLength);
            buffer->skipBytes(frameLength + 2);

            if (multiBulkSize < 0) {
                reply->setType(RedisReplyType::NIL);
                return reset();
            }

            if (multiBulkSize == 0) {
                reply->setValue(std::vector<StringPiece>());
                return reset();
            }

            std::vector<StringPiece>* bulks = reply->getMutableArray();
            BOOST_ASSERT(bulks && "RedisReplyMessage getMutableArray exception");
            return readMultiBukls(buffer, bytes, bulks);
        }
    }
    else if (READ_BULK == state) {
        if (bytes.length() >= bulkSize + 2) {
            StringPiece data;
            buffer->readableBytes(&data);
            buffer->skipBytes(bulkSize + 2);
            reply->setValue(StringPiece(data.data(), bulkSize));
        }
        else {
            checkpoint(READ_BULK);
            return RedisReplyPtr();
        }
    }
    else if (READ_MULTI_BULK == state) {
        std::vector<StringPiece>* bulks = reply->getMutableArray();
        BOOST_ASSERT(bulks && "RedisReplyMessage getMutableArray exception");

        if (bulkSize > 0) {
            DLOG_DEBUG << "bulkSize is not empty, directly to read the bulk data";

            if (!readMultiBulkElement(buffer, bytes, bulks)) {
                DLOG_TRACE << "data has not read completely - bulk size, continue to reading";
                checkpoint(READ_MULTI_BULK);
                return RedisReplyPtr();
            }
        }

        return readMultiBukls(buffer, bytes, bulks);
    }

    return reset();
}

void RedisReplyMessageDecoder::fail(ChannelHandlerContext& ctx, long frameLength) {
    if (frameLength > 0) {
        ctx.fireExceptionCaught(ctx,
                                TooLongFrameException(StringUtil::printf(
                                            "frame length exceeds %d: %d - discarded",
                                            maxBulkSize,
                                            frameLength)));
    }
    else {
        ctx.fireExceptionCaught(ctx,
                                TooLongFrameException(StringUtil::printf(
                                            "frame length exceeds %d - discarded",
                                            maxBulkSize)));
    }
}

int RedisReplyMessageDecoder::indexOf(const StringPiece& bytes, int offset) {
    for (int i = offset; i < bytes.length(); ++i) {
        if (bytes[i] == '\r' || bytes[i] == '\0') {
            if (i+1 == bytes.length()) {
                return -1;
            }
            else if (bytes[i+1] == '\n') {
                return i;
            }
        }
    }

    return -1;
}

RedisReplyPtr RedisReplyMessageDecoder::reset() {
    RedisReplyPtr message = this->reply;

    this->reply.reset();
    this->bulkSize = 0;
    this->multiBulkSize = 0;

    checkpoint(READ_INITIAL);
    return message;
}

ChannelHandlerPtr RedisReplyMessageDecoder::clone() {
    return ChannelHandlerPtr(new RedisReplyMessageDecoder());
}

bool RedisReplyMessageDecoder::readMultiBulkElement(
    const ReplayingDecoderBufferPtr& buffer,
    const StringPiece& bytes,
    std::vector<StringPiece>* bulks) {

    if (bytes.length() >= bulkSize + 2) {
        StringPiece data;
        buffer->readableBytes(&data);
        buffer->skipBytes(bulkSize + 2);
        bulks->push_back(StringPiece(data.data(), bulkSize));
        bulkSize = 0;
        --multiBulkSize;
        return true;
    }

    return false;
}

RedisReplyPtr RedisReplyMessageDecoder::readMultiBukls(
    const ReplayingDecoderBufferPtr& buffer,
    const StringPiece& bytes,
    std::vector<StringPiece>* bulks) {
    int j = multiBulkSize;

    for (int i = 0; i < j; ++i) {
        StringPiece subBytes;
        buffer->readableBytes(&subBytes);
        int frameLength = indexOf(subBytes, 0);

        if (frameLength < 0) {
            DLOG_TRACE << "data has not read completely - multi-bulk size, continue to reading";
            checkpoint(READ_MULTI_BULK);
            return RedisReplyPtr();
        }

        if (frameLength == 0 || subBytes[0] != REDIS_PREFIX_SINGLE_BULK_REPLY) {
            if (frameLength) {
                LOG_ERROR << "parsing multi-bulk, but the first line not begin with the '$'";
            }
            else {
                LOG_ERROR << "parsing multi-bulk, but is empty line";
            }

            //TODO fire an error?
            buffer->skipBytes(frameLength + 2);
            reply->setType(RedisReplyType::NIL);
            return reset();
        }

        if (subBytes[1] != '-') {
            bulkSize = getFrameInt(subBytes, frameLength);
            buffer->skipBytes(frameLength + 2);

            if (bytes.length() >= frameLength + 4 + bulkSize) {
                readMultiBulkElement(buffer, bytes, bulks);
            }
            else {
                DLOG_TRACE << "data has not read completely - bulk size, continue to reading";
                checkpoint(READ_MULTI_BULK);
                return RedisReplyPtr();
            }
        }
        else {
            bulks->push_back(StringPiece());
            buffer->skipBytes(frameLength + 2);
            --multiBulkSize;
        }
    }

    return reset();
}

}
}
}
