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

#include <cetty/redis/protocol/commands/SortedSets.h>

namespace cetty {
namespace redis {
namespace protocol {
namespace commands {

// Sorted Sets
static const std::string ZADD              = "ZADD";
static const std::string ZCARD             = "ZCARD";
static const std::string ZCOUNT            = "ZCOUNT";
static const std::string ZINCRBY           = "ZINCRBY";
static const std::string ZINTERSTORE       = "ZINTERSTORE";
static const std::string ZRANGE            = "ZRANGE";
static const std::string ZRANGEBYSCORE     = "ZRANGEBYSCORE";
static const std::string ZRANK             = "ZRANK";
static const std::string ZREM              = "ZREM";
static const std::string ZREMRANGEBYRANK   = "ZREMRANGEBYRANK";
static const std::string ZREMRANGEBYSCORE  = "ZREMRANGEBYSCORE";
static const std::string ZREVRANGE         = "ZREVRANGE";
static const std::string ZREVRANGEBYSCORE  = "ZREVRANGEBYSCORE";
static const std::string ZREVRANK          = "ZREVRANK";
static const std::string ZSCORE            = "ZSCORE";
static const std::string ZUNIONSTORE       = "ZUNIONSTORE";

}
}
}
}
