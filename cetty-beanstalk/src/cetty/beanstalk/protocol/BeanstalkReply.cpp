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

#include <cetty/beanstalk/protocol/BeanstalkReply.h>

#include <cetty/util/StringUtil.h>

namespace cetty {
namespace beanstalk {
namespace protocol {

using namespace cetty::util;

const BeanstalkReplyType BeanstalkReplyType::INSERTED(1, "INSERTED");
const BeanstalkReplyType BeanstalkReplyType::BURIED(2, "BURIED");
const BeanstalkReplyType BeanstalkReplyType::USING(3, "USING");
const BeanstalkReplyType BeanstalkReplyType::RESERVED(4, "RESERVED");
const BeanstalkReplyType BeanstalkReplyType::WATCHING(5, "WATCHING");
const BeanstalkReplyType BeanstalkReplyType::KICKED(6, "KICKED");
const BeanstalkReplyType BeanstalkReplyType::FOUND(7, "FOUND");
const BeanstalkReplyType BeanstalkReplyType::OK(8, "OK");
const BeanstalkReplyType BeanstalkReplyType::INVALID(-1, "INVALID");

BeanstalkReplyType::BeanstalkReplyType(int value, const char* name)
    : Enum<BeanstalkReplyType>(value, name) {
    if (defaultEnum()) {
        setDefaultEnum(new BeanstalkReplyType(-1, "INVALID"));
    }
}

std::string BeanstalkReply::toString() const {
    std::string tmp(type_.name());

    if (id_ >= 0) {
        StringUtil::printf(&tmp, " id: %d", id_);
    }

    if (count_ >= 0) {
        StringUtil::printf(&tmp, " count: %d", count_);
    }

    if (!tube_.empty()) {
        StringUtil::printf(&tmp, " tube: %s", tube_.c_str());
    }

    return tmp;
}

}
}
}
