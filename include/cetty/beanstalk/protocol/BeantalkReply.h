/*
 * BeantalkReply.h
 *
 *  Created on: Mar 11, 2013
 *      Author: chenhl
 */

#ifndef BEANTALKREPLY_H_
#define BEANTALKREPLY_H_

#include <cetty/Types.h>
#include <cetty/util/Enum.h>
#include <cetty/util/StringPiece.h>
#include <cetty/util/ReferenceCounter.h>

#include <boost/variant.hpp>
#include <vector>

#if defined(WIN32)
#if defined(ERROR)
#undef ERROR
#endif
#endif

namespace cetty {
namespace beanstalk {
namespace protocol {

using namespace cetty::util;

class BeanstalkReply : public ReferenceCounter<BeanstalkReply> {
public:
	BeanstalkReply(){}

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

typedef boost::intrusive_ptr<BeanstalkReply> BeanstalkReplyPtr;

}
}
}


#endif /* BEANTALKREPLY_H_ */
