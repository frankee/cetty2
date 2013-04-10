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
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

#include <cetty/util/StringPiece.h>
#include <cetty/channel/ChannelPtr.h>
#include <cetty/redis/RedisServiceFuture.h>
#include <cetty/redis/protocol/RedisCommandPtr.h>
#include <cetty/redis/protocol/RedisReplyPtr.h>
#include <cetty/redis/protocol/commands/Strings.h>

namespace cetty {
namespace redis {

using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::service;
using namespace cetty::redis::protocol;

class RedisClient : private boost::noncopyable {
public:
    RedisClient(const ChannelPtr& channel)
        : channel_(channel) {}

    ~RedisClient() {}

    typedef boost::function2<void,
            const RedisServiceFuture&,
            const RedisReplyPtr&> ReplyCallback;

    typedef boost::function1<void, int> StatusCallBack;
    typedef boost::function2<void, int, const StringPiece&> BulkCallBack;
    typedef boost::function2<void, int, const std::vector<StringPiece>&> MultiBulkCallBack;

public:
    void request(const RedisCommandPtr& command,
                 const RedisServiceFuturePtr& future);

    // Strings Command
    void set(const std::string& key, const StringPiece& value);

    void setnx(const std::string& key, const StringPiece& value);

    void get(const std::string& key, const BulkCallBack& callback);

    template<typename Iterator>
    void get(const Iterator& keyBegin,
             const Iterator& keyEnd,
             const MultiBulkCallBack& callback) {
        RedisCommandPtr command =
            commands::stringsCommandGet<Iterator>(keyBegin, keyEnd);

        RedisServiceFuturePtr future(new RedisServiceFuture(
                                         boost::bind(
                                             &RedisClient::multiBulkCallBack,
                                             _1,
                                             _2,
                                             callback)));
        request(command, future);
    }

    void del(const std::string& key);

#if 0
    void hset(const std::string& key, const std::string& field, const std::string& value);
    void hset(const std::string& key, const std::string& field, const StringPiece& value);

    void hmset(const std::string& key, const std::vector<std::pair<std::string, std::string> >& fields, const StatusCallBack& done);
    void hmset(const std::string& key, const std::vector<std::pair<std::string, StringPiece> >& fields, const StatusCallBack& done);

    void hsetnx(const std::string& key, const std::string& field, const std::string& value, const StatusCallBack& done);
    void hsetnx(const std::string& key, const std::string& field, const StringPiece& value, const StatusCallBack& done);

    void hget(const std::string& key, const std::string& field, const BulkCallBack& done);
    void hmget(const std::string& key, const std::vector<std::string>& fields, const MultiBulkCallBack& done);

    void rename(const std::string& key, const std::string& newKey, const StatusCallBack& done);
#endif

    void beginTransaction(const StatusCallBack& callback);
    void commitTransaction(const StatusCallBack& callback);

private:
    static void statusCallBack(const RedisServiceFuture& future,
                               const RedisReplyPtr& reply,
                               const RedisClient::StatusCallBack& callback);

    static void bulkCallBack(const RedisServiceFuture& future,
                             const RedisReplyPtr& reply,
                             const RedisClient::BulkCallBack& callback);

    static void multiBulkCallBack(const RedisServiceFuture& future,
                                  const RedisReplyPtr& reply,
                                  const MultiBulkCallBack& callback);

private:
    ChannelPtr channel_;
};

}
}

#endif //#if !defined(CETTY_REDIS_REDISCLIENT_H)

// Local Variables:
// mode: c++
// End:
