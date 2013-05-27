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
    RedisReplyType(int value, const char* name)
        : cetty::util::Enum<RedisReplyType>(value, name) {
    }
};

class RedisReply : public ReferenceCounter<RedisReply, int> {
public:
    RedisReply();

    const RedisReplyType& type() const;

    void setType(const RedisReplyType& type);

    void setValue(int64_t integer);

    void setValue(const StringPiece& StringPiece);

    void setValue(const std::vector<StringPiece>& StringPieces);

    int64_t integerValue() const;

    StringPiece stringValue() const;

    StringPiece status() const;

    StringPiece error() const;

    bool isNil() const;

    const std::vector<StringPiece>& array() const;

    std::vector<StringPiece>* mutableArray();

    std::string toString() const;

private:
    typedef boost::variant<int64_t,
            StringPiece,
            std::vector<StringPiece> > Value;

private:
    RedisReplyType  type_;
    Value value_;

private:
    static const std::vector<StringPiece> EMPTY_STRING_PIECES;
};

inline
RedisReply::RedisReply()
    : type_(RedisReplyType::NIL) {
}

inline
const RedisReplyType& RedisReply::type() const {
    return type_;
}

inline
void RedisReply::setType(const RedisReplyType& type) {
    type_ = type;
}

inline
void RedisReply::setValue(int64_t integer) {
    value_ = integer;
}

inline
void RedisReply::setValue(const StringPiece& StringPiece) {
    value_ = StringPiece;
}

inline
void RedisReply::setValue(const std::vector<StringPiece>& StringPieces) {
    value_ = StringPieces;
}

inline
StringPiece RedisReply::stringValue() const {
    if (type_ == RedisReplyType::STRING) {
        return boost::get<StringPiece>(value_);
    }

    return StringPiece();
}

inline
StringPiece RedisReply::status() const {
    if (type_ == RedisReplyType::STATUS) {
        return boost::get<StringPiece>(value_);
    }

    return StringPiece();
}

inline
StringPiece RedisReply::error() const {
    if (type_ == RedisReplyType::ERROR) {
        return boost::get<StringPiece>(value_);
    }

    return StringPiece();
}

inline
bool RedisReply::isNil() const {
    return type_ == RedisReplyType::NIL;
}

inline
const std::vector<StringPiece>& RedisReply::array() const {
    if (type_ == RedisReplyType::ARRAY) {
        return boost::get<std::vector<StringPiece> >(value_);
    }

    return EMPTY_STRING_PIECES;
}

}
}
}

#endif //#if !defined(CETTY_REDIS_PROTOCOL_REDISREPLYMESSAGE_H)

// Local Variables:
// mode: c++
// End:
