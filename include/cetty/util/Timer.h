#if !defined(CETTY_UTIL_TIMER_H)
#define CETTY_UTIL_TIMER_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
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

#include <boost/cstdint.hpp>
#include <cetty/util/TimerFwd.h>
#include <cetty/util/TimerTask.h>

namespace cetty {
namespace util {

class TimeUnit;

/**
 * Schedules {@link TimerTask}s for one-time future execution in a background
 * thread.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 * @apiviz.has org.jboss.netty.util.TimerTask oneway - - executes
 * @apiviz.has org.jboss.netty.util.Timeout oneway - - creates
 */

class Timer {
public:
    virtual ~Timer() {}

    /**
     * Schedules the specified {@link TimerTask} for one-time execution after
     * the specified delay.
     *
     * @param task the execution task.
     * @param delay the time duration (milliseconds) expires from now.
     *
     * @return a handle which is associated with the specified task
     *
     * @throws IllegalStateException if this timer has been
     *                               {@link #stop() stopped} already
     */
    virtual const TimeoutPtr& newTimeout(const TimerTask& task, boost::int64_t delay) = 0;

    /**
     * Schedules the specified {@link TimerTask} for one-time execution after
     * the specified delay.
     *
     * @return a handle which is associated with the specified task
     *
     * @throws IllegalStateException if this timer has been
     *                               {@link #stop() stopped} already
     */
    virtual const TimeoutPtr& newTimeout(const TimerTask& task, boost::int64_t delay, const TimeUnit& unit) = 0;

    /**
     * Releases all resources acquired by this {@link Timer} and cancels all
     * tasks which were scheduled but not executed yet.
     */
    virtual void stop() = 0;
};

}
}

#endif //#if !defined(CETTY_UTIL_TIMER_H)

// Local Variables:
// mode: c++
// End:

