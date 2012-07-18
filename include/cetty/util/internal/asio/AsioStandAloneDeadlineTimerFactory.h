#if !defined(CETTY_UTIL_INTERNAL_ASIO_ASIOSTANDALONEDEADLINETIMERFACTORY_H)
#define CETTY_UTIL_INTERNAL_ASIO_ASIOSTANDALONEDEADLINETIMERFACTORY_H

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

#include <vector>
#include <cetty/util/TimerFactory.h>
#include <cetty/util/internal/asio/AsioDeadlineTimer.h>
#include <cetty/channel/socket/asio/AsioServicePoolFwd.h>

namespace cetty {
namespace util {
namespace internal {
namespace asio {

using namespace cetty::util;
using namespace cetty::channel::socket::asio;

class AsioStandAloneDeadlineTimerFactory : public cetty::util::TimerFactory {
public:
    AsioStandAloneDeadlineTimerFactory(AsioServicePtr& service) : service(service) {
        if (!service) {
            throw InvalidArgumentException("the service is empty.");
        }

        timer = TimerPtr(new AsioDeadlineTimer(service));
    }

    virtual ~AsioStandAloneDeadlineTimerFactory() {}

    virtual const TimerPtr& getTimer(const cetty::channel::ChannelPtr& channel) {
        return timer;
    }

    virtual void stopTimers() {
        if (timer) {
            timer->stop();
        }
    }

private:
    AsioServicePtr service;
    TimerPtr timer;
};

}
}
}
}

#endif //#if !defined(CETTY_UTIL_INTERNAL_ASIO_ASIOSTANDALONEDEADLINETIMERFACTORY_H)

// Local Variables:
// mode: c++
// End:
