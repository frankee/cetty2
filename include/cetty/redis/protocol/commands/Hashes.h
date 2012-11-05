#if !defined(CETTY_REDIS_PROTOCOL_COMMAND_HASHES_H)
#define CETTY_REDIS_PROTOCOL_COMMAND_HASHES_H

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
#include <string>

#include <cetty/Types.h>
#include <cetty/util/StringPiece.h>
#include <cetty/redis/protocol/RedisCommandPtr.h>

namespace cetty {
namespace redis {
namespace protocol {
namespace commands {

using namespace cetty::util;
using namespace cetty::redis;

RedisCommandPtr hashesCommandDel(const std::string& key, const std::string& field);
RedisCommandPtr hashesCommandDel(const std::string& key, const std::vector<std::string>& fields);

RedisCommandPtr hashesCommandExist(const std::string& key, const std::string& field);

RedisCommandPtr hashesCommandGet(const std::string& key, const std::string& field);
RedisCommandPtr hashesCommandGet(const std::string& key, const std::vector<std::string>& fields);

RedisCommandPtr hashesCommandGetAll(const std::string& key);
RedisCommandPtr hashesCommandIncrementBy(const std::string& key,
        const std::string& field,
        int64_t increment);

RedisCommandPtr hashesCommandIncrementBy(const std::string& key,
        const std::string& field,
        double increment);

RedisCommandPtr hashesCommandKeys(const std::string& key);

RedisCommandPtr hashesCommandFieldCnt(const std::string& key);


RedisCommandPtr hashesCommandSet(const std::string& key,
                                 const std::vector<std::pair<std::string, std::string> >& values);

RedisCommandPtr hashesCommandSet(const std::string& key,
                                 const std::vector<std::pair<std::string, StringPiece> >& values);

RedisCommandPtr hashesCommandSet(const std::string& key,
                                 const std::vector<std::string>& fields,
                                 const std::vector<std::string>& values);

RedisCommandPtr hashesCommandSet(const std::string& key,
                                 const std::vector<std::string>& fields,
                                 const std::vector<StringPiece>& values);

RedisCommandPtr hashesCommandSet(const std::string& key,
                                 const std::string& field,
                                 const std::string& value);

RedisCommandPtr hashesCommandSet(const std::string& key,
                                 const std::string& field,
                                 const StringPiece& value);

RedisCommandPtr hashesCommandSetNx(const std::string& key,
                                   const std::string& field,
                                   const std::string& value);

RedisCommandPtr hashesCommandSetNx(const std::string& key,
                                   const std::string& field,
                                   const StringPiece& value);

RedisCommandPtr hashesCommandValues(const std::string& key);

}
}
}
}

#endif //#if !defined(CETTY_REDIS_PROTOCOL_COMMAND_HASHES_H)

// Local Variables:
// mode: c++
// End:
