#if !defined(CETTY_UTIL_CURRENTTHREAD_H)
#define CETTY_UTIL_CURRENTTHREAD_H

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

#include <cetty/Types.h>

#if defined(CETTY_OS_FAMILY_UNIX)
#include <pthread.h>
#endif

namespace cetty {
namespace util {

#if defined(CETTY_OS_FAMILY_UNIX)
    typedef pthread_t ThreadId;
#elif defined(CETTY_OS_FAMILY_WINDOWS)
    typedef unsigned long ThreadId;
#endif

class CurrentThread {
public:
    /// Suspends the current thread for the specified
    /// amount of time.
    static void sleep(int64_t milliseconds);
    
    /// Yields cpu to other threads.
    static void yield();
    
    /// Returns the native thread ID for the current thread.    
    static const ThreadId& id();

//     static boost::posix_time::ptime now() {
//         return boost::posix_time::microsec_clock::universal_time();
//     }
// 
//     static boost::posix_time::time_duration nowStamp();
// 
//     static boost::posix_time::ptime nowUTC();

private:
    CurrentThread();
    CurrentThread(const CurrentThread&);
};

}
}


#endif //#if !defined(CETTY_UTIL_CURRENTTHREAD_H)

// Local Variables:
// mode: c++
// End:
