/**
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

#include "RedisResponseDecoder.h"

#include <cetty/channel/Channel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/util/Integer.h>
#include <cetty/util/Exception.h>

#include <cetty/handler/codec/frame/TooLongFrameException.h>

#include "RedisReplyMessage.h"

namespace cetty { namespace redis {

using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::handler::codec::frame;

#define REDIS_LBR                       "\r\n"
static const char* REDIS_STATUS_REPLY_OK           = "OK";
static const char* REDIS_PREFIX_STATUS_REPLY_ERROR = "-ERR ";

static const char REDIS_PREFIX_ERROR_REPLY       = '-';
static const char REDIS_PREFIX_STATUS_REPLY      = '+';
static const char REDIS_PREFIX_INTEGER_REPLY     = ':';
static const char REDIS_PREFIX_SINGLE_BULK_REPLY = '$';
static const char REDIS_PREFIX_MULTI_BULK_REPLY  = '*';

//static const char REDIS_WHITESPACE                " \f\n\r\t\v"

cetty::channel::ChannelMessage
    RedisResponseDecoder::decode(ChannelHandlerContext& ctx,
    Channel& channel,
    const ChannelBufferPtr& buffer) {
        // Try all delimiters and choose the delimiter which yields the shortest frame.
        int minFrameLength = Integer::MAX_VALUE;
        
        Array arry;
        buffer->readableBytes(arry);

        RedisReplyMessagePtr reply = RedisReplyMessagePtr(new RedisReplyMessage);

        int frameLength = indexOf(arry, 0);
        if (frameLength > 0) {
            if (arry[0] == REDIS_PREFIX_STATUS_REPLY) {
                reply->setType(ReplyType::REPLY_STATUS);
                buffer->skipBytes(frameLength + 2);
                reply->setValue(SimpleString(arry.data(1), frameLength-1));
            }
            else if (arry[0] == REDIS_PREFIX_INTEGER_REPLY) {
                reply->setType(ReplyType::REPLY_INTEGER);
                buffer->skipBytes(frameLength + 2);
                reply->setValue(atoi(arry.data(1)));
            }
            else if (arry[0] == REDIS_PREFIX_ERROR_REPLY) {
                reply->setType(ReplyType::REPLY_ERROR);
                buffer->skipBytes(frameLength + 2);
                reply->setValue(SimpleString(arry.data(1), frameLength-1));
            }
            else if (arry[0] == REDIS_PREFIX_SINGLE_BULK_REPLY) {
                if (arry[1] != '-') {
                    reply->setType(ReplyType::REPLY_STRING);
                    arry[frameLength] = '\0';
                    int strSize = atoi(arry.data(1));

                    if (arry.length() >= frameLength + 4 + strSize) {
                        buffer->skipBytes(frameLength + 2);
                        
                        Array data;
                        buffer->readableBytes(data);
                        buffer->skipBytes(strSize + 2);
                        reply->setValue(SimpleString(data.data(), strSize));
                    }
                    else {
                        return ChannelMessage::EMPTY_MESSAGE;
                    }
                }
                else {
                    buffer->skipBytes(frameLength + 2);
                    reply->setType(ReplyType::REPLY_NIL);
                }
            }
            else if (arry[0] == REDIS_PREFIX_MULTI_BULK_REPLY) {
                if (arry[1] != '-' && arry[1] != '0') {
                    reply->setType(ReplyType::REPLY_ARRAY);
                    arry[frameLength] = '\0';
                    int arrayCount = atoi(arry.data(1));
                    std::vector<SimpleString> stringArray;
                    stringArray.reserve(arrayCount);

                    buffer->skipBytes(frameLength + 2);
                    for (int i = 0; i < arrayCount; ++i) {
                        Array subArry;
                        buffer->readableBytes(subArry);
                        int frameLength = indexOf(subArry, 0);

                        if (subArry[0] != REDIS_PREFIX_SINGLE_BULK_REPLY) {
                            continue;
                        }

                        if (subArry[1] != '-') {
                            subArry[frameLength] = '\0';
                            int strSize = atoi(subArry.data(1));

                            if (arry.length() >= frameLength + 4 + strSize) {
                                buffer->skipBytes(frameLength + 2);

                                Array data;
                                buffer->readableBytes(data);
                                data[strSize] = '\0';
                                buffer->skipBytes(strSize + 2);

                                stringArray.push_back(SimpleString(data.data(), strSize));
                            }
                            else {
                                break;
                            }
                        }
                        else {
                            stringArray.push_back(SimpleString());
                            buffer->skipBytes(frameLength + 2);
                        }
                    }

                    if (stringArray.size() < arrayCount) {
                        return ChannelMessage::EMPTY_MESSAGE;
                    }
                    else {
                        reply->setValue(stringArray);
                    }
                }
                else {
                    buffer->skipBytes(frameLength + 2);
                    reply->setType(ReplyType::REPLY_NIL);
                }
            }
            return ChannelMessage(reply);
        }
        else {
            if (!discardingTooLongFrame) {
                if (buffer->readableBytes() > maxFrameLength) {
                    // Discard the content of the buffer until a delimiter is found.
                    tooLongFrameLength = buffer->readableBytes();
                    buffer->skipBytes(buffer->readableBytes());
                    discardingTooLongFrame = true;
                }
            }
            else {
                // Still discarding the buffer since a delimiter is not found.
                tooLongFrameLength += buffer->readableBytes();
                buffer->skipBytes(buffer->readableBytes());
            }
            return ChannelMessage::EMPTY_MESSAGE;
        }
}

void RedisResponseDecoder::fail(ChannelHandlerContext& ctx, long frameLength) {
    std::string msg;
    msg.reserve(64);

    if (frameLength > 0) {
        msg.append("frame length exceeds ");
        Integer::appendString(maxFrameLength, &msg);
        msg.append(": ");
        Integer::appendString(frameLength, &msg);
        msg.append(" - discarded");

        Channels::fireExceptionCaught(ctx.getChannel(),
            TooLongFrameException(msg));
    }
    else {
        msg.append("frame length exceeds ");
        Integer::appendString(maxFrameLength, &msg);
        msg.append(" - discarded");

        Channels::fireExceptionCaught(ctx.getChannel(),
            TooLongFrameException(msg));
    }
}

int RedisResponseDecoder::indexOf(const Array& arry, int offset) {
    for (int i = offset; i < arry.length(); ++i) {
        if (arry[i] == '\r' || arry[i] == '\0') {
            if (i+1 == arry.length()) {
                return -1;
            }
            else if (arry[i+1] == '\n') {
                return i;
            }
        }
    }

    return -1;
}

cetty::channel::ChannelHandlerPtr RedisResponseDecoder::clone() {
    return ChannelHandlerPtr(new RedisResponseDecoder());
}

}}
