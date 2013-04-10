#if !defined(CETTY_BEANSTALK_BEANSTALKCLIENT_H)
#define CETTY_BEANSTALK_BEANSTALKCLIENT_H

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
/*
 *      Author: chenhl
 */

#include <map>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

#include <cetty/util/StringPiece.h>
#include <cetty/channel/ChannelPtr.h>
#include <cetty/beanstalk/BeanstalkServiceFuture.h>
#include <cetty/beanstalk/protocol/BeanstalkReply.h>
#include <cetty/beanstalk/protocol/BeanstalkCommand.h>

namespace cetty {
namespace beanstalk {

using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::service;
using namespace cetty::beanstalk::protocol;

class BeanstalkClient : private boost::noncopyable {
public:
    typedef boost::function<void (const BeanstalkReplyPtr&)> ReplyCallback;

public:
    BeanstalkClient(const ChannelPtr& channel)
        : channel_(channel) {}

    ~BeanstalkClient() {}

public:
    void request(const BeanstalkCommandPtr& command,
                 const BeanstalkServiceFuturePtr& future);

    void put(const std::string& data, const ReplyCallback& callback);

    void put(const std::string& data,
             int priority,
             int delay,
             int ttr,
             const ReplyCallback& callback);

    void use(const std::string& tubeName,
             const ReplyCallback& callback);

    void reserve(const ReplyCallback& callback);
    void reserve(int timeout, const ReplyCallback& callback);

    void del(int id, const ReplyCallback& callback);

    void release(int id, const ReplyCallback& callback);

    void release(int id,
                 int priority,
                 int delay,
                 const ReplyCallback& callback);

    void bury(int id, const ReplyCallback& callback);
    void bury(int id, int priority, const ReplyCallback& callback);

    void touch(int id, const ReplyCallback& callback);

    void watch(const std::string& tube, const ReplyCallback& callback);
    void ignore(const std::string& tube, const ReplyCallback& callback);

    void peek(int id, const ReplyCallback& callback);
    void peekReady(const ReplyCallback& callback);
    void peekDelayed(const ReplyCallback& callback);
    void peekBuried(const ReplyCallback& callback);

    void kick(int bound, const ReplyCallback& callback);
    void kickJob(int id, const ReplyCallback& callback);

    void statsJob(int id, const ReplyCallback& callback);
    void statsTube(const std::string& tube, const ReplyCallback& callback);
    void stats(const ReplyCallback& callback);

    void listTubes(const ReplyCallback& callback);
    void listUsedTube(const ReplyCallback& callback);
    void listWatchedTubes(const ReplyCallback& callback);

    void pauseTube(const std::string& tube,
                   const ReplyCallback& callback);

    void pauseTube(const std::string& tube,
                   int delay,
                   const ReplyCallback& callback);

    void quit();

private:
    static void internalCallBack(const BeanstalkServiceFuture& future,
                                 const BeanstalkReplyPtr& reply,
                                 const ReplyCallback& callback);

private:
    ChannelPtr channel_;
};

}
}

#endif //#if !defined(CETTY_BEANSTALK_BEANSTALKCLIENT_H)

// Local Variables:
// mode: c++
// End:
