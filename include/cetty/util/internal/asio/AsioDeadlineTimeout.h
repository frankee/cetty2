#if !defined(CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMEOUT_H)
#define CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMEOUT_H

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
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/placeholders.hpp>

#include <cetty/channel/socket/asio/AsioServicePtr.h>
#include <cetty/util/Timeout.h>
#include <cetty/util/TimerTask.h>
#include <cetty/util/TimeUnit.h>

namespace cetty {
namespace util {
namespace internal {
namespace asio {

using namespace cetty::util;
using namespace cetty::channel::socket::asio;

class AsioDeadlineTimer;

class AsioDeadlineTimeout : public cetty::util::Timeout {
public:
    AsioDeadlineTimeout(const TimerPtr& timer,
                        const AsioServicePtr& ioService,
                        const TimerTask& task,
                        boost::int64_t delay);

    virtual ~AsioDeadlineTimeout();

    virtual const TimerPtr& getTimer() const;

    virtual bool isExpired() const;
    virtual bool isCancelled() const;
    virtual bool isActived() const;

    virtual void cancel();
    virtual int expiresFromNow() const;

private:
    void timerExpiresCallback(const boost::system::error_code& error, const TimerTask& task);

private:
    enum {
        TIMER_UNINITIALIZED  = 0,
        TIMER_CANCELLED      = 1,
        TIMER_EXPIRED        = 2,
        TIMER_ACTIVE         = 4,
    };

private:
    int state;
    TimerPtr timer;

    boost::asio::deadline_timer deadlineTimer;
};

}
}
}
}

#endif //#if !defined(CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMEOUT_H)

// Local Variables:
// mode: c++
// End:
