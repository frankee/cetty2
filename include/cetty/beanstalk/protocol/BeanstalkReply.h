#if !defined(CETTY_BEANSTALK_PROTOCOL_BEANSTALKREPLY_H)
#define CETTY_BEANSTALK_PROTOCOL_BEANSTALKREPLY_H

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
 *      Author: chenhl
 */

#include <map>
#include <vector>
#include <boost/variant.hpp>

#include <cetty/Types.h>
#include <cetty/util/Enum.h>
#include <cetty/util/StringPiece.h>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/beanstalk/protocol/BeanstalkReplyPtr.h>

namespace cetty {
namespace beanstalk {
namespace protocol {

using namespace cetty::util;

class BeanstalkReplyType : public cetty::util::Enum<BeanstalkReplyType> {
public:
    static const BeanstalkReplyType INSERTED;
    static const BeanstalkReplyType BURIED;
    static const BeanstalkReplyType USING;
    static const BeanstalkReplyType RESERVED;
    static const BeanstalkReplyType WATCHING;
    static const BeanstalkReplyType KICKED;
    static const BeanstalkReplyType FOUND;
    static const BeanstalkReplyType OK;
    static const BeanstalkReplyType INVALID;

private:
    BeanstalkReplyType(int value, const char* name)
        : Enum<BeanstalkReplyType>(value, name) {
    }
};

class BeanstalkReply : public ReferenceCounter<BeanstalkReply, int> {
public:
	BeanstalkReply()
        :id_ (-1),
        count_(-1),
        type_(BeanstalkReplyType::INVALID) {
    }

    int id();
	void setId(int id);
	
    int count();
	void setCount(int count);

    const BeanstalkReplyType& type() const;
    void setType(const BeanstalkReplyType& type);

    const StringPiece& data() const;
    void setData(const StringPiece& data);

    const std::string& tube() const;
    void setTube(const StringPiece& tube);

    std::string toString() const;

private:
    int id_;
    int count_;
    std::string tube_;
    StringPiece data_;
    BeanstalkReplyType type_;
};

inline
int BeanstalkReply::id() {
    return id_;
}

inline
void BeanstalkReply::setId(int id) {
    id_ = id;
}

inline
void BeanstalkReply::setCount(int count) {
    count_ = count;
}

inline
int BeanstalkReply::count() {
    return count_;
}

inline
const BeanstalkReplyType& BeanstalkReply::type() const {
    return type_;
}

inline
void BeanstalkReply::setType(const BeanstalkReplyType& type) {
    type_ = type;
}

inline
const StringPiece& BeanstalkReply::data() const {
    return data_;
}

inline
void BeanstalkReply::setData(const StringPiece& data) {
    data_ = data;
}

inline
const std::string& BeanstalkReply::tube() const {
    return tube_;
}

inline
void BeanstalkReply::setTube(const StringPiece& tube) {
    tube_.assign(tube.c_str(), tube.size());
}

}
}
}

#endif //#if !defined(CETTY_BEANSTALK_PROTOCOL_BEANSTALKREPLY_H)

// Local Variables:
// mode: c++
// End:
