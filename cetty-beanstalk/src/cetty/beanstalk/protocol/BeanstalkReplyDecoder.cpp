
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
/*
 * BeanstalkReplyDecoder.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: chenhl
 */

#include <cetty/beanstalk/protocol/BeanstalkReplyDecoder.h>
#include <cetty/util/StringUtil.h>
#include <cetty/beanstalk/protocol/BeanstalkReply.h>

namespace cetty {
namespace beanstalk {
namespace protocol {

BeanstalkReplyDecoder::BeanstalkReplyDecoder()
    : dataSize_(0) {
    decoder_.setDecoder(boost::bind(&BeanstalkReplyDecoder::decode,
                                    this,
                                    _1,
                                    _2));
}

BeanstalkReplyPtr BeanstalkReplyDecoder::decode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& buffer) {
    if (!reply_) {
        reply_ = new BeanstalkReply();
    }

    StringPiece bytes;
    buffer->readableBytes(&bytes);

    int lineLength = findLineEnd(bytes, 0);

    if (lineLength < 0) {
        LOG_DEBUG << "has not find the \"\\r\\n\", continue to reading";
        return BeanstalkReplyPtr();
    }
    else if (lineLength == 0) {
        LOG_ERROR << "parsing reply from server, but is an empty line";
        return reset();
    }

    if (dataSize_ == 0) { // parse first line
        buffer->skipBytes(lineLength + 2);

        StringPiece typeStr;
        int offset = getStrValue(bytes, 0, &typeStr);
        BeanstalkReplyType type = BeanstalkReplyType::parseFrom(typeStr.c_str());
        reply_->setType(type);

        int intValue;

        if (type == BeanstalkReplyType::INSERTED ||
                type == BeanstalkReplyType::BURIED) {
            getIntValue(bytes, offset, &intValue);
            reply_->setId(intValue);
        }
        else if (type == BeanstalkReplyType::USING) {
            StringPiece tube;
            getStrValue(bytes, offset, &tube);
            reply_->setTube(tube);
        }
        else if (type == BeanstalkReplyType::RESERVED ||
                 type == BeanstalkReplyType::FOUND) {
            int newOffset = getIntValue(bytes, offset, &intValue);
            reply_->setId(intValue);

            getIntValue(bytes, newOffset, &dataSize_);

            if (bytes.length() >= lineLength + dataSize_ + 4) {
                reply_->setData(bytes.substr(lineLength + 2, dataSize_));
                buffer->skipBytes(dataSize_ + 2);
            }
            else {
                LOG_DEBUG << "need read more for data part when parse " << type.name();
                return BeanstalkReplyPtr();
            }
        }
        else if (type == BeanstalkReplyType::OK) {
            getIntValue(bytes, offset, &dataSize_);

            if (bytes.length() >= lineLength + dataSize_ + 4) {
                reply_->setData(bytes.substr(lineLength + 2, dataSize_));
                buffer->skipBytes(dataSize_ + 2);
            }
            else {
                LOG_DEBUG << "need read more for data part when parse " << type.name();
                return BeanstalkReplyPtr();
            }
        }
        else if (type == BeanstalkReplyType::WATCHING ||
                 type == BeanstalkReplyType::KICKED) {
            getIntValue(bytes, offset, &intValue);
            reply_->setCount(intValue);
        }
        else {

        }
    }
    else { // parse the data part
        if (bytes.length() >= dataSize_ + 2) {
            buffer->skipBytes(dataSize_ + 2);
            reply_->setData(bytes.substr(0, dataSize_));
        }
        else {
            LOG_DEBUG << "need read more for data part";
            return BeanstalkReplyPtr();
        }
    }

    return reset();
}

int BeanstalkReplyDecoder::findLineEnd(const StringPiece& bytes, int offset) {
    for (int i = offset; i < bytes.length(); ++i) {
        if (bytes[i] == '\r' || bytes[i] == '\0') {
            if (i + 1 == bytes.length()) {
                return -1;
            }
            else if (bytes[i+1] == '\n') {
                return i;
            }
        }
    }

    return -1;
}

int BeanstalkReplyDecoder::getIntValue(const StringPiece& bytes, int offset, int* value) {
    BOOST_ASSERT(value);

    if (offset < 0) {
        return 0;
    }

    int i = offset;
    int j = 0;

    // trim first
    while (bytes[i] == ' ' && i < bytes.length()) {
        i++;
    }

    if (i >= bytes.length()) {
        return offset;
    }

    j = i;

    while (bytes[j] >= '0' && bytes[j] <= '9') {
        ++j;
    }

    if (i ==j) {
        return offset;
    }

    *value = StringUtil::strto32(bytes.substr(i, j - i));
    return j;
}

int BeanstalkReplyDecoder::getStrValue(const StringPiece& bytes, int offset, StringPiece* str) {
    BOOST_ASSERT(str);

    if (offset < 0) {
        return 0;
    }

    int i = offset;
    int length = bytes.length();

    while (bytes[i] == ' ' && i < length) {
        ++ i;
    }

    if (i >= length) {
        return offset;
    }

    int endIndex = i;

    while (bytes[endIndex] != ' ' && bytes[endIndex] != '\r') {
        ++endIndex;
    }

    if (endIndex >= length) {
        return offset;
    }

    *str = bytes.substr(i, endIndex - i);
    return endIndex;
}

BeanstalkReplyPtr BeanstalkReplyDecoder::reset() {
    BeanstalkReplyPtr message = reply_;
    reply_.reset();
    dataSize_ = 0;
    return message;
}

}
}
}
