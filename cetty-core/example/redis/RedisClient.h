#if !defined(CETTY_REDIS_REDISCLIENT_H)
#define CETTY_REDIS_REDISCLIENT_H

/**
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
#include <boost/function.hpp>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/util/Timer.h>
#include <cetty/bootstrap/ClientBootstrap.h>
#include "RedisReplyMessage.h"
#include "RedisMessageHandler.h"

namespace cetty { namespace redis {

using namespace cetty::util;
using namespace cetty::buffer;
using namespace cetty::util;
using namespace cetty::bootstrap;

class RedisCommandMaker;
class RedisClientContext;

class RedisClient {
public:
    typedef boost::function<void (const RedisReplyMessagePtr&)> CallBack;
    typedef boost::function<void (int)> StatusCallBack;
    typedef boost::function<void (int, const SimpleString&)> StringCallBack;
    typedef boost::function<void (int, const std::vector<SimpleString>&)> ArrayCallBack;

    static const StatusCallBack DUMY_STATUS_CALL_BACK;

public:
    RedisClient() {}
    ~RedisClient() {}
    
public:
    int  addServer(const std::string& host, int port);
    void removeServer(const std::string& host, int port);

    bool connected() const;

public:
    void command(RedisCommandMaker& commandMaker, const CallBack& done);

    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, const SimpleString& value);
    
    void setnx(const std::string& key, const std::string& value);
    void setnx(const std::string& key, const SimpleString& value);

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

    RedisClientContext* getContext();

private:
    std::map<std::string, RedisClientContext*> clientContexts;
};

class RedisClientContext {
public:
    RedisClientContext(const std::string& host, int port);
    ~RedisClientContext() {}

    int connect();
    bool connected() const;

    void write(const ChannelBufferPtr& buffer);

    void setDisconnected();

    void appendCallback(const RedisClient::CallBack& callback);

    const TimerPtr& getTimer();

    void wait();

private:
    std::string host;
    int         port;
    Channel* channel;

    RedisMessageHandlerPtr messageHandler;

    ClientBootstrap* bootstrap;
};

}}

#endif //#if !defined(CETTY_REDIS_REDISCLIENT_H)