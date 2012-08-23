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

#include <cetty/redis/command/Keys.h>
#include <cetty/redis/RedisCommand.h>

namespace cetty {
namespace redis {
namespace command {

using namespace cetty::redis;

static const std::string DEL       = "DEL";
static const std::string EXISTS    = "EXISTS";
static const std::string EXPIRE    = "EXPIRE";
static const std::string EXPIREAT  = "EXPIREAT";
static const std::string KEYS      = "KEYS";
static const std::string PERSIST   = "PERSIST";
static const std::string RANDOMKEY = "RANDOMKEY";
static const std::string RENAME    = "RENAME";
static const std::string RENAMENX  = "RENAMENX";
static const std::string TTL       = "TTL";
static const std::string TYPE      = "TYPE";

RedisCommandPtr keysCommandDel(const std::string& key) {
    RedisCommandPtr command(new RedisCommand(DEL));
    (*command << key).done();

    return command;
}

RedisCommandPtr keysCommandDel(const std::vector<std::string>& keys) {
    RedisCommandPtr command(new RedisCommand(DEL));
    (*command << keys).done();
    return command;
}

RedisCommandPtr keysCommandRename(const std::string& key, const std::string& newKey) {
    RedisCommandPtr command(new RedisCommand(RENAME));
    (*command << key << newKey).done();
    return command;
}

RedisCommandPtr keysCommandRenameNx(const std::string& key, const std::string& newKey) {
    RedisCommandPtr command(new RedisCommand(RENAMENX));
    (*command << key << newKey).done();
    return command;
}

}
}
}






