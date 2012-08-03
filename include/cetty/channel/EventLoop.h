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
#include <cetty/util/ReferenceCounter.h>
#include <cetty/channel/EventLoopPtr.h>
#include <cetty/channel/EventLoopPoolPtr.h>

namespace cetty {
namespace channel {

class EventLoop : public cetty::util::ReferenceCounter<EventLoop, int> {
public:
    typedef boost::function0<void> Functor;

public:
    EventLoop(const EventLoopPoolPtr& pool);

    virtual ~EventLoop();

    const boost::thread::id& getThreadId() const;
    void setThreadId(const boost::thread::id& id);

    const EventLoopPoolPtr& getEventLoopPool() const { return pool; }

    bool inLoopThread() const;

    virtual void post(const Functor& handler) = 0;

private:
    EventLoop(const EventLoop&);
    EventLoop& operator=(const EventLoop&);

private:
    boost::thread::id threadId;
    EventLoopPoolPtr pool;
};

inline
const boost::thread::id& EventLoop::getThreadId() const {
    return threadId;
}

inline
void EventLoop::setThreadId(const boost::thread::id& id) {
    threadId = id;
}

inline
bool EventLoop::inLoopThread() const {
    return threadId == boost::this_thread::get_id();
}
}
}

#endif //#if !defined(CETTY_CHANNEL_EVENTLOOP_H)

// Local Variables:
// mode: c++
// End: