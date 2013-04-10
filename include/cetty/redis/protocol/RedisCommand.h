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

#include <cetty/util/StringPiece.h>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/redis/protocol/RedisCommandPtr.h>

namespace cetty {
namespace redis {
namespace protocol {

using namespace cetty::util;
using namespace cetty::buffer;

class RedisCommand : public cetty::util::ReferenceCounter<RedisCommand, int> {
public:
    RedisCommand(const std::string& name);
    ~RedisCommand() {}

    const std::string& name() const;

    RedisCommand& append(const char* param, int size);

    RedisCommand& append(const std::string& param);

    RedisCommand& operator<< (const std::string& param);

    RedisCommand& operator<< (const StringPiece& param);

    template <typename T>
    RedisCommand& operator<<(T const& datum);

    RedisCommand& operator<<(const std::vector<std::string>& data);

    RedisCommand& operator<<(const std::vector<StringPiece>& data);

    template <typename T>
    RedisCommand& operator<<(const std::vector<T>& data);

    void done();

    const ChannelBufferPtr& buffer() const;

private:
    int paramCnt_;
    std::string name_;
    ChannelBufferPtr buffer_;
};

inline
const std::string& RedisCommand::name() const {
    return name_;
}

inline
RedisCommand& RedisCommand::operator<<(const std::string& param) {
    return append(param);
}

inline
RedisCommand& RedisCommand::operator<<(const StringPiece& param) {
    return append(param.data(), param.length());
}

template <typename T> inline
RedisCommand& RedisCommand::operator<<(T const& datum) {
    try {
        return append(boost::lexical_cast<std::string>(datum));
    }
    catch (const std::exception& e) {
        LOG_ERROR << "append data has exception " << e.what();
        return *this;
    }
}

inline
RedisCommand& RedisCommand::operator<<(const std::vector<std::string>& data) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        append(data[i]);
    }

    return *this;
}

inline
RedisCommand& RedisCommand::operator<<(const std::vector<StringPiece>& data) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        append(data[i].data(), data[i].length());
    }

    return *this;
}

template <typename T> inline
RedisCommand& RedisCommand::operator<<(const std::vector<T>& data) {
    try {
        for (std::size_t i = 0; i < data.size(); ++i) {
            append(boost::lexical_cast<std::string>(data[i]));
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR << "append data has exception " << e.what();
        return *this;
    }

    return *this;
}

inline
const ChannelBufferPtr& RedisCommand::buffer() const {
    return buffer_;
}

}
}
}

#endif //#if !defined(CETTY_REDIS_PROTOCOL_REDISCOMMAND_H)

// Local Variables:
// mode: c++
// End:
