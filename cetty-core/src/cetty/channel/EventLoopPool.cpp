
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

#include <cetty/channel/EventLoopPool.h>
#include <cetty/channel/EventLoop.h>

namespace cetty {
namespace channel {

EventLoopPool::EventLoopMap EventLoopPool::allEventLoops;
static EventLoopPtr emptyEventLoop;

const EventLoopPtr& EventLoopPool::current() {
    boost::thread::id id = boost::this_thread::get_id();
    EventLoopMap::iterator itr = allEventLoops.find(id);
    if (itr != allEventLoops.end()) {
        return itr->second;
    }

    return emptyEventLoop;
}

EventLoopPool::EventLoopPool(int threadCnt)
    : started(false),
      mainThread(0 == threadCnt),
      threadCnt(threadCnt),
      eventLoopCnt(mainThread ? 1 : threadCnt) {

    if (threadCnt < 0) {
        threadCnt = boost::thread::hardware_concurrency();
        //LOG_WARN(logger, "poolSize is negative, instead of the cpu number : %d.", threadCnt);
    }
    else if (0 == threadCnt) {
        //LOG_WARN(logger, "onlyMainThread.");
    }

    mainThreadId = boost::this_thread::get_id();
}

EventLoopPool::~EventLoopPool() {
    EventLoops::iterator itr = eventLoops.begin();

    for (; itr != eventLoops.end(); ++itr) {
        EventLoopHolder* holder = *itr;

        if (holder) {
            delete holder;
        }
    }
}

}
}
