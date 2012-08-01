#if !defined(CETTY_UTIL_TIMERFACTORY_H)
#define CETTY_UTIL_TIMERFACTORY_H

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

#include <cetty/channel/ChannelPtr.h>
#include <cetty/util/TimerPtr.h>

namespace cetty {
namespace util {

using namespace cetty::channel;

class TimerFactory {
public:
    virtual ~TimerFactory() {}

    /**
     * may make sure the timer task, which will be performed in the same
     * thread as the channel, if under multi-io-thread mode.
     * 
     * 具体实现应该尽量保证获取的Timer和设置的channel在同一个线程中。
     * 在基于ASIO的实现中，当设置的channel有效（为AsioSocketChannel或者AsioDatagramChannel）时，
     * Timer和channel使用同一个AsioService，即保证使用Timer的TimerTask和channel在同一个线程中执行。
     * 当channel为空，或者无效时，返回默认的Timer，并无法保证TimerTask与channel在同一线程执行。
     *
     * @exception RuntimeException
     */
    virtual const TimerPtr& getTimer(const ChannelPtr& channel) = 0;

    /**
     *
     */
    virtual void stopTimers() = 0;

public:
    static bool hasFactory();

    /**
     * injection of the implement of the TimerFactory.
     */
    static void setFactory(const TimerFactoryPtr& timerFactory);
    static void resetFactory();
    static void resetFactory(const TimerFactoryPtr& timerFactory);

    static TimerFactory& getFactory();

private:
    static TimerFactoryPtr factory;
};

}
}

#endif //#if !defined(CETTY_UTIL_TIMERFACTORY_H)

// Local Variables:
// mode: c++
// End:
