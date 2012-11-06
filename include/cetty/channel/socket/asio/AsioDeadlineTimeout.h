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

#include <cetty/channel/Timeout.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;

class AsioDeadlineTimeout : public cetty::channel::Timeout {
public:
    enum {
        TIMER_UNINITIALIZED  = 0,
        TIMER_CANCELLED      = 1,
        TIMER_EXPIRED        = 2,
        TIMER_ACTIVE         = 4,
    };
    
public:
    AsioDeadlineTimeout(boost::asio::io_service& ioService,
        const boost::posix_time::ptime& timestamp)
        : deadlineTimer(ioService, timestamp) {
    }

    AsioDeadlineTimeout(boost::asio::io_service& ioService,
        int64_t delay)
        : deadlineTimer(ioService, boost::posix_time::milliseconds(delay)) {
    }

    virtual ~AsioDeadlineTimeout() {

    }

    virtual bool isExpired() const { return state == TIMER_EXPIRED; }
    virtual bool isCancelled() const { return state == TIMER_CANCELLED; }
    virtual bool isActived() const { return state == TIMER_ACTIVE; }

    virtual void cancel() {
        boost::system::error_code code;
        deadlineTimer.cancel(code);
        if (code) {

        }
    }

    virtual boost::int64_t expiresFromNow() const {
        return deadlineTimer.expires_from_now().total_milliseconds();
    }

    boost::asio::deadline_timer& getTimer() {
        return deadlineTimer;
    }

    void setState(int state) {
        this->state = state;
    }
    
private:
    int state;
    boost::asio::deadline_timer deadlineTimer;
};

typedef boost::intrusive_ptr<AsioDeadlineTimeout> AsioDeadlineTimeoutPtr;

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIODEADLINETIMEOUT_H)

// Local Variables:
// mode: c++
// End:
