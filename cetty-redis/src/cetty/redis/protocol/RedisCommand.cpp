
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

#include <cetty/redis/protocol/RedisCommand.h>

#include <cetty/util/StringUtil.h>
#include <cetty/buffer/Unpooled.h>

namespace cetty {
namespace redis {
namespace protocol {

using namespace cetty::util;
using namespace cetty::buffer;

RedisCommand::RedisCommand(const std::string& name)
    : paramCnt_(0),
      name_(name) {
    buffer_ = Unpooled::buffer(512*1024);
    buffer_->setIndex(12, 12);
    append(name);
}

RedisCommand& RedisCommand::append(const char* param, int size) {
    if (NULL == param || size <= 0) {
        return *this;
    }

    buffer_->writeBytes(StringUtil::printf("$%d\r\n", size));
    buffer_->writeBytes(param, size);
    buffer_->writeBytes("\r\n");

    ++paramCnt_;
    return *this;
}

RedisCommand& RedisCommand::append(const std::string& param) {
    if (param.empty()) {
        return *this;
    }

    buffer_->writeBytes(StringUtil::printf("$%d\r\n", param.size()));
    buffer_->writeBytes(param); // binary safe string
    buffer_->writeBytes("\r\n");

    ++paramCnt_;
    return *this;
}

void RedisCommand::done() {
    std::string header;
    StringUtil::printf(&header, "*%d\r\n", paramCnt_);

    BOOST_ASSERT(buffer_->aheadWritableBytes() >= (int)header.size());

    buffer_->writeBytesAhead(header);
}

}
}
}
