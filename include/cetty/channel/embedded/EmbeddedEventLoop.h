#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDEVENTLOOP_H)
#define CETTY_CHANNEL_EMBEDDED_EMBEDDEDEVENTLOOP_H
/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/channel/EventLoop.h>

namespace cetty {
namespace channel {
namespace embedded {

using namespace cetty::channel;

class EmbeddedEventLoop : public EventLoop {
public:
    EmbeddedEventLoop()
        : EventLoop(EventLoopPoolPtr()) {
    }

    virtual ~EmbeddedEventLoop() {}


    virtual void post(const Functor& handler) {}
    virtual TimeoutPtr runAt(const boost::posix_time::ptime& timestamp,
        const Functor& timerCallback) {}

    virtual TimeoutPtr runAfter(int millisecond, const Functor& timerCallback) {}
    virtual TimeoutPtr runEvery(int millisecond, const Functor& timerCallback) {}
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDEVENTLOOP_H)

// Local Variables:
// mode: c++
// End:
