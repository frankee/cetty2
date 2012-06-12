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

#include <cetty/util/internal/asio/AsioDeadlineTimerFactory.h>

#include <boost/assert.hpp>
#include <cetty/util/Timer.h>
#include <cetty/util/Exception.h>
#include <cetty/util/NestedDiagnosticContext.h>
#include <cetty/util/internal/asio/AsioDeadlineTimer.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioSocketChannel.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/logging/InternalLoggerFactory.h>


namespace cetty {
namespace util {
namespace internal {
namespace asio {

using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::logging;
using namespace cetty::channel::socket::asio;

InternalLogger* AsioDeadlineTimerFactory::logger = NULL;

AsioDeadlineTimerFactory::AsioDeadlineTimerFactory(const AsioServicePtr& service) {
    BOOST_ASSERT(service && "Initialized service CAN NOT BE NULL.");

    if (service) {
        timers[service->getId()] = new AsioDeadlineTimer(service);
    }

    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("AsioDeadlineTimerFactory");
    }
}

AsioDeadlineTimerFactory::AsioDeadlineTimerFactory(const AsioServicePoolPtr& pool) {
    BOOST_ASSERT(pool && "Initialized pool CAN NOT BE NULL.");

    if (pool) {
        AsioServicePool::Iterator itr = pool->begin();
        AsioServicePool::Iterator end = pool->end();

        for (; itr != end; ++itr) {
            const AsioServicePtr& service = *itr;
            timers[service->getId()] = new AsioDeadlineTimer(service);
        }
    }

    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("AsioDeadlineTimerFactory");
    }
}

AsioDeadlineTimerFactory::~AsioDeadlineTimerFactory() {
    stopTimers();

    std::map<int, TimerPtr>::iterator itr;

    for (itr = timers.begin(); itr != timers.end(); ++itr) {
        const TimerPtr& ptr = itr->second;
        BOOST_ASSERT(ptr);
        delete ptr;
    }

    timers.clear();
}

const TimerPtr& AsioDeadlineTimerFactory::getTimer(const ChannelPtr& channel) {
    if (timers.empty()) {
        BOOST_ASSERT(false && "timers has not initialized.");

        CETTY_NDC_SCOPE("AsioDeadlineTimerFactory::getTimer");
        LOG_ERROR(logger, "AsioDeadlineTimerFactory using before initialized.");
        throw RuntimeException("AsioDeadlineTimerFactory using before initialized.");
    }

    // if the channel ptr is null, using the standalone timer.
    if (!channel) {
        LOG_WARN(logger, "the input channel is NULL, return the default timer, you should make sure the thread safe.");
        return timers.begin()->second;
    }

    AsioSocketChannel* socketChannel =
        dynamic_cast<AsioSocketChannel*>(channel);

    if (socketChannel) {
        return timers[socketChannel->getService()->getId()];
    }

    // TODO: CHECK UDP HERE
    //AsioDatagramChannel* datagramChannel =
    //    dynamic_cast<AsioDatagramChannel*>(channel.get());
    //if (datagramChannel) {
    //}

    LOG_WARN(logger, "the input channel is invalid, return the default timer, you should make sure the thread safe.");
    return timers.begin()->second;
}

void AsioDeadlineTimerFactory::stopTimers() {
    std::map<int, TimerPtr>::iterator itr = timers.begin();
    std::map<int, TimerPtr>::iterator endItr = timers.end();

    for (; itr != endItr; ++itr) {
        itr->second->stop();
    }
}

}
}
}
}