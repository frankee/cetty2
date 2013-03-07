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
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <cetty/channel/EventLoopPtr.h>
#include <cetty/channel/EventLoopPoolPtr.h>
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
        virtual const EventLoopPtr& eventLoop() const = 0;
    };

    typedef std::map<ThreadId, EventLoopPtr> EventLoops;
    typedef boost::ptr_vector<EventLoopHolder> EventLoopHolders;

    class Iterator {
    public:
        typedef EventLoopHolders::iterator HolderIterator;

    public:
        Iterator(const HolderIterator& iter);
        Iterator(const Iterator& iter);

        Iterator& operator++();

        const EventLoopPtr& operator*() const;

        bool operator==(const Iterator& iter) const;
        bool operator!=(const Iterator& iter) const;

    private:
        HolderIterator iter_;
    };
    
public:
    static const EventLoopPtr& current();

public:
    EventLoopPool(int ioThreadCount);
    virtual ~EventLoopPool();

    bool empty() const;
    bool isStarted() const;
    bool isSingleThread() const;
    
    int size() const;

    Iterator begin();
    Iterator end();

    /**
     *
     */
    virtual const EventLoopPtr& nextLoop() = 0;

    /**
     *
     */
    virtual bool start() = 0;

    /**
     * Stop all eventLoop in the pool.
     */
    virtual void stop() = 0;

    /**
     *
     */
    virtual void waitingForStop() = 0;

protected:
    EventLoopHolder* loopHolderAt(int index);
    void appendLoopHolder(EventLoopHolder* holder);

    void setStarted(bool started);

    void insertLoop(const ThreadId& id, const EventLoopPtr& loop);

private:
    static EventLoops allEventLoops_;

private:
    bool started_;
    bool singleThread_; //< indicated this pool is only use the program's main thread.

    int threadCnt_;
    int eventLoopCnt_;

    //
    ThreadId mainThreadId_;

    EventLoopHolders eventLoops_;
};

inline
EventLoopPool::Iterator& EventLoopPool::Iterator::operator++() {
    ++iter_;
    return *this;
}

inline
const EventLoopPtr& EventLoopPool::Iterator::operator*() const {
    return iter_->eventLoop();
}

inline
bool EventLoopPool::Iterator::operator==(const Iterator& iter) const {
    return this->iter_ == iter.iter_;
}

inline
bool EventLoopPool::Iterator::operator!=(const Iterator& iter) const {
    return this->iter_ != iter.iter_;
}

inline
bool EventLoopPool::isStarted() const {
    return started_;
}

inline
bool EventLoopPool::isSingleThread() const {
    return singleThread_;
}

inline
bool EventLoopPool::empty() const {
    return 0 == eventLoopCnt_;
}

inline
int EventLoopPool::size() const {
    return eventLoopCnt_;
}

inline
EventLoopPool::Iterator EventLoopPool::begin() {
    return Iterator(eventLoops_.begin());
}

inline
EventLoopPool::Iterator EventLoopPool::end() {
    return Iterator(eventLoops_.end());
}

inline
EventLoopPool::EventLoopHolder* EventLoopPool::loopHolderAt(int index) {
    return &eventLoops_.at(index);
}

inline
void EventLoopPool::appendLoopHolder(EventLoopPool::EventLoopHolder* holder) {
    eventLoops_.push_back(holder);
}

inline
void EventLoopPool::setStarted(bool started) {
    started_ = started;
}

inline
void EventLoopPool::insertLoop(const ThreadId& id, const EventLoopPtr& loop) {
    allEventLoops_.insert(std::make_pair(id, loop));
}

}
}

#endif //#if !defined(CETTY_CHANNEL_EVENTLOOPPOOL_H)

// Local Variables:
// mode: c++
// End:
