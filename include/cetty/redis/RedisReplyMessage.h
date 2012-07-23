#if !defined(CETTY_REDIS_REDISREPLYMESSAGE_H)
#define CETTY_REDIS_REDISREPLYMESSAGE_H

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

#include <vector>
#include <boost/variant.hpp>
#include <cetty/util/Enum.h>
#include <cetty/util/SimpleString.h>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace redis {

using namespace cetty::util;

class RedisReplyMessageType : public cetty::util::Enum<RedisReplyMessageType> {
public:
    static const RedisReplyMessageType STRING;
    static const RedisReplyMessageType ARRAY;
    static const RedisReplyMessageType INTEGER;
    static const RedisReplyMessageType NIL;
    static const RedisReplyMessageType STATUS;
    static const RedisReplyMessageType ERROR;

private:
    RedisReplyMessageType(int value) : cetty::util::Enum<RedisReplyMessageType>(value) {}
};

typedef boost::variant<boost::int64_t, SimpleString, std::vector<SimpleString> > RedisReplyMessageValue;

class RedisReplyMessage : public ReferenceCounter<RedisReplyMessage> {
public:
    RedisReplyMessage() : type(RedisReplyMessageType::NIL) {}

    void setType(const RedisReplyMessageType& type) {
        this->type = type;
    }
    const RedisReplyMessageType& getType() const {
        return this->type;
    }

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
        if (type == RedisReplyMessageType::INTEGER) {
            return boost::get<boost::int64_t>(value);
        }
    }

    const SimpleString& getString() const {
        if (type == RedisReplyMessageType::STRING) {
            return boost::get<SimpleString>(value);
        }
    }

    const SimpleString& getStatus() const {
        if (type == RedisReplyMessageType::STATUS) {
            return boost::get<SimpleString>(value);
        }
    }

    const SimpleString& getError() const {
        if (type == RedisReplyMessageType::ERROR) {
            return boost::get<SimpleString>(value);
        }
    }

    bool isNil() const {
        return type == RedisReplyMessageType::NIL;
    }

    const std::vector<SimpleString>& getArray() const {
        if (type == RedisReplyMessageType::ARRAY) {
            return boost::get<std::vector<SimpleString> >(value);
        }
    }

private:
    RedisReplyMessageType  type;
    RedisReplyMessageValue value;
};

}
}

#endif //#if !defined(CETTY_REDIS_REDISREPLYMESSAGE_H)

// Local Variables:
// mode: c++
// End:
