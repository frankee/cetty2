
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
 * BeanstalkClient.cpp
 *
 *  Created on: Mar 13, 2013
 *      Author: chenhl
 */

#include <cetty/logging/LoggerHelper.h>
#include <cetty/service/ClientService.h>
#include <cetty/beanstalk/BeanstalkClient.h>
#include <cetty/beanstalk/protocol/commands/Producer.h>
#include <cetty/beanstalk/protocol/commands/Consumer.h>

namespace cetty {
namespace beanstalk {

using namespace cetty::beanstalk::protocol;

void BeanstalkClient::request(const BeanstalkCommandPtr& command,
                              const BeanstalkServiceFuturePtr& future) {
    ::cetty::service::callMethod(channel_, command, future);
}

void BeanstalkClient::put(const std::string& data, const ReplyCallback& callback) {
    put(data, commands::DEFAULT_PRIORITY, 0, commands::DEFAULT_TTR, callback);
}

void BeanstalkClient::put(const std::string& data,
                          int priority,
                          int delay,
                          int ttr,
                          const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::put(data, priority, delay, ttr), future);
}

void BeanstalkClient::use(const std::string& tubeName,
                          const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));
    request(protocol::commands::use(tubeName), future);
}

void BeanstalkClient::reserve(const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));
    request(protocol::commands::reserve(), future);
}

void BeanstalkClient::reserve(int timeout, const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));
    request(protocol::commands::reserve(timeout), future);
}

void BeanstalkClient::del(int id, const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));
    request(protocol::commands::del(id), future);

}

void BeanstalkClient::release(int id,
                              int priority,
                              int delay,
                              const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));
    request(protocol::commands::release(id, priority, delay), future);
}

void BeanstalkClient::release(int id, const ReplyCallback& callback) {
    release(id, commands::DEFAULT_PRIORITY, 0, callback);
}

void BeanstalkClient::bury(int id,
                           int priority,
                           const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));
    request(protocol::commands::bury(id, priority), future);
}

void BeanstalkClient::bury(int id, const ReplyCallback& callback) {
    bury(id, commands::DEFAULT_PRIORITY, callback);
}

void BeanstalkClient::touch(int id, const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));
    request(protocol::commands::touch(id), future);
}

void BeanstalkClient::watch(const std::string& tube,
                            const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::watch(tube), future);
}

void BeanstalkClient::ignore(const std::string& tube,
                             const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));
    request(protocol::commands::ignore(tube), future);
}

void BeanstalkClient::peek(int id, const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));
    request(protocol::commands::peek(id), future);
}

void BeanstalkClient::peekReady(const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::peekReady(), future);
}

void BeanstalkClient::peekDelayed(const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::peekDelayed(), future);
}

void BeanstalkClient::peekBuried(const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::peekBuried(), future);
}

void BeanstalkClient::kick(int bound, const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::kick(bound), future);
}

void BeanstalkClient::kickJob(int id, const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::kickJob(id), future);
}

void BeanstalkClient::statsJob(int id, const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::statsJob(id), future);
}

void BeanstalkClient::statsTube(const std::string& tube,
                                const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::statsTube(tube), future);
}

void BeanstalkClient::stats(const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::stats(), future);
}

void BeanstalkClient::listTubes(const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::listTubes(), future);
}

void BeanstalkClient::listUsedTube(const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::internalCallBack,
                                   _1,
                                   _2,
                                   callback)));

    request(protocol::commands::listTubeUsed(), future);
}

void BeanstalkClient::listWatchedTubes(const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::listTubesWatched(), future);
}

void BeanstalkClient::pauseTube(const std::string& tube,
                                int delay,
                                const ReplyCallback& callback) {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       callback)));

    request(protocol::commands::pauseTube(tube, delay), future);
}

void BeanstalkClient::pauseTube(const std::string& tube,
                                const ReplyCallback& callback) {
    pauseTube(tube, 0, callback);
}

void BeanstalkClient::quit() {
    BeanstalkServiceFuturePtr future(
        new BeanstalkServiceFuture(boost::bind(
                                       &BeanstalkClient::internalCallBack,
                                       _1,
                                       _2,
                                       ReplyCallback())));
    request(protocol::commands::quit(), future);
}

void BeanstalkClient::internalCallBack(const BeanstalkServiceFuture& future,
                                       const BeanstalkReplyPtr& reply,
                                       const ReplyCallback& callback) {
    if (callback) {
        callback(reply);
    }
    else {
        if (reply) {
            LOG_INFO << reply->toString();
        }
    }
}

}
}
