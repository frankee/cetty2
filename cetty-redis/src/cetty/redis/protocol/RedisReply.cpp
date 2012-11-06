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

#include <cetty/redis/protocol/RedisReply.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace redis {
namespace protocol {

const RedisReplyType RedisReplyType::STRING  = 0;
const RedisReplyType RedisReplyType::ARRAY   = 1;
const RedisReplyType RedisReplyType::INTEGER = 2;
const RedisReplyType RedisReplyType::NIL     = 3;
const RedisReplyType RedisReplyType::STATUS  = 4;
const RedisReplyType RedisReplyType::ERROR   = 5;

const std::vector<StringPiece> RedisReply::EMPTY_STRING_PIECES;

int64_t RedisReply::getInteger() const {
    if (type == RedisReplyType::INTEGER) {
        return boost::get<int64_t>(value);
    }
    else {
        LOG_WARN << "the RedisReplyMessageType is not integer when getInteger";
        return 0;
    }
}

std::vector<StringPiece>* RedisReply::getMutableArray() {
    if (type == RedisReplyType::ARRAY) {
        if (value.which() == 0) {
            value = std::vector<StringPiece>();
        }

        return boost::get<std::vector<StringPiece> >(&value);
    }

    return NULL;
}

}
}
}
