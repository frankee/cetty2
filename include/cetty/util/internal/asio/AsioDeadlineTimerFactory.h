#if !defined(CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMERFACTORY_H)
#define CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMERFACTORY_H

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
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <map>
#include <boost/thread/thread.hpp>
#include <cetty/util/TimerFactory.h>
#include <cetty/channel/EventLoopPoolPtr.h>

namespace cetty {
namespace util {
namespace internal {
namespace asio {

using namespace cetty::util;

class AsioDeadlineTimerFactory : public cetty::util::TimerFactory {
public:
    AsioDeadlineTimerFactory(const EventLoopPtr& eventLoop);
    AsioDeadlineTimerFactory(const EventLoopPoolPtr& pool);
    AsioDeadlineTimerFactory(const EventLoopPoolPtr& parentPool,
                             const EventLoopPoolPtr& childPool);

    virtual ~AsioDeadlineTimerFactory();

    virtual const TimerPtr& getTimer(const boost::thread::id& id);
    virtual void stopTimers();

private:
    void initWithPool(const EventLoopPoolPtr& pool);

private:
    typedef std::map<boost::thread::id, TimerPtr> TimerMap;

private:
    TimerMap timers;
};

}
}
}
}

#endif //#if !defined(CETTY_UTIL_INTERNAL_ASIO_ASIODEADLINETIMERFACTORY_H)

// Local Variables:
// mode: c++
// End:
