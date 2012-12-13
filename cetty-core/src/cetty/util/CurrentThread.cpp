
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

#include <cetty/util/CurrentThread.h>
#include <boost/thread.hpp>

#if defined(CETTY_OS_FAMILY_WINDOWS)
#include <windows.h>
#endif

namespace cetty {
namespace util {

static boost::thread_specific_ptr<ThreadId> currentThreadId;

const ThreadId& CurrentThread::id() {
    ThreadId* id = currentThreadId.get();

    if (!id) {
        id = new ThreadId;
#if defined(CETTY_OS_FAMILY_UNIX)
        *id = pthread_self();
#elif defined(CETTY_OS_FAMILY_WINDOWS)
        *id = ::GetCurrentThreadId();
#endif
        currentThreadId.reset(id);
    }

    return *id;
}

void CurrentThread::sleep(int64_t milliseconds) {
    boost::this_thread::sleep(boost::posix_time::milliseconds(milliseconds));
}

void CurrentThread::yield() {
    boost::this_thread::yield();
}

}
}
