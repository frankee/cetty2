#if !defined(CETTY_REDIS_PROTOCOL_COMMAND_STRINGS_H)
#define CETTY_REDIS_PROTOCOL_COMMAND_STRINGS_H

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

#include <cetty/Types.h>
#include <cetty/util/StringPiece.h>
#include <cetty/redis/protocol/RedisCommand.h>

namespace cetty {
namespace redis {
namespace protocol {
namespace commands {

extern const std::string MGET;
extern const std::string MSET;
extern const std::string MSETNX;

using namespace cetty::util;

RedisCommandPtr stringsCommandAppend(const std::string& key, const StringPiece& value);

RedisCommandPtr stringsCommandBitcount(const std::string& key, int start, int end);
RedisCommandPtr stringsCommandDecrement(const std::string& key);
RedisCommandPtr stringsCommandDecrementBy(const std::string& key, int64_t value);

RedisCommandPtr stringsCommandGet(const std::string& key);

template<typename Iterator>
RedisCommandPtr stringsCommandGet(const Iterator& begin, const Iterator& end) {
    RedisCommandPtr command(new RedisCommand(MGET));

    for (Iterator itr = begin; itr != end; ++itr) {
        *command << *itr;
    }

    command->done();
    return command;
}

RedisCommandPtr stringsCommandGetBit(const std::string& key, int offset);
RedisCommandPtr stringsCommandGetRange(const std::string& key, int start, int end);

RedisCommandPtr stringsCommandGetset(const std::string& key, const StringPiece& value);

RedisCommandPtr stringsCommandIncrement(const std::string& key);
RedisCommandPtr stringsCommandIncrement(const std::string& key, int64_t value);
RedisCommandPtr stringsCommandIncrement(const std::string& key, double value);

template<typename Iterator>
RedisCommandPtr stringsCommandSet(const std::string& key, const Iterator& begin, const Iterator& end) {
    RedisCommandPtr command(new RedisCommand(MSET));
    *command << key;

    for (Iterator itr = begin; itr != end; ++itr) {
        *command << *itr;
    }

    command->done();
    return command;
}

RedisCommandPtr stringsCommandSet(const std::string& key, const StringPiece& value, int expiration);

RedisCommandPtr stringsCommandSet(const std::string& key, const StringPiece& value);

RedisCommandPtr stringCommandSetBit(const std::string& key, const StringPiece& value, int offset);

template<typename Iterator>
RedisCommandPtr stringsCommandSetNx(const std::string& key, const Iterator& begin, const Iterator& end) {
    RedisCommandPtr command(new RedisCommand(MSETNX));
    *command << key;

    for (Iterator itr = begin; itr != end; ++itr) {
        *command << *itr;
    }

    command->done();
    return command;
}

RedisCommandPtr stringsCommandSetNx(const std::string& key, const StringPiece& value);
RedisCommandPtr stringsCommandSetRange(const std::string& key, int offset, const StringPiece& value);

RedisCommandPtr stringsCommandStrlen(const std::string& key);

}
}
}
}

#endif //#if !defined(CETTY_REDIS_PROTOCOL_COMMAND_STRINGS_H)

// Local Variables:
// mode: c++
// End:
