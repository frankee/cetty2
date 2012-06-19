#if !defined(CETTY_REDIS_REDISCLIENT_H)
#define CETTY_REDIS_REDISCLIENT_H

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

#include <map>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/redis/RedisServiceFuture.h>

namespace cetty {
namespace redis {

using namespace cetty::util;

class RedisCommand;

class RedisClient {
public:
    RedisClient() {}
    ~RedisClient() {}

public:
    void request(const RedisCommandPtr& command, const RedisServiceFuturePtr& future);

    // Strings Command
    void set(const std::string& key, const std::string& value, const RedisServiceFuturePtr& future);
    void set(const std::string& key, const char* value, int length, const RedisServiceFuturePtr& future);

    void setnx(const std::string& key, const std::string& value, const RedisServiceFuturePtr& future);
    void setnx(const std::string& key, const char* value, int length, const RedisServiceFuturePtr& future);

    void get(const std::string& key, const StringCallBack& done);
    void mget(const std::vector<std::string>& keys, const ArrayCallBack& done);

    void hset(const std::string& key, const std::string& field, const std::string& value, const StatusCallBack& done = DUMY_STATUS_CALL_BACK);
    void hset(const std::string& key, const std::string& field, const SimpleString& value, const StatusCallBack& done = DUMY_STATUS_CALL_BACK);

    void hmset(const std::string& key, const std::vector<std::pair<std::string, std::string> >& fields, const StatusCallBack& done = DUMY_STATUS_CALL_BACK);
    void hmset(const std::string& key, const std::vector<std::pair<std::string, SimpleString> >& fields, const StatusCallBack& done = DUMY_STATUS_CALL_BACK);

    void hsetnx(const std::string& key, const std::string& field, const std::string& value, const StatusCallBack& done = DUMY_STATUS_CALL_BACK);
    void hsetnx(const std::string& key, const std::string& field, const SimpleString& value, const StatusCallBack& done = DUMY_STATUS_CALL_BACK);

    void hget(const std::string& key, const std::string& field, const StringCallBack& done);
    void hmget(const std::string& key, const std::vector<std::string>& fields, const ArrayCallBack& done);

    void rename(const std::string& key, const std::string& newKey, const StatusCallBack& done);

    void beginTransaction(const StatusCallBack& done);
    void commitTransaction(const StatusCallBack& done);
};

}
}

#endif //#if !defined(CETTY_REDIS_REDISCLIENT_H)

// Local Variables:
// mode: c++
// End:
