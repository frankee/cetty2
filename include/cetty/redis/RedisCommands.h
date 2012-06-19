#if !defined(CETTY_REDIS_REDISCOMMANDS_H)
#define CETTY_REDIS_REDISCOMMANDS_H

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

namespace cetty {
namespace redis {

class RedisCommands {
public:
    // Key Commands
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

    // String Commands
    static const std::string APPEND    = "APPEND";
    static const std::string DECR      = "DECR";
    static const std::string DECRBY    = "DECRBY";
    static const std::string GET       = "GET";
    static const std::string GETBIT    = "GETBIT";
    static const std::string GETRANGE  = "GETRANGE";
    static const std::string GETSET    = "GETSET";
    static const std::string INCR      = "INCR";
    static const std::string INCRBY    = "INCRBY";
    static const std::string MGET      = "MGET";
    static const std::string MSET      = "MSET";
    static const std::string MSETNX    = "MSETNX";
    static const std::string SET       = "SET";
    static const std::string SETBIT    = "SETBIT";
    static const std::string SETEX     = "SETEX";
    static const std::string SETNX     = "SETNX";
    static const std::string SETRANGE  = "SETRANGE";
    static const std::string STRLEN    = "STRLEN";

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

    // Miscellaneous
    static const std::string FLUSHDB           = "FLUSHDB";
    static const std::string SELECT            = "SELECT";
    static const std::string AUTH              = "AUTH";
    static const std::string QUIT              = "QUIT";

    // Hash Sets
    static const std::string HDEL              = "HDEL";
    static const std::string HGET              = "HGET";
    static const std::string HGETALL           = "HGETALL";
    static const std::string HMGET             = "HMGET";
    static const std::string HSET              = "HSET";
};

}
}

#endif //#if !defined(CETTY_REDIS_REDISCOMMANDS_H)

// Local Variables:
// mode: c++
// End:
