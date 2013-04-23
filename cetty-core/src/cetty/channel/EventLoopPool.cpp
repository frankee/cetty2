
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

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {

EventLoopPool::EventLoops EventLoopPool::allEventLoops_;

static EventLoopPtr emptyEventLoop;

EventLoopPool::Iterator::Iterator(const HolderIterator& iter)
    : iter_(iter) {
}

EventLoopPool::Iterator::Iterator(const Iterator& iter)
    : iter_(iter.iter_) {
}

const EventLoopPtr& EventLoopPool::current() {
    const ThreadId& id = CurrentThread::id();
    EventLoops::iterator itr = allEventLoops_.find(id);

    if (itr != allEventLoops_.end()) {
        return itr->second;
    }

    return emptyEventLoop;
}

EventLoopPool::EventLoopPool(int ioThreadCount)
    : started_(false),
      singleThread_(0 == ioThreadCount),
      threadCnt_(ioThreadCount),
      eventLoopCnt_(singleThread_ ? 1 : ioThreadCount) {

    if (ioThreadCount < 0) {
        threadCnt_ = boost::thread::hardware_concurrency();
        eventLoopCnt_ = threadCnt_;
        LOG_WARN << "poolSize is negative, instead of the cpu number : "
                 << threadCnt_;
    }
    else if (0 == threadCnt_) {
        LOG_WARN << "onlyMainThread.";
    }

    mainThreadId_ = CurrentThread::id();
}

EventLoopPool::~EventLoopPool() {
}

void EventLoopPool::insertLoop(const ThreadId& id, const EventLoopPtr& loop) {
    allEventLoops_.insert(std::make_pair(id, loop));
}

}
}
