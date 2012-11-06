#if !defined(CETTY_REDIS_PROTOCOL_REDISREPLYMESSAGE_H)
#define CETTY_REDIS_PROTOCOL_REDISREPLYMESSAGE_H

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
#include <cetty/Types.h>
#include <boost/variant.hpp>
#include <cetty/util/Enum.h>
#include <cetty/util/StringPiece.h>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/redis/protocol/RedisReplyPtr.h>

#if defined(WIN32)
#if defined(ERROR)
#undef ERROR
#endif
#endif

namespace cetty {
namespace redis {
namespace protocol {

using namespace cetty::util;

class RedisReplyType : public cetty::util::Enum<RedisReplyType> {
public:
    static const RedisReplyType STRING;
    static const RedisReplyType ARRAY;
    static const RedisReplyType INTEGER;
    static const RedisReplyType NIL;
    static const RedisReplyType STATUS;
    static const RedisReplyType ERROR;

private:
    RedisReplyType(int value) : cetty::util::Enum<RedisReplyType>(value) {}
};

class RedisReply : public ReferenceCounter<RedisReply> {
public:
    RedisReply() : type(RedisReplyType::NIL) {}

    void setType(const RedisReplyType& type) {
        this->type = type;
    }
    const RedisReplyType& getType() const {
        return this->type;
    }

    void setValue(int64_t integer) {
        value = integer;
    }

    void setValue(const StringPiece& StringPiece) {
        value = StringPiece;
    }

    void setValue(const std::vector<StringPiece>& StringPieces) {
        value = StringPieces;
    }

    int64_t getInteger() const;

    StringPiece getString() const {
        if (type == RedisReplyType::STRING) {
            return boost::get<StringPiece>(value);
        }

        return StringPiece();
    }

    StringPiece getStatus() const {
        if (type == RedisReplyType::STATUS) {
            return boost::get<StringPiece>(value);
        }

        return StringPiece();
    }

    StringPiece getError() const {
        if (type == RedisReplyType::ERROR) {
            return boost::get<StringPiece>(value);
        }

        return StringPiece();
    }

    bool isNil() const {
        return type == RedisReplyType::NIL;
    }

    const std::vector<StringPiece>& getArray() const {
        if (type == RedisReplyType::ARRAY) {
            return boost::get<std::vector<StringPiece> >(value);
        }

        return EMPTY_STRING_PIECES;
    }

    std::vector<StringPiece>* getMutableArray();

private:
    typedef boost::variant<int64_t,
            StringPiece,
            std::vector<StringPiece> > Value;

private:
    RedisReplyType  type;
    Value value;

private:
    static const std::vector<StringPiece> EMPTY_STRING_PIECES;
};

}
}
}

#endif //#if !defined(CETTY_REDIS_PROTOCOL_REDISREPLYMESSAGE_H)

// Local Variables:
// mode: c++
// End:
