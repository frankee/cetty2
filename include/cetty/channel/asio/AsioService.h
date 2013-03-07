#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVICE_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVICE_H

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

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/placeholders.hpp>

#include <cetty/channel/EventLoop.h>
#include <cetty/channel/EventLoopPoolPtr.h>
#include <cetty/channel/asio/AsioServicePtr.h>
#include <cetty/channel/asio/AsioServicePoolPtr.h>
#include <cetty/channel/asio/AsioDeadlineTimeout.h>

namespace cetty {
namespace channel {
namespace asio {

/**
 *
 *
 */
class AsioService : public cetty::channel::EventLoop {
public:
    AsioService(const EventLoopPoolPtr& pool)
        : EventLoop(pool) {
    }

    boost::asio::io_service& service();

    virtual void stop();

    virtual void post(const Handler& handler);

    virtual TimeoutPtr runAt(const boost::posix_time::ptime& timestamp,
                             const Handler& handler);

    virtual TimeoutPtr runAfter(int64_t millisecond,
                                const Handler& handler);

    virtual TimeoutPtr runEvery(int64_t millisecond,
                                const Handler& handler);

private:
    void timerExpiresCallback(const boost::system::error_code& code,
                              const Handler& handler,
                              const AsioDeadlineTimeoutPtr& timeout);

    void repeatTimerExpiresCallback(const boost::system::error_code& code,
                                    const Handler& handler,
                                    int64_t millisecond,
                                    const AsioDeadlineTimeoutPtr& timeout);

private:
    boost::asio::io_service ioService_;
    std::list<AsioDeadlineTimeoutPtr> timers_;
};

inline
boost::asio::io_service& AsioService::service() {
    return ioService_;
}

}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVICE_H)

// Local Variables:
// mode: c++
// End:
