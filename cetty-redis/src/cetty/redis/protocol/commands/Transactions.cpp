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

#include <cetty/redis/protocol/commands/Transactions.h>

namespace cetty {
namespace redis {
namespace protocol {
namespace commands {

// Miscellaneous
const std::string DISCARD  = "DISCARD";
const std::string EXEC     = "EXEC";
const std::string MULTI    = "MULTI";
const std::string UNWATCH  = "UNWATCH";
const std::string WATCH    = "WATCH";

RedisCommandPtr transactionsCommandDiscard() {
    RedisCommandPtr command(new RedisCommand(DISCARD));
    command->done();
    return command;
}

RedisCommandPtr transactionsCommandExec() {
    RedisCommandPtr command(new RedisCommand(EXEC));
    command->done();
    return command;
}

RedisCommandPtr transactionsCommandMulti() {
    RedisCommandPtr command(new RedisCommand(MULTI));
    command->done();
    return command;
}

RedisCommandPtr transactionsCommandUnwatch() {
    RedisCommandPtr command(new RedisCommand(UNWATCH));
    command->done();
    return command;
}

RedisCommandPtr transactionsCommandWatch(const std::string& key) {
    RedisCommandPtr command(new RedisCommand(WATCH));
    (*command << key).done();
    return command;
}

}
}
}
}
