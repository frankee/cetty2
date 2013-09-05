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

#include <cetty/util/CurrentThread.h>
#include <cetty/logging/LoggerHelper.h>

#include <cetty/channel/Timeout.h>
#include <cetty/channel/EventLoop.h>
#include <cetty/channel/EventLoopPool.h>

namespace cetty {
namespace channel {
namespace embedded {

using namespace cetty::channel;

class EmbeddedEventLoop : public EventLoop {
public:
    EmbeddedEventLoop()
        : EventLoop(EventLoopPoolPtr()) {
        setThreadId(CurrentThread::id());
    }

    virtual ~EmbeddedEventLoop() {}

    virtual const std::string& type() const {
        static const std::string TYPE("embedded");
        return TYPE;
    }

    virtual void post(const Handler& handler) {
        if (handler) {
            handler();
        }
    }

    virtual TimeoutPtr runAt(const boost::posix_time::ptime& timestamp,
                             const Handler& handler) {
        LOG_WARN << "EmbeddedEventLoop does not support runAt";
        return TimeoutPtr();
    }

    virtual TimeoutPtr runAfter(int64_t millisecond, const Handler& handler) {
        LOG_WARN << "EmbeddedEventLoop does not support runAfter";
        return TimeoutPtr();
    }

    virtual TimeoutPtr runEvery(int64_t millisecond, const Handler& handler) {
        LOG_WARN << "EmbeddedEventLoop does not support runEvery";
        return TimeoutPtr();
    }
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDEVENTLOOP_H)

// Local Variables:
// mode: c++
// End:
