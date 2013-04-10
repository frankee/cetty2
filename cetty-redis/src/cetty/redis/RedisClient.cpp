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

#include <cetty/service/ClientService.h>
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

void RedisClient::statusCallBack(const RedisServiceFuture& future,
                                 const RedisReplyPtr& reply,
                                 const RedisClient::StatusCallBack& callback) {
    if (callback) {
        if (!reply) {
            callback(-1);
        }
        else if (reply->type() == RedisReplyType::STATUS) {
            callback(0);
        }
        else if (reply->type() == RedisReplyType::ERROR) {
            callback(-1);
        }
    }
    else {
        if (!reply) {
            LOG_DEBUG << "redis replied: " << reply->toString();
        }
        else {
            LOG_DEBUG << "has not set callback and no replay from redis";
        }
    }
}

void RedisClient::bulkCallBack(const RedisServiceFuture& future,
                               const RedisReplyPtr& reply,
                               const RedisClient::BulkCallBack& callback) {
    if (callback) {
        if (!reply) {
            callback(-1, StringPiece());
        }
        else if (reply->type() == RedisReplyType::NIL) {
            callback(-1, StringPiece());
        }
        else if (reply->type() == RedisReplyType::STATUS) {
            callback(0, reply->status());
        }
        else if (reply->type() == RedisReplyType::STRING) {
            callback(0, reply->stringValue());
        }
    }
    else {
        if (!reply) {
            LOG_DEBUG << "redis replied: " << reply->toString();
        }
        else {
            LOG_DEBUG << "has not set callback and no replay from redis";
        }
    }
}

void RedisClient::multiBulkCallBack(const RedisServiceFuture& future,
                                    const RedisReplyPtr& reply,
                                    const MultiBulkCallBack& callback) {
    if (callback) {
        if (!reply) {
            callback(-1, std::vector<StringPiece>());
        }
        else if (reply->type() == RedisReplyType::NIL) {
            callback(-1, std::vector<StringPiece>());
        }
        else if (reply->type() == RedisReplyType::ARRAY) {
            callback(0, reply->array());
        }
    }
    else {
        if (!reply) {
            LOG_DEBUG << "redis replied: " << reply->toString();
        }
        else {
            LOG_DEBUG << "has not set callback and no replay from redis";
        }
    }
}

void RedisClient::request(const RedisCommandPtr& command,
                          const RedisServiceFuturePtr& future) {
    cetty::service::callMethod(channel_, command, future);
}

void RedisClient::set(const std::string& key, const StringPiece& value) {
    RedisServiceFuturePtr future(
        new RedisServiceFuture(boost::bind(
                                   &RedisClient::statusCallBack,
                                   _1,
                                   _2,
                                   StatusCallBack())));

    request(stringsCommandSet(key, value), future);
}

void RedisClient::get(const std::string& key, const BulkCallBack& callback) {
    RedisServiceFuturePtr future(new RedisServiceFuture(
                                     boost::bind(
                                         &RedisClient::bulkCallBack,
                                         _1,
                                         _2,
                                         callback)));

    request(stringsCommandGet(key), future);
}

void RedisClient::del(const std::string& key) {
    RedisServiceFuturePtr future(
        new RedisServiceFuture(boost::bind(
                                   &RedisClient::statusCallBack,
                                   _1,
                                   _2,
                                   StatusCallBack())));

    request(keysCommandDel(key), future);
}

void RedisClient::beginTransaction(const StatusCallBack& callback) {
    RedisServiceFuturePtr future(
        new RedisServiceFuture(boost::bind(
                                   &RedisClient::statusCallBack,
                                   _1,
                                   _2,
                                   callback)));

    request(transactionsCommandMulti(), future);
}

void RedisClient::commitTransaction(const StatusCallBack& callback) {
    RedisServiceFuturePtr future(
        new RedisServiceFuture(boost::bind(
                                   &RedisClient::statusCallBack,
                                   _1,
                                   _2,
                                   callback)));

    request(transactionsCommandExec(), future);
}

}
}
