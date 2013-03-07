#if !defined(CETTY_CHANNEL_EVENTLOOP_H)
#define CETTY_CHANNEL_EVENTLOOP_H

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

#include <boost/function.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <cetty/Types.h>
#include <cetty/channel/Timeout.h>
#include <cetty/channel/EventLoopPtr.h>
#include <cetty/channel/EventLoopPoolPtr.h>

#include <cetty/util/CurrentThread.h>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

class EventLoop : public cetty::util::ReferenceCounter<EventLoop, int> {
public:
    typedef boost::function0<void> Handler;

public:
    EventLoop(const EventLoopPoolPtr& pool);

    virtual ~EventLoop();

    /**
     * the the id of thread which the EventLoop in.
     */
    const ThreadId& threadId() const;

    /**
     * set the thread id when the EventLoop initializing.
     */
    void setThreadId(const ThreadId& id);

    /**
     * the EventLoopPool which the EventLoop in.
     */
    const EventLoopPoolPtr& eventLoopPool() const;

    /**
     * test the current context is int the thread which the EventLoop in.
     */
    bool inLoopThread() const;

    //virtual void start();

    /**
     * stop the EventLoop, no block
     * default clear the reference count of the EventLoopPoolPtr
     */
    virtual void stop();

    //virtual void dispatch(const Handler& handler) = 0;

    /**
     *
     */
    virtual void post(const Handler& handler) = 0;

    /**
     *
     */
    virtual TimeoutPtr runAt(const boost::posix_time::ptime& timestamp,
                             const Handler& handler) = 0;

    /**
     *
     *
     */
    virtual TimeoutPtr runAfter(int64_t millisecond,
                                const Handler& handler) = 0;
    
    /**
     *
     *
     */
    virtual TimeoutPtr runEvery(int64_t millisecond,
                                const Handler& handler) = 0;

private:
    ThreadId threadId_;
    EventLoopPoolPtr pool_;
};

inline
const ThreadId& EventLoop::threadId() const {
    return threadId_;
}

inline
void EventLoop::setThreadId(const ThreadId& id) {
    threadId_ = id;
}

inline
bool EventLoop::inLoopThread() const {
    return threadId_ == CurrentThread::id();
}

inline
const EventLoopPoolPtr& EventLoop::eventLoopPool() const {
    return pool_;
}

}
}

#endif //#if !defined(CETTY_CHANNEL_EVENTLOOP_H)

// Local Variables:
// mode: c++
// End:
