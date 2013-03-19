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
        : channel(channel) {}

    ~BeanstalkClient() {}

    typedef boost::function2<void,
    		                 const BeanstalkServiceFuture&,
                             const BeanstalkReplyPtr&> ReplyCallback;

    typedef boost::function1<void, std::string> CommandCallBack;
    typedef boost::function2<void, std::string, int> IdCallBack;
    typedef boost::function2<void, std::string, std::string> DataCallBack;
    typedef boost::function3<void, std::string, int, std::string> IdDataCallBack;

public:
    void request(const BeanstalkCommandPtr& command,
    		     const BeanstalkServiceFuturePtr& future);

    void put(const std::string& data,
    		 int priority,
    		 int delay,
    		 int ttr,
    		 const IdCallBack &callback);

    void use(const std::string &tubeName,
    		 const DataCallBack &callback);

    void reserve(const IdDataCallBack &callback);
    void reserve(int timeout, const IdDataCallBack &callback);

    void del(int id, const CommandCallBack &callback);

    void release(int id,
    		     int priority,
    		     int delay,
    		     const CommandCallBack &callback);

    void bury(int id, int priority, const CommandCallBack &callback);

    void touch(int id, const CommandCallBack &callback);

    void watch(const std::string &tube, const IdCallBack &callback);
    void ignore(const std::string &tube, const IdCallBack &callback);

    void peek(int id, const IdDataCallBack &callback);
    void peekReady(const IdDataCallBack &callback);
    void peekDelayed(const IdDataCallBack &callback);
    void peekBuried(const IdDataCallBack &callback);

    void kick(int bound, const IdCallBack &callback);
    void kickJob(int id, const CommandCallBack &callback);

    void statsJob(int id, const DataCallBack &callback);
    void statsTube(const std::string &tube, const DataCallBack &callback);
    void stats(const DataCallBack &callback);

    void listTubes(const DataCallBack &callback);
    void listTubeUsed(const DataCallBack &callback);
    void listTubesWatched(const DataCallBack &callback);

    void pauseTube(const std::string &tube,
    		       int delay,
    		       const CommandCallBack& callback);

    void quit();

private:
    static void commandCallBack(const BeanstalkServiceFuture& future,
                                const BeanstalkReplyPtr& reply,
                                const CommandCallBack& callback);

    static void idCallBack(const BeanstalkServiceFuture& future,
                           const BeanstalkReplyPtr& reply,
                           const IdCallBack& callback);

    static void countCallBack(const BeanstalkServiceFuture& future,
                              const BeanstalkReplyPtr& reply,
                              const IdCallBack& callback);

    static void dataCallBack(const BeanstalkServiceFuture& future,
                             const BeanstalkReplyPtr& reply,
                             const DataCallBack& callback);

    static void idDataCallBack(const BeanstalkServiceFuture& future,
                               const BeanstalkReplyPtr& reply,
                               const IdDataCallBack& callback);

private:
    ChannelPtr channel;
};

}
}


#endif /* BEANSTALKCLIENT_H_ */
