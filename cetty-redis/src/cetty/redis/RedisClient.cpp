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

#include <cetty/redis/RedisClient.h>

#include <deque>
#include <boost/bind.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/NullChannel.h>
#include <cetty/channel/ChannelFuture.h>

#include <cetty/logging/LoggerHelper.h>

#include <cetty/redis/protocol/RedisReply.h>
#include <cetty/redis/protocol/RedisCommand.h>
#include <cetty/redis/protocol/commands/Keys.h>
#include <cetty/redis/protocol/commands/Strings.h>
#include <cetty/redis/protocol/commands/Transactions.h>


namespace cetty {
namespace redis {

using namespace cetty::channel;
using namespace cetty::redis::protocol;
using namespace cetty::redis::protocol::commands;

void dummySetCallback(const RedisServiceFuture& future,
                      const RedisReplyPtr& reply) {
    LOG_DEBUG << "redis replied the set command";
}

void RedisClient::statusCallBack(const RedisServiceFuture& future,
                    const RedisReplyPtr& reply,
                    const RedisClient::StatusCallBack& callback) {
    if (!reply) {
        callback(-1);
    }
    else if (reply->getType() == RedisReplyType::STATUS) {
        callback(0);
    }
    else if (reply->getType() == RedisReplyType::ERROR) {
        callback(-1);
    }
}

void integerCallBack(const RedisReplyPtr& reply,
                     const RedisClient::StatusCallBack& callback) {
    if (!reply) {
        callback(-1);
    }
    else if (reply->getType() == RedisReplyType::INTEGER) {
        callback(0);
    }
    else if (reply->getType() == RedisReplyType::STATUS) {
        callback(0);
    }
    else if (reply->getType() == RedisReplyType::ERROR) {
        callback(-1);
    }
}

void RedisClient::bulkCallBack(const RedisServiceFuture& future,
                  const RedisReplyPtr& reply,
                  const RedisClient::BulkCallBack& callback) {
    if (!reply) {
        callback(-1, StringPiece());
    }
    else if (reply->getType() == RedisReplyType::NIL) {
        callback(-1, StringPiece());
    }
    else if (reply->getType() == RedisReplyType::STATUS) {
        callback(0, reply->getStatus());
    }
    else if (reply->getType() == RedisReplyType::STRING) {
        callback(0, reply->getString());
    }
}

void RedisClient::multiBulkCallBack(const RedisServiceFuture& future,
                                    const RedisReplyPtr& reply,
                                    const MultiBulkCallBack& callback) {
    if (!reply) {
        callback(-1, std::vector<StringPiece>());
    }
    else if (reply->getType() == RedisReplyType::NIL) {
        callback(-1, std::vector<StringPiece>());
    }
    else if (reply->getType() == RedisReplyType::ARRAY) {
        callback(0, reply->getArray());
    }
}

void RedisClient::request(const RedisCommandPtr& command,
                          const RedisServiceFuturePtr& future) {
    callMethod(clientService, command, future);
}

void RedisClient::set(const std::string& key, const StringPiece& value) {
    RedisServiceFuturePtr future(
        new RedisServiceFuture(boost::bind(dummySetCallback, _1, _2)));

    request(stringsCommandSet(key, value), future);
}

void RedisClient::get(const std::string& key, const BulkCallBack& callback) {
    RedisServiceFuturePtr future(new RedisServiceFuture(
        boost::bind(&RedisClient::bulkCallBack, _1, _2, callback)));

    request(stringsCommandGet(key), future);
}

void RedisClient::del(const std::string& key) {
    RedisServiceFuturePtr future(
        new RedisServiceFuture(boost::bind(dummySetCallback, _1, _2)));

    request(keysCommandDel(key), future);
}

void RedisClient::beginTransaction(const StatusCallBack& callback) {
    RedisServiceFuturePtr future(new RedisServiceFuture(
        boost::bind(&RedisClient::statusCallBack, _1, _2, callback)));

    request(transactionsCommandMulti(), future);
}

void RedisClient::commitTransaction(const StatusCallBack& callback) {
    RedisServiceFuturePtr future(new RedisServiceFuture(
        boost::bind(&RedisClient::statusCallBack, _1, _2, callback)));

    request(transactionsCommandExec(), future);
}

}
}
