
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

EventLoopPool::EventLoopMap EventLoopPool::allEventLoops_;
static EventLoopPtr emptyEventLoop;

const EventLoopPtr& EventLoopPool::current() {
    const ThreadId& id = CurrentThread::id();
    EventLoopMap::iterator itr = allEventLoops_.find(id);
    if (itr != allEventLoops_.end()) {
        return itr->second;
    }

    return emptyEventLoop;
}

EventLoopPool::EventLoopPool(int threadCnt)
    : started_(false),
      mainThread_(0 == threadCnt),
      threadCnt_(threadCnt),
      eventLoopCnt_(mainThread_ ? 1 : threadCnt) {

    if (threadCnt < 0) {
        threadCnt_ = boost::thread::hardware_concurrency();
        eventLoopCnt_ = threadCnt_;
        //LOG_WARN(logger, "poolSize is negative, instead of the cpu number : %d.", threadCnt);
    }
    else if (0 == threadCnt) {
        //LOG_WARN(logger, "onlyMainThread.");
    }

    mainThreadId_ = CurrentThread::id();
}

EventLoopPool::~EventLoopPool() {
    EventLoops::iterator itr = eventLoops_.begin();

    for (; itr != eventLoops_.end(); ++itr) {
        EventLoopHolder* holder = *itr;

        if (holder) {
            delete holder;
        }
    }
}

}
}
