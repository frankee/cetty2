#if !defined(CETTY_UTIL_TIMERTASK_H)
#define CETTY_UTIL_TIMERTASK_H

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

#include <boost/function.hpp>

namespace cetty {
namespace util {

class Timeout;

/**
 * A task which is executed after the delay specified with
 * {@link Timer#newTimeout(TimerTask, long, TimeUnit)}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

/**
* Executed after the delay specified with
* {@link Timer#newTimeout(const TimerTask&, boost::int64_t, const TimeUnit&)}.
*
* @param timeout a handle which is associated with this task.
*/
typedef boost::function<void (Timeout& timeout)> TimerTask;

#if 0
class TimerTask {
public:
    virtual ~TimerTask() {}

    /**
     * Executed after the delay specified with
     * {@link Timer#newTimeout(TimerTask, long, TimeUnit)}.
     *
     * @param timeout a handle which is associated with this task
     * throw Exception
     */
    virtual void run(Timeout& timeout) = 0;
};
#endif

}
}

#endif //#if !defined(CETTY_UTIL_TIMERTASK_H)

// Local Variables:
// mode: c++
// End:

