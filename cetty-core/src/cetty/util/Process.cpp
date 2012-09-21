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

#include <cetty/util/Process.h>

#if defined(BOOST_POSIX_API)
#  include <cerrno>
#  include <signal.h>
#elif defined(BOOST_WINDOWS_API)
#  include <windows.h>
#else
#  error "Unsupported platform." 
#endif

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace util {

Process::PID Process::id() {
#if defined(BOOST_WINDOWS_API)
    return ::GetCurrentProcessId();
#elif defined(BOOST_POSIX_API)
    return getpid();
#else
    return 0;
#endif
}

void Process::times(int64_t* userTime, int64_t* kernelTime) {
#if defined(BOOST_WINDOWS_API)
    FILETIME creationFt;
    FILETIME exitFt;
    FILETIME kernelFt;
    FILETIME userFt;

    if (GetProcessTimes(GetCurrentProcess(), &creationFt, &exitFt, &kernelFt, &userFt) != 0) {
        ULARGE_INTEGER time;
        time.LowPart  = kernelFt.dwLowDateTime;
        time.HighPart = kernelFt.dwHighDateTime;

        if (kernelTime) {
            *kernelTime = int64_t(time.QuadPart/10000000L);
        }

        time.LowPart  = userFt.dwLowDateTime;
        time.HighPart = userFt.dwHighDateTime;

        if (userTime) {
            *userTime = int64_t(time.QuadPart/10000000L);
        }
    }
    else {
        *userTime = *kernelTime = -1;
    }

#elif defined(BOOST_POSIX_API)
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    *userTime   = (int64_t)usage.ru_utime.tv_sec;
    *kernelTime = (int64_t)usage.ru_stime.tv_sec;
#endif
}

void Process::terminate(PID pid, bool force /*= false*/) {
#if defined(BOOST_WINDOWS_API)
    HANDLE h = ::OpenProcess(PROCESS_TERMINATE, FALSE, pid);

    if (h) {
        if (::TerminateProcess(h, EXIT_FAILURE) == 0) {
            ::CloseHandle(h);
            LOG_ERROR << "TerminateProcess failed";
        }
        else {
            ::CloseHandle(h);
        }
    }
    else {
        switch (GetLastError()) {
        case ERROR_ACCESS_DENIED:
            //throw NoPermissionException("cannot kill process");
        case ERROR_NOT_FOUND:
            //throw NotFoundException("cannot kill process");
        default:
            //throw SystemException("cannot kill process");
            break;
        }
    }

#elif defined(BOOST_POSIX_API)

    if (::kill(id_, force ? SIGKILL : SIGTERM) == -1) {
        switch (errno) {
        case ESRCH:

            //throw NotFoundException("cannot kill process");
        case EPERM:

            //throw NoPermissionException("cannot kill process");
        default:
            //throw SystemException("cannot kill process");
            break;
        }
    }

#endif
}

}
}
