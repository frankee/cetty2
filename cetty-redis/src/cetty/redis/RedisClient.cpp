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

#include "RedisClient.h"

#include <deque>
#include <boost/bind.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/NullChannel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/bootstrap/ClientBootstrap.h>
#include <cetty/channel/socket/asio/AsioClientSocketChannelFactory.h>

#include <cetty/redis/RedisCommand.h>
#include <cetty/redis/RedisMessageHandler.h>
#include <cetty/redis/RedisResponseDecoder.h>

namespace cetty {
namespace redis {

using namespace cetty::channel;
using namespace cetty::bootstrap;
using namespace cetty::channel::socket::asio;

void dummyCallBack(const RedisReplyMessagePtr& reply) {
#if defined(_DEBUG)
    printf("reply.\n");
#endif
}

void dummyStatusCallBack(int status) {

}

void statusCallBack(const RedisReplyMessagePtr& reply, const RedisClient::StatusCallBack& callback) {
    if (!reply) {
        callback(-1);
    }
    else if (reply->getType() == RedisReplyMessageType::STATUS) {
        callback(0);
    }
    else if (reply->getType() == RedisReplyMessageType::ERROR) {
        callback(-1);
    }
}

void integerCallBack(const RedisReplyMessagePtr& reply, const RedisClient::StatusCallBack& callback) {
    if (!reply) {
        callback(-1);
    }
    else if (reply->getType() == RedisReplyMessageType::INTEGER) {
        callback(0);
    }
    else if (reply->getType() == RedisReplyMessageType::STATUS) {
        callback(0);
    }
    else if (reply->getType() == RedisReplyMessageType::ERROR) {
        callback(-1);
    }
}

void stringCallBack(const RedisReplyMessagePtr& reply, const RedisClient::StringCallBack& callback) {
    if (!reply) {
        callback(-1, SimpleString());
    }
    else if (reply->getType() == RedisReplyMessageType::NIL) {
        callback(-1, SimpleString());
    }
    else if (reply->getType() == RedisReplyMessageType::STATUS) {
        callback(0, reply->getStatus());
    }
    else if (reply->getType() == RedisReplyMessageType::STRING) {
        callback(0, reply->getString());
    }
}

void arrayCallBack(const RedisReplyMessagePtr& reply, const RedisClient::ArrayCallBack& callback) {
    if (!reply) {
        callback(-1, std::vector<SimpleString>());
    }
    else if (reply->getType() == RedisReplyMessageType::NIL) {
        callback(-1, std::vector<SimpleString>());
    }
    else if (reply->getType() == RedisReplyMessageType::ARRAY) {
        callback(0, reply->getArray());
    }
}

void RedisClient::request(const RedisCommand* request, const RedisServiceFuturePtr& future) {

    if (context->connected()) {
        context->write(commandMaker.done());
        context->appendCallback(done);
    }
    else {
        done(RedisReplyMessagePtr());
    }
}

void RedisClient::set(const std::string& key, const SimpleString& value) {
    request(RedisCommand("SET") << key << value, boost::bind(dummyCallBack, _1));
}

void RedisClient::set(const std::string& key, const std::string& value) {
    request(RedisCommand("SET") << key << value, boost::bind(dummyCallBack, _1));
}

void RedisClient::get(const std::string& key, const StringCallBack& done) {
    request(RedisCommand("GET") << key, boost::bind(stringCallBack, _1, done));
}

void RedisClient::mget(const std::vector<std::string>& keys, const ArrayCallBack& done) {
    RedisCommand maker("MGET");

    for (std::size_t i = 0; i < keys.size(); ++i) {
        maker << keys[i];
    }

    request(maker, boost::bind(arrayCallBack, _1, done));
}

void RedisClient::rename(const std::string& key, const std::string& newKey, const StatusCallBack& done) {
    request(RedisCommand("RENAME") << key << newKey,boost::bind(statusCallBack, _1, done));
}

void RedisClient::beginTransaction(const StatusCallBack& done) {
    request(RedisCommand("MULTI"), boost::bind(statusCallBack, _1, done));
}

void RedisClient::commitTransaction(const StatusCallBack& done) {
    request(RedisCommand("EXEC"), boost::bind(statusCallBack, _1, done));
}

void RedisClient::hset(const std::string& key, const std::string& field, const std::string& value, const StatusCallBack& done) {
    request(RedisCommand("HSET") << key << field << value, boost::bind(integerCallBack, _1, done));
}

void RedisClient::hset(const std::string& key, const std::string& field, const SimpleString& value, const StatusCallBack& done) {
    request(RedisCommand("HSET") << key << field << value, boost::bind(integerCallBack, _1, done));
}

void RedisClient::hmset(const std::string& key, const std::vector<std::pair<std::string, std::string> >& fields, const StatusCallBack& done /*= DUMY_STATUS_CALL_BACK*/) {
    RedisCommand maker("HMSET");

    maker << key;

    for (std::size_t i = 0; i < fields.size(); ++i) {
        maker << fields[i].first << fields[i].second;
    }

    request(maker, boost::bind(integerCallBack, _1, done));
}

void RedisClient::hmset(const std::string& key, const std::vector<std::pair<std::string, SimpleString> >& fields, const StatusCallBack& done /*= DUMY_STATUS_CALL_BACK*/) {
    RedisCommand maker("HMSET");

    maker << key;

    for (std::size_t i = 0; i < fields.size(); ++i) {
        maker << fields[i].first << fields[i].second;
    }

    request(maker, boost::bind(integerCallBack, _1, done));
}

void RedisClient::hget(const std::string& key, const std::string& field, const StringCallBack& done) {
    request(RedisCommand("HGET") << key << field, boost::bind(stringCallBack, _1, done));
}

void RedisClient::hmget(const std::string& key, const std::vector<std::string>& fields, const ArrayCallBack& done) {
    RedisCommand maker("HMGET");

    maker << key;

    for (std::size_t i = 0; i < fields.size(); ++i) {
        maker << fields[i];
    }

    request(maker, boost::bind(arrayCallBack, _1, done));
}

void RedisClient::hsetnx(const std::string& key, const std::string& field, const std::string& value, const StatusCallBack& done /*= DUMY_STATUS_CALL_BACK*/) {
    command(RedisCommand("HSETNX") << key << field << value, boost::bind(integerCallBack, _1, done));
}

void RedisClient::hsetnx(const std::string& key, const std::string& field, const SimpleString& value, const StatusCallBack& done /*= DUMY_STATUS_CALL_BACK*/) {
    hsetnx(key, field, value).request()
    command(RedisCommand("HSETNX") << key << field << value, boost::bind(integerCallBack, _1, done));
}

void RedisClient::setnx(const std::string& key, const std::string& value) {
    command(RedisCommand("SETNX") << key << value, boost::bind(dummyCallBack, _1));
}

void RedisClient::setnx(const std::string& key, const SimpleString& value) {
    request(RedisCommand("SETNX") << key << value, boost::bind(dummyCallBack, _1));
}

}
}