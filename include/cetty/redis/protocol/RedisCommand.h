#if !defined(CETTY_REDIS_PROTOCOL_REDISCOMMAND_H)
#define CETTY_REDIS_PROTOCOL_REDISCOMMAND_H

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/Unpooled.h>

#include <cetty/util/Integer.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/StringPiece.h>
#include <cetty/util/ReferenceCounter.h>

#include <cetty/redis/protocol/RedisCommandPtr.h>

namespace cetty {
namespace redis {
namespace protocol {

using namespace cetty::buffer;
using namespace cetty::util;

class RedisCommand : public cetty::util::ReferenceCounter<RedisCommand, int> {
public:
    RedisCommand(const std::string& name)
        : paramCnt(0), name(name) {
        buffer = Unpooled::buffer(512*1024);
        buffer->setIndex(12, 12);
        append(name);
    }

    ~RedisCommand() {}

    const std::string& getCommandName() const {
        return this->name;
    }

    RedisCommand& append(const char* param, int size) {
        if (NULL == param || size <= 0) {
            return *this;
        }

        buffer->writeBytes(StringUtil::strprintf("$%d\r\n", size));
        buffer->writeBytes(param, size);
        buffer->writeBytes("\r\n");

        ++paramCnt;
        return *this;
    }

    RedisCommand& append(const std::string& param) {
        if (param.empty()) {
            return *this;
        }

        buffer->writeBytes(StringUtil::strprintf("$%d\r\n", param.size()));
        buffer->writeBytes(param); // binary safe string
        buffer->writeBytes("\r\n");

        ++paramCnt;
        return *this;
    }

    RedisCommand& operator<< (const std::string& param) {
        return append(param);
    }

    RedisCommand& operator<< (const StringPiece& param) {
        return append(param.data(), param.length());
    }

    template <typename T>
    RedisCommand& operator<<(T const& datum) {
        return append(boost::lexical_cast<std::string>(datum));
    }

    RedisCommand& operator<<(const std::vector<std::string>& data) {
        for (std::size_t i = 0; i < data.size(); ++i) {
            append(data[i]);
        }

        return *this;
    }

    RedisCommand& operator<<(const std::vector<StringPiece>& data) {
        for (std::size_t i = 0; i < data.size(); ++i) {
            append(data[i].data(), data[i].length());
        }

        return *this;
    }

    template <typename T>
    RedisCommand& operator<<(const std::vector<T>& data) {
        for (std::size_t i = 0; i < data.size(); ++i) {
            append(boost::lexical_cast<std::string>(data[i]));
        }

        return *this;
    }

    void done() {
        std::string header;
        StringUtil::strprintf(&header, "*%d\r\n", paramCnt);

        BOOST_ASSERT(buffer->aheadWritableBytes() >= (int)header.size());

        buffer->writeBytesAhead(header);
    }

    const ChannelBufferPtr& getBuffer() const { return buffer; }

private:
    int paramCnt;
    std::string name;
    ChannelBufferPtr buffer;
};

}
}
}

#endif //#if !defined(CETTY_REDIS_PROTOCOL_REDISCOMMAND_H)

// Local Variables:
// mode: c++
// End:
