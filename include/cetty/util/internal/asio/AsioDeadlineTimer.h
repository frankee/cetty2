#if !defined(CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMER_H)
#define CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMER_H

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

#include <list>

#include <cetty/util/Timer.h>
#include <cetty/channel/socket/asio/AsioServicePoolFwd.h>

namespace cetty {
namespace util {
namespace internal {
namespace asio {

using namespace cetty::util;
using cetty::channel::socket::asio::AsioServicePtr;

class AsioDeadlineTimer : public cetty::util::Timer {
public:
    AsioDeadlineTimer(const AsioServicePtr& ioService);
    virtual ~AsioDeadlineTimer();

    virtual const TimeoutPtr& newTimeout(const TimerTask& task, boost::int64_t delay);
    virtual const TimeoutPtr& newTimeout(const TimerTask& task, boost::int64_t delay, const TimeUnit& unit);

    virtual void stop();

private:
    void clearInactivedTimeout();

private:
    typedef std::list<TimeoutPtr> TimeoutList;

private:
    AsioServicePtr ioService;
    TimeoutList timeouts;
};

}
}
}
}


#endif //#if !defined(CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMER_H)

// Local Variables:
// mode: c++
// End:

