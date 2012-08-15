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
#include <cetty/channel/socket/asio/AsioService.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioSocketChannel.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace util {
namespace internal {
namespace asio {

using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::logging;
using namespace cetty::channel::socket::asio;

AsioDeadlineTimerFactory::AsioDeadlineTimerFactory(const EventLoopPtr& eventLoop) {
    BOOST_ASSERT(eventLoop && "Initialized service CAN NOT BE NULL.");

    if (eventLoop) {
        timers[eventLoop->getThreadId()] = new AsioDeadlineTimer(eventLoop);
    }
}

AsioDeadlineTimerFactory::AsioDeadlineTimerFactory(const EventLoopPoolPtr& pool) {
    BOOST_ASSERT(pool && "Initialized pool CAN NOT BE NULL.");

    if (pool) {
        initWithPool(pool);
    }
}

AsioDeadlineTimerFactory::AsioDeadlineTimerFactory(const EventLoopPoolPtr& parentPool,
        const EventLoopPoolPtr& childPool) {
    BOOST_ASSERT(parentPool && childPool && "Initialized pool CAN NOT BE NULL.");

    if (parentPool) {
        initWithPool(parentPool);
    }

    if (childPool && childPool != parentPool) {
        initWithPool(childPool);
    }
}

AsioDeadlineTimerFactory::~AsioDeadlineTimerFactory() {
    stopTimers();

    TimerMap::iterator itr;

    for (itr = timers.begin(); itr != timers.end(); ++itr) {
        const TimerPtr& ptr = itr->second;
        BOOST_ASSERT(ptr);
        delete ptr;
    }

    timers.clear();
}

const TimerPtr& AsioDeadlineTimerFactory::getTimer(const boost::thread::id& id) {
    if (timers.empty()) {
        BOOST_ASSERT(false && "timers has not initialized.");

        CETTY_NDC_SCOPE("AsioDeadlineTimerFactory::getTimer");
        LOG_ERROR() << "AsioDeadlineTimerFactory using before initialized.";
        throw RuntimeException("AsioDeadlineTimerFactory using before initialized.");
    }

    return timers[id];
}

void AsioDeadlineTimerFactory::stopTimers() {
    TimerMap::iterator itr = timers.begin();
    TimerMap::iterator endItr = timers.end();

    for (; itr != endItr; ++itr) {
        itr->second->stop();
    }
}

void AsioDeadlineTimerFactory::initWithPool(const EventLoopPoolPtr& pool) {
    EventLoopPool::Iterator itr = pool->begin();
    EventLoopPool::Iterator end = pool->end();

    for (; itr != end; ++itr) {
        const EventLoopPtr& eventLoop = *itr;
        timers[eventLoop->getThreadId()] = new AsioDeadlineTimer(eventLoop);
    }
}

}
}
}
}
