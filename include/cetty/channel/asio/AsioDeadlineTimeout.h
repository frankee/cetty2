#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIODEADLINETIMEOUT_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIODEADLINETIMEOUT_H

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/placeholders.hpp>

#include <cetty/util/CurrentThread.h>
#include <cetty/channel/Timeout.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace cetty::util;
using namespace cetty::channel;

class AsioService;

class AsioDeadlineTimeout : public cetty::channel::Timeout {
public:
    enum {
        TIMER_UNINITIALIZED  = 0,
        TIMER_CANCELLED      = 1,
        TIMER_EXPIRED        = 2,
        TIMER_ACTIVE         = 4,
    };

public:
    AsioDeadlineTimeout(AsioService& service,
                        int id,
                        const boost::posix_time::ptime& timestamp);

    AsioDeadlineTimeout(AsioService& service,
                        int id,
                        int64_t delay);

    virtual ~AsioDeadlineTimeout();

    virtual bool isExpired() const;
    virtual bool isCancelled() const;
    virtual bool isActived() const;

    virtual void cancel();

    virtual boost::int64_t expiresFromNow() const;

    boost::asio::deadline_timer& timer();

    int id() const;

    int state() const;
    void setState(int state);

    const ThreadId& threadId() const;

private:
    int id_;
    int state_;
    ThreadId threadId_;

    boost::asio::deadline_timer deadlineTimer_;
};

typedef boost::intrusive_ptr<AsioDeadlineTimeout> AsioDeadlineTimeoutPtr;

inline
boost::asio::deadline_timer& AsioDeadlineTimeout::timer() {
    return deadlineTimer_;
}

inline
int AsioDeadlineTimeout::id() const {
    return id_;
}

inline
    int AsioDeadlineTimeout::state() const {
        return state_;
}

inline
void AsioDeadlineTimeout::setState(int state) {
    state_ = state;
}

inline
const ThreadId& AsioDeadlineTimeout::threadId() const {
    return threadId_;
}

}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIODEADLINETIMEOUT_H)

// Local Variables:
// mode: c++
// End:
