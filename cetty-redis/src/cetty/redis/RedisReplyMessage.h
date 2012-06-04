#if !defined(CETTY_REDIS_REDISREPLYMESSAGE_H)
#define CETTY_REDIS_REDISREPLYMESSAGE_H

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

#include <vector>
#include <boost/variant.hpp>
#include <cetty/util/Enum.h>
#include <cetty/util/SimpleString.h>
#include <cetty/util/ReferenceCounter.h>

namespace cetty { namespace redis {

using namespace cetty::util;

class ReplyType : public cetty::util::Enum<ReplyType> {
public:
    static const ReplyType REPLY_STRING;
    static const ReplyType REPLY_ARRAY;
    static const ReplyType REPLY_INTEGER;
    static const ReplyType REPLY_NIL;
    static const ReplyType REPLY_STATUS;
    static const ReplyType REPLY_ERROR;

private:
    ReplyType(int value) : cetty::util::Enum<ReplyType>(value) {}
};

typedef boost::variant<boost::int64_t, SimpleString, std::vector<SimpleString> > ReplyValue;

class RedisReplyMessage : public ReferenceCounter<RedisReplyMessage> {
public:
    RedisReplyMessage() : type(ReplyType::REPLY_NIL) {}

    void setType(const ReplyType& type) { this->type = type; }
    const ReplyType& getType() const { return this->type; }

    void setValue(boost::int64_t integer) {
        value = integer;
    }

    void setValue(const SimpleString& simpleString) {
        value = simpleString;
    }

    void setValue(const std::vector<SimpleString>& simpleStrings) {
        value = simpleStrings;
    }

    boost::int64_t getInteger() const {
        if (type == ReplyType::REPLY_INTEGER) {
            return boost::get<boost::int64_t>(value);
        }
    }

    const SimpleString& getString() const {
        if (type == ReplyType::REPLY_STRING) {
            return boost::get<SimpleString>(value);
        }
    }

    const SimpleString& getStatus() const {
        if (type == ReplyType::REPLY_STATUS) {
            return boost::get<SimpleString>(value);
        }
    }

    const SimpleString& getError() const {
        if (type == ReplyType::REPLY_ERROR) {
            return boost::get<SimpleString>(value);
        }
    }

    bool isNil() const {
        return type == ReplyType::REPLY_NIL;
    }

    const std::vector<SimpleString>& getArray() const {
        if (type == ReplyType::REPLY_ARRAY) {
            return boost::get<std::vector<SimpleString> >(value);
        }
    }

private:
    ReplyType  type;
    ReplyValue value;
};


typedef boost::intrusive_ptr<RedisReplyMessage> RedisReplyMessagePtr;

}}


#endif //#if !defined(CETTY_REDIS_REDISREPLYMESSAGE_H)