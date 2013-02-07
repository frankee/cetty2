#if !defined(CETTY_CHANNEL_EVENTLOOPPOOL_H)
#define CETTY_CHANNEL_EVENTLOOPPOOL_H

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

#include <vector>
#include <boost/thread.hpp>
#include <cetty/channel/EventLoopPtr.h>
#include <cetty/util/CurrentThread.h>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace channel {

    using namespace cetty::util;

class EventLoopPool : public cetty::util::ReferenceCounter<EventLoopPool> {
public:
    class EventLoopHolder {
    public:
        virtual ~EventLoopHolder() {}
        virtual const EventLoopPtr& getEventLoop() const = 0;
    };

    typedef std::vector<EventLoopHolder*> EventLoops;
    typedef std::map<ThreadId, EventLoopPtr> EventLoopMap;

    static const EventLoopPtr& current();

public:
    class Iterator {
    public:
        typedef EventLoops::iterator EventLoopIterator;

    public:
        Iterator(const EventLoopIterator& iter) : iter(iter) {}
        Iterator(const Iterator& iter) : iter(iter.iter) {}

        Iterator& operator++() { ++iter; return *this; }

        const EventLoopPtr& operator*() { return (*iter)->getEventLoop(); }
        bool operator==(const Iterator& iter) const { return this->iter == iter.iter; }
        bool operator!=(const Iterator& iter) const { return this->iter != iter.iter; }

    private:
        EventLoopIterator iter;
    };

public:
    EventLoopPool(int threadCnt);
    virtual ~EventLoopPool();

    bool isStarted() const { return started_; }
    bool isMainThread() const { return mainThread_; }
    bool empty() const { return 0 == eventLoopCnt_; }
    
    int getEventLoopCnt() const { return eventLoopCnt_; }
    Iterator begin() { return Iterator(eventLoops_.begin()); }
    Iterator end() { return Iterator(eventLoops_.end()); }

    virtual bool start() = 0;
    
    /**
     *
     */
    virtual void waitForStop() = 0;

    /**
     * Stop all eventLoop in the pool.
     */
    virtual void stop() = 0;
    
    virtual const EventLoopPtr& getNextLoop() = 0;

protected:
    bool started_;
    bool mainThread_; //< indicated this pool is only use the program's main thread.
    
    int threadCnt_;
    int eventLoopCnt_;

    // The next io_service to use for a connection.
    int nextServiceIndex_;

    //
    ThreadId mainThreadId_;

    EventLoops eventLoops_;

protected:
    static EventLoopMap allEventLoops_;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_EVENTLOOPPOOL_H)

// Local Variables:
// mode: c++
// End:
