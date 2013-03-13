/*
 * BeanstalkClient.h
 *
 *  Created on: Mar 11, 2013
 *      Author: chenhl
 */

#ifndef BEANSTALKCLIENT_H_
#define BEANSTALKCLIENT_H_

#include <map>
#include <boost/bind.hpp>

#include <cetty/util/StringPiece.h>
#include <cetty/channel/ChannelPtr.h>
#include <cetty/beanstalk/BeanstalkServiceFuture.h>
#include <cetty/beanstalk/protocol/BeanstalkCommand.h>
#include <cetty/beanstalk/protocol/BeanstalkReply.h>
#include <cetty/beanstalk/protocol/commands/Producer.h>
#include <cetty/beanstalk/protocol/commands/Consumer.h>

namespace cetty {
namespace beanstalk {

using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::service;
using namespace cetty::beanstalk::protocol;

class BeanstalkClient {
public:
	BeanstalkClient(const ChannelPtr& channel)
        : channel_(channel) {}

    ~BeanstalkClient() {}

    typedef boost::function2<void,
        const BeanstalkServiceFuture&,
        const BeanstalkReplyPtr&> ReplyCallback;

    typedef boost::function2<void, std::string, const std::vector<StringPiece>&> MultiBulkCallBack;

public:
    void request(const BeanstalkCommandPtr& command, const BeanstalkServiceFuturePtr& future);

    void put(const std::string& data,
    		 int priority,
    		 int delay,
    		 int ttr,
    		 const PutCallBack callback);

    void setnx(const std::string& key, const StringPiece& value);

    void get(const std::string& key, const BulkCallBack& callback);

    template<typename Iterator>
    void get(const Iterator& keyBegin, const Iterator& keyEnd, const MultiBulkCallBack& callback) {
        RedisCommandPtr command =
            cetty::redis::protocol::commands::stringsCommandGet<Iterator>(keyBegin, keyEnd);

        RedisServiceFuturePtr future(new RedisServiceFuture(
            boost::bind(&RedisClient::multiBulkCallBack, _1, _2, callback)));

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


#endif /* BEANSTALKCLIENT_H_ */
