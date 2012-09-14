#if !defined(CETTY_UTIL_PROCESS_H)
#define CETTY_UTIL_PROCESS_H

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

#include <boost/config.hpp>
#include <boost/cstdint.hpp>
#include <boost/system/config.hpp>

#include <sys/types.h>

namespace cetty {
namespace util {

class Process {
public:
#if defined(BOOST_POSIX_API)
    typedef pid_t PID;
#elif defined(BOOST_WINDOWS_API)
    typedef unsigned long PID;
#endif

public:
    /// Returns the process ID of the current process.
    static PID id();

    /// Returns the number of seconds spent by the
    /// current process in user and kernel mode.
    static void times(boost::int64_t* userTime, boost::int64_t* kernelTime);

    static void terminate(PID pid, bool force = false);

private:
    Process();
    Process(const Process&);
    Process& operator=(const Process&);
};

}
}


#endif //#if !defined(CETTY_UTIL_PROCESS_H)

// Local Variables:
// mode: c++
// End:
