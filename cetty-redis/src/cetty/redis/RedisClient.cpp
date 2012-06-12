
/**
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

#include "RedisCommandMaker.h"
#include "RedisResponseDecoder.h"
#include "RedisMessageHandler.h"

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
    else if (reply->getType() == ReplyType::REPLY_STATUS) {
        callback(0);
    }
    else if (reply->getType() == ReplyType::REPLY_ERROR) {
        callback(-1);
    }
}

void integerCallBack(const RedisReplyMessagePtr& reply, const RedisClient::StatusCallBack& callback) {
    if (!reply) {
        callback(-1);
    }
    else if (reply->getType() == ReplyType::REPLY_INTEGER) {
        callback(0);
    }
    else if (reply->getType() == ReplyType::REPLY_STATUS) {
        callback(0);
    }
    else if (reply->getType() == ReplyType::REPLY_ERROR) {
        callback(-1);
    }
}

void stringCallBack(const RedisReplyMessagePtr& reply, const RedisClient::StringCallBack& callback) {
    if (!reply) {
        callback(-1, SimpleString());
    }
    else if (reply->getType() == ReplyType::REPLY_NIL) {
        callback(-1, SimpleString());
    }
    else if (reply->getType() == ReplyType::REPLY_STATUS) {
        callback(0, reply->getStatus());
    }
    else if (reply->getType() == ReplyType::REPLY_STRING) {
        callback(0, reply->getString());
    }
}

void arrayCallBack(const RedisReplyMessagePtr& reply, const RedisClient::ArrayCallBack& callback) {
    if (!reply) {
        callback(-1, std::vector<SimpleString>());
    }
    else if (reply->getType() == ReplyType::REPLY_NIL) {
        callback(-1, std::vector<SimpleString>());
    }
    else if (reply->getType() == ReplyType::REPLY_ARRAY) {
        callback(0, reply->getArray());
    }
}

const RedisClient::StatusCallBack RedisClient::DUMY_STATUS_CALL_BACK = boost::bind(dummyStatusCallBack, _1);


int cetty::redis::RedisClient::addServer(const std::string& host, int port) {
    RedisClientContext* context = new RedisClientContext(host, port);
    context->connect();

    clientContexts.insert(std::make_pair<std::string, RedisClientContext*>(host, context));
    return 0;
}

void RedisClient::command(RedisCommandMaker& commandMaker, const CallBack& done) {
    RedisClientContext* context = getContext();

    if (context->connected()) {
        context->write(commandMaker.done());
        context->appendCallback(done);
    }
    else {
        done(RedisReplyMessagePtr());
    }

}

RedisClientContext* RedisClient::getContext() {
    RedisClientContext* context = clientContexts.begin()->second;

    if (context) {
        if (!context->connected()) {
            context->connect();
        }
    }

    return context;
}

void RedisClient::set(const std::string& key, const SimpleString& value) {
    command(RedisCommandMaker("SET") << key << value, boost::bind(dummyCallBack, _1));
}

void RedisClient::set(const std::string& key, const std::string& value) {
    command(RedisCommandMaker("SET") << key << value, boost::bind(dummyCallBack, _1));
}

void RedisClient::get(const std::string& key, const StringCallBack& done) {
    command(RedisCommandMaker("GET") << key, boost::bind(stringCallBack, _1, done));
}

void RedisClient::mget(const std::vector<std::string>& keys, const ArrayCallBack& done) {
    RedisCommandMaker maker("MGET");

    for (std::size_t i = 0; i < keys.size(); ++i) {
        maker << keys[i];
    }

    command(maker, boost::bind(arrayCallBack, _1, done));
}

void RedisClient::rename(const std::string& key, const std::string& newKey, const StatusCallBack& done) {
    command(RedisCommandMaker("RENAME") << key << newKey,boost::bind(statusCallBack, _1, done));
}

void RedisClient::beginTransaction(const StatusCallBack& done) {
    command(RedisCommandMaker("MULTI"), boost::bind(statusCallBack, _1, done));
}

void RedisClient::commitTransaction(const StatusCallBack& done) {
    command(RedisCommandMaker("EXEC"), boost::bind(statusCallBack, _1, done));
}

void RedisClient::hset(const std::string& key, const std::string& field, const std::string& value, const StatusCallBack& done) {
    command(RedisCommandMaker("HSET") << key << field << value, boost::bind(integerCallBack, _1, done));
}

void RedisClient::hset(const std::string& key, const std::string& field, const SimpleString& value, const StatusCallBack& done) {
    command(RedisCommandMaker("HSET") << key << field << value, boost::bind(integerCallBack, _1, done));
}

void RedisClient::hmset(const std::string& key, const std::vector<std::pair<std::string, std::string> >& fields, const StatusCallBack& done /*= DUMY_STATUS_CALL_BACK*/) {
    RedisCommandMaker maker("HMSET");

    maker << key;

    for (std::size_t i = 0; i < fields.size(); ++i) {
        maker << fields[i].first << fields[i].second;
    }

    command(maker, boost::bind(integerCallBack, _1, done));
}

void RedisClient::hmset(const std::string& key, const std::vector<std::pair<std::string, SimpleString> >& fields, const StatusCallBack& done /*= DUMY_STATUS_CALL_BACK*/) {
    RedisCommandMaker maker("HMSET");

    maker << key;

    for (std::size_t i = 0; i < fields.size(); ++i) {
        maker << fields[i].first << fields[i].second;
    }

    command(maker, boost::bind(integerCallBack, _1, done));
}

void RedisClient::hget(const std::string& key, const std::string& field, const StringCallBack& done) {
    command(RedisCommandMaker("HGET") << key << field, boost::bind(stringCallBack, _1, done));
}

void RedisClient::hmget(const std::string& key, const std::vector<std::string>& fields, const ArrayCallBack& done) {
    RedisCommandMaker maker("HMGET");

    maker << key;

    for (std::size_t i = 0; i < fields.size(); ++i) {
        maker << fields[i];
    }

    command(maker, boost::bind(arrayCallBack, _1, done));
}

void RedisClient::hsetnx(const std::string& key, const std::string& field, const std::string& value, const StatusCallBack& done /*= DUMY_STATUS_CALL_BACK*/) {
    command(RedisCommandMaker("HSETNX") << key << field << value, boost::bind(integerCallBack, _1, done));
}

void RedisClient::hsetnx(const std::string& key, const std::string& field, const SimpleString& value, const StatusCallBack& done /*= DUMY_STATUS_CALL_BACK*/) {
    command(RedisCommandMaker("HSETNX") << key << field << value, boost::bind(integerCallBack, _1, done));
}

void RedisClient::setnx(const std::string& key, const std::string& value) {
    command(RedisCommandMaker("SETNX") << key << value, boost::bind(dummyCallBack, _1));
}

void RedisClient::setnx(const std::string& key, const SimpleString& value) {
    command(RedisCommandMaker("SETNX") << key << value, boost::bind(dummyCallBack, _1));
}

bool RedisClient::connected() const {
    std::map<std::string, RedisClientContext*>::const_iterator itr = clientContexts.begin();

    for (; itr != clientContexts.end(); ++itr) {
        if (itr->second->connected()) { return true; }
    }

    return false;
}

RedisClientContext::RedisClientContext(const std::string& host, int port)
    : host(host), port(port), channel(NULL), bootstrap(NULL) {
    // Configure the client.
    bootstrap = new ClientBootstrap(
        ChannelFactoryPtr(new AsioClientSocketChannelFactory()));

    bootstrap->setPipeline(Channels::pipeline(
                               ChannelHandlerPtr(new RedisResponseDecoder),
                               ChannelHandlerPtr(new RedisMessageHandler(boost::bind(&RedisClientContext::setDisconnected, this)))));
}

int RedisClientContext::connect() {
    // Start the connection attempt.
    ChannelFuturePtr future = bootstrap->connect(host, port);

    // Wait until the connection attempt succeeds or fails.
    channel = &(future->awaitUninterruptibly().getChannel());

    if (!future->isSuccess()) {
        printf("Exception happened, %s.", future->getCause()->what());
        //bootstrap->releaseExternalResources();
        channel = NULL;
        return -1;
    }

    messageHandler = boost::dynamic_pointer_cast<RedisMessageHandler>(channel->getPipeline().getLast());

    printf("connect to the remote server %s:%d\n", host.c_str(), port);
    return 0;
}

bool RedisClientContext::connected() const {
    return NULL != channel;
}

void RedisClientContext::write(const ChannelBufferPtr& buffer) {
    if (channel) {
        channel->write(buffer, false);
    }
}

void RedisClientContext::setDisconnected() {
    this->channel = NULL;
    this->messageHandler.reset();
}

void RedisClientContext::appendCallback(const RedisClient::CallBack& callback) {
    if (messageHandler) {
        if (!callback) {
            printf("append callback is empty.\n");
        }

        messageHandler->appendCallback(callback);
    }
}

void RedisClientContext::wait() {
    if (channel) {
        channel->getCloseFuture()->awaitUninterruptibly();
    }
}

const TimerPtr& RedisClientContext::getTimer() {
    if (channel) {
        return TimerFactory::getFactory().getTimer(*channel);
    }
    else {
        return TimerFactory::getFactory().getTimer(NullChannel::getInstance());
    }
}

}
}